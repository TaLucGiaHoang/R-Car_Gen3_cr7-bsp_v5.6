#include "rivp_ldd_api.h"
#include "rivp_app_ldd_cmn_local.h"
#include "rivp_app_frame.h"
#include "rivp_app_rtp.h"
#include "rivp_app_frame.h"
#include "string.h"

static RIVP_LDD_U8 start_code[4] = {0,0,0,1};

static inline RIVP_LDD_U16 nbo_read_uint16(const RIVP_LDD_U8* ptr)
{
	return (((RIVP_LDD_U16)ptr[0]) << 8) | ptr[1];
}

static inline void rivp_frame_output_frame(st_rivp_rtp_unpacker *unpacker)
{
    static RIVP_LDD_U8 lMsg[RIVP_APP_LDD_SIZE_MAIN_DEC_MSG];
    RIVP_VIDEO_FRAME *video_frame = unpacker->video_frame;
    RIVP_APP_LDD_CMN_PARAM_T *app = unpacker->app;
    RIVP_LDD_BOOL ret;
    RIVP_LDD_RESULT result;

    if (app->firstInput == RIVP_LDD_TRUE) {
        /* wait until RIVP_AppLddDecAvcCbStrmBufFree is called */
        ret = RIVP_AppLddOsalQueueReceive(&app->msgInf[RIVP_APP_LDD_STRMFREE_MSG_QUEUE], &lMsg[0]);
        memcpy(&app->decoding, &lMsg[0], sizeof(app->decoding));
        /* Wait for the callback function to exit completely. */
        RIVP_AppLddOsalUSleep(RIVP_APP_LDD_DEC_AVC_SLEEP_TIME);
    } else {
        /* skip receive process on first decode */
        ret = RIVP_LDD_TRUE;
    }

    if (RIVP_LDD_TRUE == ret) {
        video_frame->frameSize = video_frame->framePos;
        app->input.strmBuff[0].virtAddr = video_frame->data;
        app->input.strmBuff[0].physAddr = (RIVP_LDD_U32)(video_frame->data);
        app->input.strmBuff[0].size = video_frame->frameSize;
        R_OSAL_CacheFlush((RIVP_LDD_U32)video_frame->data, video_frame->frameSize, R_OSAL_CACHE_D);
        app->input.strmBuffNum = 1;
        app->input.strmId = app->input.strmId + 1;
        app->input.strmBuff[1].virtAddr = NULL;
        app->input.strmBuff[1].physAddr = 0;
        app->input.strmBuff[1].size = 0;

        /* input(RIVPD_BSC_User_Manual 4.2.3, 5.5.5) */
        app->firstInput = RIVP_LDD_TRUE;

        RIVP_AppLddCmnGetCurTime(&app->perf.funcStart);
        result = RIVP_LddInput(app->ctx, &app->input);
        RIVP_AppLddCmnGetProcTime(app, RIVP_APP_LDD_FUNC_INPUT);

        if (app->displayMode == RIVP_LDD_FALSE) {
            RIVP_LDD_LOG_APP_INF("RIVP_Input(strmId=%d) = %s\n", app->input.strmId, g_rivpAppLddCmnResCode[(RIVP_LDD_U32)result]);
        }
        if ((result != RIVP_LDD_RTN_OK) && (result != RIVP_LDD_RTN_WARNING)) {
            /* in this case because callback CbError is not notified(such as context of argument is NULL), check the return value for error process. */
            app->errorCode = result;
            RIVP_LDD_LOG_APP_INF("RIVP_Input(strmId=%d) = %s\n", app->input.strmId, g_rivpAppLddCmnResCode[(RIVP_LDD_U32)result]);
        }

        rivp_dec_get_video_frame(unpacker);
    }
}

static inline void rivp_frame_write(st_rivp_rtp_unpacker *unpacker, RIVP_LDD_U8 *ptr, RIVP_LDD_U32 len)
{
    RIVP_VIDEO_FRAME *video_frame = unpacker->video_frame;
    if (video_frame->framePos >= RIVP_APP_VIDEO_FRAME_SIZE) {
        RIVP_LDD_LOG_APP_INF("Stream exceed video frame size.\r\n");
        return;
    }
    memcpy(video_frame->data + video_frame->framePos, ptr, len);
    video_frame->framePos += len;
}

static inline void rivp_frame_handle_nalu(st_rivp_rtp_unpacker *unpacker, RIVP_LDD_U8 *ptr, RIVP_LDD_U32 len)
{
    nalu_hdr_t *nalu_hdr = (nalu_hdr_t*)ptr;
    // if start of new frame is received and previous frame was received
    // send frame to be processed for output
    switch (nalu_hdr->Type) {
    case NAL_TYPE_AUD:
        if (unpacker->video_frame->framePos > 0)
            rivp_frame_output_frame(unpacker);
        return; // AUD can be discarded
    case NAL_TYPE_SPS:
    case NAL_TYPE_EOSEQ:
    case NAL_TYPE_EOSTM:
        if (unpacker->video_frame->framePos > 0)
            rivp_frame_output_frame(unpacker);
        // Fall-through intended so that the NAL unit is written to the new frame
    default:
        rivp_frame_write(unpacker, start_code, sizeof(start_code));
        rivp_frame_write(unpacker, ptr, len);
        break;
    }
    switch (nalu_hdr->Type) {
    case NAL_TYPE_EOSEQ:
    case NAL_TYPE_EOSTM:
    case NAL_TYPE_IDR:
        rivp_frame_output_frame(unpacker);
        break;
    default:
        break;
    }
}

static inline void rivp_frame_handle_stap(st_rivp_rtp_unpacker *unpacker, RIVP_LDD_U8 *ptr, RIVP_LDD_U32 len)
{
    RIVP_LDD_U16 nal_len;
    ptr += 1;
    for (len -= 1; len > 2; len -= nal_len + 2) {
        nal_len = nbo_read_uint16(ptr);
        ptr += 2;
        if (((nal_len + 2U) > len) || (nal_len < 2U)) {
            RIVP_LDD_LOG_APP_INF("Invalid STAP-A format\r\n");
            return;
        }
        rivp_frame_handle_nalu(unpacker, ptr, nal_len);
        ptr += nal_len;
    }
}

static inline void rivp_frame_handle_fu(st_rivp_rtp_unpacker *unpacker, RIVP_LDD_U8 *ptr, RIVP_LDD_U32 len)
{
    fu_indi_t *fu_indi;
    fu_hdr_t *fu_hdr;
    RIVP_LDD_U8 l_fu_indi;

    fu_indi = (fu_indi_t*)ptr;
    ptr += 1;
    fu_hdr = (fu_hdr_t*)ptr;
    ptr += 1;

    if (fu_hdr->S) {
        rivp_frame_write(unpacker, start_code, sizeof(start_code));
        l_fu_indi = 0;
        l_fu_indi |= fu_indi->F << 7;
        l_fu_indi |= fu_indi->Nri << 5;
        l_fu_indi |= fu_hdr->Type;
        rivp_frame_write(unpacker, &l_fu_indi, sizeof(l_fu_indi));
    }

    if (len > 2) {
        rivp_frame_write(unpacker, ptr, len - 2);
    }

    if (fu_hdr->E) {
        rivp_frame_output_frame(unpacker);
    }
}

void rivp_frame_read_packets(st_rivp_rtp_unpacker *unpacker)
{
    RIVP_LDD_U8 *ptr;
    RIVP_LDD_U32 len;
    nalu_hdr_t *nalu_hdr;
    RIVP_RTP_FRAME *rtp_frame = unpacker->rtp_frame;

    for (int i = 0; i < rtp_frame->bufIndex; i++) {
        ptr = rtp_frame->data + (i * RTP_PACKET_SIZE);
        len = rtp_frame->bufSize[i];
        ptr += RTP_HEADER_SIZE;
        len -= RTP_HEADER_SIZE;
        nalu_hdr = (nalu_hdr_t*)ptr;
        switch (nalu_hdr->Type) {
        case 0:
        case 31:
            // undefined NAL types
            return;

        case 25:
        case 26:
        case 27:
        case 29:
            RIVP_LDD_LOG_APP_INF("Unsupported NAL type %d\r\n", nalu_hdr->Type);
            break;
        
        case 24: // STAP-A
            rivp_frame_handle_stap(unpacker, ptr, len);
            break;
        case 28: // FU-A
            rivp_frame_handle_fu(unpacker, ptr, len);
            break;
        default: // 1-23 NAL unit
            rivp_frame_handle_nalu(unpacker, ptr, len);
            break;
        }
    }
}
