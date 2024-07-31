/**
 * @file
 * RTP client/server module
 *
 */

/*
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived lFrom this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 */

#include "rivp_app_ldd_cmn_local.h"
#include "rivp_app_ldd_mmgr.h"
#include "rivp_app_rtp.h"
#include "r_print_api.h"
#include "r_osal_api.h"

#include "lwip/opt.h"
#include "lwipopts.h"

#if LWIP_SOCKET && LWIP_IGMP /* don't build if not configured for use in lwipopts.h */

#include "lwip/sys.h"
#include "lwip/sockets.h"
#include <string.h>

#define RTP_RECV_TASK_PRIORITY  180
/** RTP stream port */
#ifndef RTP_STREAM_PORT
#define RTP_STREAM_PORT             4000
#endif

#define H264_PACKET_SIZE            2000

#define RTP_STREAM_ADDRESS          inet_addr("192.168.0.20")
#define RTP_RECV_TIMEOUT            2000
#define RTP_RECV_STATS              50
#define RTP_RECV_PROCESSING(p,s)

#define MAX_VIDEO_FRAME_NUM         3

extern osal_memory_manager_handle_t g_hndl_mmngr;

static RIVP_RTP_FRAME loc_Frame[MAX_VIDEO_FRAME_NUM] =
{
    {0, 0, 0, NULL, {0}, {0}},
    {1, 0, 0, NULL, {0}, {0}},
    {2, 0, 0, NULL, {0}, {0}}
};


static RIVP_LDD_S32 loc_GetEmptyFrame(void)
{
    RIVP_LDD_S32 i = -1;
    for(i = 0; i < MAX_VIDEO_FRAME_NUM; i++) {
        if(0 == loc_Frame[i].frameUse) {
            return i;
        }
    }
    return -1;
}

static RIVP_LDD_S32 loc_GetFrameFromAddr(RIVP_LDD_U32 addr)
{
    RIVP_LDD_S32 i = -1;
    for(i = 0; i < MAX_VIDEO_FRAME_NUM; i++) {
        if(addr == (RIVP_LDD_U32)&loc_Frame[i]) {
            return i;
        }
    }
    return -1;
}

#if 0
static void printf_rtp_header(rtp_hdr_t *hdr)
{
    R_PRINT_Log("V:%x X:%x P:%x S:%x T:%X\r\n", hdr->Version, hdr->Extension, hdr->PayLoad, PP_HTONS(hdr->SeqNum), hdr->Timestamp);
}
#endif

RIVP_LDD_BOOL loc_IsSqStart(RIVP_LDD_U8* rtpBuf)
{
    nalu_hdr_t  *lNaluHdr;
    lNaluHdr =  (nalu_hdr_t *)&rtpBuf[RTP_HEADER_SIZE];
    return (lNaluHdr->Type  == NAL_TYPE_SPS) ? RIVP_LDD_TRUE : RIVP_LDD_FALSE;

}

RIVP_LDD_BOOL loc_IsSqStop(RIVP_LDD_U8* rtpBuf)
{
    nalu_hdr_t  *lNaluHdr;
    lNaluHdr =  (nalu_hdr_t *)&rtpBuf[RTP_HEADER_SIZE];
    return (lNaluHdr->Type  == 10) ? RIVP_LDD_TRUE : RIVP_LDD_FALSE;

}

RIVP_LDD_U16 loc_GetRtpFlag(RIVP_LDD_U8* rtpBuf, RIVP_LDD_S32 recvBytes)
{
    rtp_hdr_t   *lRtpHdr;
    nalu_hdr_t  *lNaluHdr;
    fu_hdr_t    *lFuHdr;
    RIVP_LDD_U16 lFlag = FU_ERR;
    lNaluHdr =  (nalu_hdr_t *)&rtpBuf[RTP_HEADER_SIZE];
    if (lNaluHdr->Type == 0) {
        R_PRINT_Log("error nal header\r\n");
        return lFlag;
    } else if (lNaluHdr->Type > 0 && lNaluHdr->Type < NAL_TYPE_STAP_A) {
        if(lNaluHdr->Type == NAL_TYPE_IDR) {
            lFlag = FU_NONE;
        } else {
            lFlag = FU_SQSTART;
        }
    } else if (lNaluHdr->Type == NAL_TYPE_FU_A) {
        /* Fragmentation unit */
        lFuHdr = (fu_hdr_t *)&rtpBuf[RTP_HEADER_SIZE + 1];
        lRtpHdr = (rtp_hdr_t *)rtpBuf;
        if (lRtpHdr->Marker == 1)
        {
            lFlag = FU_LAST;
        } else if (lRtpHdr->Marker == 0) {
            /*Fu not last packet*/
            if(lFuHdr->S == 1) {
                lFlag = FU_FIRST;
            } else {
                /*Fu not first  packet*/
                lFlag = FU_NFNL;
            }
        }
    } else if (lNaluHdr->Type == NAL_TYPE_STAP_A) {
        lFlag = STAP_A;
    } else {
        R_PRINT_Log("packet not support. Type=%d\r\n", lNaluHdr->Type);
        return lFlag;
    }
    return lFlag;
}

static void loc_SendFrame(RIVP_APP_LDD_CMN_PARAM_T*    lAppPtr, RIVP_LDD_U8 lFrameIndex)
{
    RIVP_LDD_BOOL               lBoolRet;
    RIVP_LDD_U32                lFrameAddr;
    loc_Frame[lFrameIndex].frameUse = 1;
    lFrameAddr = (RIVP_LDD_U32)&loc_Frame[lFrameIndex];
    lBoolRet = RIVP_AppLddOsalQueueSend(&lAppPtr->msgInf[RIVP_APP_LDD_RTP_MSG_QUEUE], (RIVP_LDD_PTR)&lFrameAddr);
    if(lBoolRet != RIVP_LDD_TRUE) {
        R_PRINT_Log("%s err\r\n", __func__);
    }
    return;
}
/**
 * RTP recv thread
 */
void *RIVP_AppRtpTask(RIVP_LDD_PTR arg)
{
    RIVP_LDD_S32              lSock;
    struct sockaddr_in        lLocal;
    struct sockaddr_in        lFrom;
    RIVP_LDD_S32              lFromLen;
    RIVP_LDD_S32              lResult;
    RIVP_LDD_S32              lFlag = -1;
    RIVP_LDD_S32              lFrameIndex = -1;
    RIVP_APP_LDD_CMN_PARAM_T* lAppPtr;
    RIVP_LDD_U32              lFrameAddr;
    RIVP_LDD_BOOL             lSeqStart = RIVP_LDD_FALSE;
    rtp_hdr_t*                lRtpHdr;
    RIVP_LDD_U8*              lRecvBuf;
    RIVP_LDD_U16*             lRecvSize;
    RIVP_LDD_U16              lBufIndex;
    RIVP_LDD_U8*              lBufFlag;

    if (arg != RIVP_LDD_NULL) {

        for (int i = 0; i < MAX_VIDEO_FRAME_NUM; i++) {
            loc_Frame[i].data = RIVP_AppLddMmgrAlloc(RTP_PACKET_NUM * RTP_PACKET_SIZE, 64);
            if (RIVP_LDD_NULL == loc_Frame[i].data) {
                RIVP_LDD_LOG_APP_ERR_SYS("Failed to allocate receive packet buffer.\n");
                lResult = RIVP_LDD_RTN_FATAL_SYSTEM;
            }
        }

        lAppPtr = (RIVP_APP_LDD_CMN_PARAM_T*)arg;
        /* create new socket */
        lSock = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        /* prepare lLocal address */
        memset(&lLocal, 0, sizeof(lLocal));
        lLocal.sin_family      = AF_INET;
        lLocal.sin_port        = PP_HTONS(RTP_STREAM_PORT);
        lLocal.sin_addr.s_addr = PP_HTONL(INADDR_ANY);

        /* bind to lLocal address */
        if (bind(lSock, (struct sockaddr *)&lLocal, sizeof(lLocal)) < 0) {
            R_PRINT_Log("bind rtp socket error \r\n");
            return NULL;
        }
        lFromLen = sizeof(lFrom);

        /* receive RTP packets */
        while(1) {
            lFrameIndex = loc_GetEmptyFrame();
            if(lFrameIndex < 0) {
                RIVP_AppLddOsalQueueReceive(&lAppPtr->msgInf[RIVP_APP_LDD_RTPFREE_MSG_QUEUE], &lFrameAddr);
                lFrameIndex = loc_GetFrameFromAddr(lFrameAddr);
                if(lFrameIndex != -1) {
                    loc_Frame[lFrameIndex].bufIndex = 0;
                    loc_Frame[lFrameIndex].frameUse = 0;
                } else {
                    R_PRINT_Log("get free frame error %08x \n", lFrameAddr);
                    continue;
                }
            }
            lRecvBuf = loc_Frame[lFrameIndex].data + (loc_Frame[lFrameIndex].bufIndex * RTP_PACKET_SIZE);
            lBufIndex = loc_Frame[lFrameIndex].bufIndex;
            lRecvSize = (RIVP_LDD_U16*)&(loc_Frame[lFrameIndex].bufSize[lBufIndex]);
            lBufFlag = (RIVP_LDD_U8*)&(loc_Frame[lFrameIndex].bufFlag[lBufIndex]);
            lResult  = lwip_recvfrom(lSock, lRecvBuf, RTP_PACKET_SIZE, 0, (struct sockaddr *)&lFrom, (socklen_t *)&lFromLen);
            if (lResult < RTP_HEADER_SIZE) {
                continue;
            }

            lRtpHdr = (rtp_hdr_t *)lRecvBuf;
            if(RTP_PT_H264 == lRtpHdr->PayLoad) {
                lFlag = loc_GetRtpFlag(lRecvBuf, lResult);
                switch (lFlag) {
                case FU_NONE:
                case FU_LAST:
                    loc_Frame[lFrameIndex].bufIndex++;
                    *lRecvSize = lResult;
                    *lBufFlag = lFlag;
                    loc_SendFrame(lAppPtr, lFrameIndex);
                    break;
                case STAP_A:
                case FU_SQSTART:
                case FU_FIRST:
                case FU_NFNL:
                    loc_Frame[lFrameIndex].bufIndex++;
                    *lRecvSize = lResult;
                    *lBufFlag = lFlag;
                    if (loc_Frame[lFrameIndex].bufIndex >= RTP_PACKET_NUM) {
                        loc_SendFrame(lAppPtr, lFrameIndex);
                    }
                    break;
                default:
                    break;
                }
            }
        }

        /* close the socket */
        lwip_close(lSock);
    }
    return NULL;
}

#endif /* LWIP_SOCKET && LWIP_IGMP */
