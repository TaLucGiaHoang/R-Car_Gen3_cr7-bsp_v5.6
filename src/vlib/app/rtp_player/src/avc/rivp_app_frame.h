
#ifndef RIVP_APP_FRAME_H
#define RIVP_APP_FRAME_H

#ifdef __cplusplus
extern "C" {
#endif
#include "rivp_ldd_types.h"
#include "rivp_app_rtp.h"

#define RIVP_APP_LDD_DEC_AVC_SLEEP_TIME			(500u)
#define RIVP_APP_VIDEO_FRAME_SIZE				(1566720)
#define RIVP_APP_VIDEO_FRAME_NUM				(3)

typedef struct
{
    RIVP_LDD_U8  frameUse;
    RIVP_LDD_U32 frameSize;
    RIVP_LDD_U32 framePos;
    RIVP_LDD_PTR data;
} RIVP_VIDEO_FRAME;

typedef struct {
    RIVP_APP_LDD_CMN_PARAM_T *app;
    RIVP_RTP_FRAME *rtp_frame;
    RIVP_VIDEO_FRAME *video_frame;
} st_rivp_rtp_unpacker;

void rivp_dec_get_video_frame(st_rivp_rtp_unpacker *unpacker);
void rivp_frame_read_packets(st_rivp_rtp_unpacker *unpacker);
#ifdef __cplusplus
}
#endif

#endif 