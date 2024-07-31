
#ifndef RIVP_APP_RTP_H
#define RIVP_APP_RTP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rivp_ldd_types.h"

#define RTP_PACKET_NUM				1000
#define RTP_PACKET_SIZE             1536

#define NAL_TYPE_IDR    5   /* Instantaneous Decoding Refresh */
#define NAL_TYPE_SPS    7   /* Sequence Parameter Set */
#define NAL_TYPE_PPS    8   /* Picture Parameter Set */
#define NAL_TYPE_AUD    9   /* Access Unit Delimiter */
#define NAL_TYPE_EOSEQ  10  /* End of Sequence */
#define NAL_TYPE_EOSTM  11  /* End of Stream */
#define NAL_TYPE_FU_A   28  /* fragmented unit 0x1C */
#define NAL_TYPE_STAP_A 24  /* single time aggregation packet */

/*RTP Payload Type*/
#define RTP_PT_H264     96

#define RTP_HEADER_SIZE 12
#define RTP_FU_DATA_POS 14


#define FU_NONE         0   /*Single Nal unit packet*/  
#define FU_FIRST        1
#define FU_NFNL         2   /*Not first and Not last*/
#define FU_LAST         3
#define FU_SQSTART		4  /* SPS PPS*/
#define STAP_A          5  /* Single-time aggregation packet type A */
#define FU_ERR          6

typedef struct
{
    RIVP_LDD_U8 CsrcLen:4;
    RIVP_LDD_U8 Extension:1;
    RIVP_LDD_U8 Padding:1;
    RIVP_LDD_U8 Version:2;

    RIVP_LDD_U8 PayLoad:7;
    RIVP_LDD_U8 Marker:1;
    RIVP_LDD_U16 SeqNum;

    RIVP_LDD_U32 Timestamp;
    RIVP_LDD_U32 Ssrc;
} rtp_hdr_t;

typedef struct {
    RIVP_LDD_U8 Type:5;
    RIVP_LDD_U8 Nri:2;
    RIVP_LDD_U8 F:1;
} nalu_hdr_t;

typedef struct {
    RIVP_LDD_U8 Type:5;
    RIVP_LDD_U8 Nri:2;
    RIVP_LDD_U8 F:1;
} fu_indi_t;

typedef struct {
    RIVP_LDD_U8 Type:5;
    RIVP_LDD_U8 R:1;
    RIVP_LDD_U8 E:1;
    RIVP_LDD_U8 S:1;
} fu_hdr_t;

typedef struct
{
    RIVP_LDD_U8 frameIndex;
	RIVP_LDD_U8 frameUse;
    RIVP_LDD_U16 bufIndex;
    RIVP_LDD_PTR data;
    RIVP_LDD_U16 bufSize[RTP_PACKET_NUM];
    RIVP_LDD_U16 bufFlag[RTP_PACKET_NUM];
} RIVP_RTP_FRAME;

extern void *RIVP_AppRtpTask(RIVP_LDD_PTR arg);

#ifdef __cplusplus
}
#endif

#endif /* RTP_H */