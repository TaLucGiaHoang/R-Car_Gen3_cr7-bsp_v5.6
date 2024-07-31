/*************************************************************************************************************
* dmac_common
* Copyright (c) [2017-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef DMAC_COMMON_H
#define DMAC_COMMON_H


#include <stdbool.h>

/***********************************************************
Macro definitions
***********************************************************/
#define DRV_DMAC_EXT_NUM_DESC    128   /* The number of external descriptor memory */
#define DRV_DMAC_INT1_NUM_DESC   128   /* The number of internal descriptor memory */
#define DRV_DMAC_INT2_NUM_DESC   128   /* The number of internal descriptor memory */

#define rDmacCH_VALID   (0x000000FF00FF0000U)

/***********************************************************
Typedef definitions
***********************************************************/
typedef enum rDmacPriorityMode_en {
    DRV_DMAC_PRIO_FIX,
    DRV_DMAC_PRIO_RR,
    DRV_DMAC_PRIO_MAX
} rDmacPriorityMode_t;

typedef enum rDmacDmaMode_en {
    DRV_DMAC_DMA_NO_DESCRIPTOR,
    DRV_DMAC_DMA_DESC_NORMAL,
    DRV_DMAC_DMA_DESC_REPEAT,
    DRV_DMAC_DMA_DESC_READOUT,
    DRV_DMAC_DMA_DESC_INFINITE,
    DRV_DMAC_DMA_MODE_MAX
} rDmacDmaMode_t;

typedef enum rDmacAddrMode_en {
    DRV_DMAC_ADDR_FIXED,
    DRV_DMAC_ADDR_INCREMENTED,
    DRV_DMAC_ADDR_DECCREMENTED,
    DRV_DMAC_ADDR_MAX
} rDmacAddrMode_t;

typedef enum rDmacTransferUnit_en {
    DRV_DMAC_TRANS_UNIT_1BYTE,
    DRV_DMAC_TRANS_UNIT_2BYTE,
    DRV_DMAC_TRANS_UNIT_4BYTE,
    DRV_DMAC_TRANS_UNIT_8BYTE,
    DRV_DMAC_TRANS_UNIT_16BYTE,
    DRV_DMAC_TRANS_UNIT_32BYTE,
    DRV_DMAC_TRANS_UNIT_64BYTE,
    DRV_DMAC_TRANS_UNIT_MAX
} rDmacTransferUnit_t;

typedef enum rDmacResource_en {
    DRV_DMAC_MEMORY,
    DRV_MAC_SEND_TPU0,
    DRV_DMAC_RESOUCE_MAX
} rDmacResource_t;

typedef enum rDmacLowSpeedMode_en {
    DRV_DMAC_SPEED_NORMAL,
    DRV_DMAC_INTERVAL_256,
	DRV_DMAC_INTERVAL_512,
	DRV_DMAC_INTERVAL_1024,
	DRV_DMAC_INTERVAL_2048,
	DRV_DMAC_INTERVAL_4096,
	DRV_DMAC_INTERVAL_8192,
	DRV_DMAC_INTERVAL_16384,
	DRV_DMAC_INTERVAL_32768,
    DRV_DMAC_SPEED_MAX
} rDmacLowSpeedMode_t;

typedef enum rDmacReason_en {
    DRV_DMAC_STAGE_END,
    DRV_DMAC_TRANSFER_END,
    DRV_DMAC_ADDRESS_ERROR
} rDmacReason_t;

typedef struct rDmacCfg_st {
    uintptr_t mSrcAddr;
    uintptr_t mDestAddr;
    uint32_t mTransferCount;
    rDmacDmaMode_t mDMAMode;
    rDmacAddrMode_t mSrcAddrMode;
    rDmacAddrMode_t mDestAddrMode;
    rDmacTransferUnit_t mTransferUnit;
    rDmacResource_t mResource;
    rDmacLowSpeedMode_t mLowSpeed;
    uint8_t mPrioLevel;
} rDmacCfg_t;

typedef struct rDmacDescUpdate_st {
    bool mSrcAddrUpdate;
    bool mDestAddrUpdate;
    bool mTransCountUpdate;
} rDmacDescUpdate_t;

typedef struct rDmacDescCfg_st {
    uintptr_t mDescBaseAddr;
    rDmacDescUpdate_t mDescUpdate;
    bool mDescRead1st;
    bool mStateEndEnable;
    uint8_t mDescCount;
    uint8_t mDescIndex;
} rDmacDescCfg_t;

/* Type definition of the callback function */
typedef void (* rDmacCallBack_t)(uint8_t chNo, rDmacReason_t reason,
				    uint8_t descIndex, bool completed, uintptr_t arg);

#endif  /* RTDMAC_COMMON_H */

