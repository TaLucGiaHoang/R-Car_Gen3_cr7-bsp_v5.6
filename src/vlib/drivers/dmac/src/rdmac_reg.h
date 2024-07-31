/*************************************************************************************************************
* dmac_reg_h
* Copyright (c) [2017-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef _RDMAC_REG_H_
#define _RDMAC_REG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Definition of RTDMAC address */
#define DRV_RTDMAC_REG_ADDR_DMAC0           (0xFFC10000U)      /* RTDMAC0 */
#define DRV_RTDMAC_REG_ADDR_DMAC1           (0xFFC20000U)      /* RTDMAC1 */
#define DRV_AUDMAC_REG_ADDR_DMAC0           (0xEC700000U)  /* AUDMAC0 */
#define DRV_AUDMAC_REG_ADDR_DMAC1           (0xEC720000U)  /* AUDMAC1 */

#define DRV_RTDMAC_DSMEM_ADDR_RTDMAC0       (0xE670A000U)      /* DSMEM RTDMAC0 */
#define DRV_RTDMAC_DSMEM_ADDR_RTDMAC1       (0xE730A000U)      /* DSMEM RTDMAC1 */
#define DRV_RTDMAC_DSMEM_ADDR_RTDMAC2       (0xE731A000U)      /* DSMEM RTDMAC2 */

#define DRV_DMAC_REG_MASK_CH0             (0x00000001U)      /* CH0 use bit mask */
#define DRV_DMAC_REG_MASK_CH1             (0x00000002U)      /* CH1 use bit mask */
#define DRV_DMAC_REG_MASK_CH2             (0x00000004U)      /* CH2 use bit mask */
#define DRV_DMAC_REG_MASK_CH3             (0x00000008U)      /* CH3 use bit mask */
#define DRV_DMAC_REG_MASK_CH4             (0x00000010U)      /* CH4 use bit mask */
#define DRV_DMAC_REG_MASK_CH5             (0x00000020U)      /* CH5 use bit mask */
#define DRV_DMAC_REG_MASK_CH6             (0x00000040U)      /* CH6 use bit mask */
#define DRV_DMAC_REG_MASK_CH7             (0x00000080U)      /* CH7 use bit mask */
#define DRV_DMAC_REG_MASK_CH8             (0x00000100U)      /* CH8 use bit mask */
#define DRV_DMAC_REG_MASK_CH9             (0x00000200U)      /* CH9 use bit mask */
#define DRV_DMAC_REG_MASK_CH10            (0x00000400U)      /* CH10 use bit mask */
#define DRV_DMAC_REG_MASK_CH11            (0x00000800U)      /* CH11 use bit mask */
#define DRV_DMAC_REG_MASK_CH12            (0x00001000U)      /* CH12 use bit mask */
#define DRV_DMAC_REG_MASK_CH13            (0x00002000U)      /* CH13 use bit mask */
#define DRV_DMAC_REG_MASK_CH14            (0x00004000U)      /* CH14 use bit mask */
#define DRV_DMAC_REG_MASK_CH15            (0x00008000U)      /* CH15 use bit mask */

#define DRV_DMAC_CH_REG_ADDR              (0x00008000U)      /* CH_REG */
#define DRV_DMAC_CH_REG_SIZE              (0x00000080U)      /* CH_REG */

#define DRV_DMAC_REG_MASK_LOWER_RDMSAR    (0xFFFFFFFFU)      /* RDMSAR use bit lower mask */
#define DRV_DMAC_REG_MASK_UPPER_RDMSAR    (0x000000FFU)      /* RDMSAR use bit upper mask */
#define DRV_DMAC_REG_MASK_LOWER_RDMDAR    (0xFFFFFFFFU)      /* RDMDAR use bit lower mask */
#define DRV_DMAC_REG_MASK_UPPER_RDMDAR    (0x000000FFU)      /* RDMDAR use bit upper mask */
#define DRV_DMAC_REG_MASK_LOWER_RDMDPBASE (0xFFFFFFF0U)      /* RDMDPBASE use bit lower mask */
#define DRV_DMAC_REG_MASK_UPPER_RDMDPBASE (0x000000FFU)      /* RDMDPBASE use bit upper mask */
#define DRV_DMAC_REG_MASK_RDMCHCR_TS      (0x00000003U)      /* RDMCHCR use bit TS */
#define DRV_DMAC_REG_MASK_ADDR            (0x000000FFFFFFFFFFU)      /* ADDRESS(40Bit) mask */
#define DRV_DMAC_REG_MASK_CHECK_ADDR      (0x000000FFFFFFF000U)      /* Check ADDRESS(40Bit) mask */
#define DRV_DMAC_REG_MASK_COUNT           (0x00FFFFFFU)      /* COUNT(24Bit) mask */

#define DRV_DMAC_REG_RDMOR_DME            (0x00000001U)
#define DRV_DMAC_REG_RDMOR_AE             (0x00000004U)
#define DRV_DMAC_REG_RDMCHCR_CAE          (0x80000000U)
#define DRV_DMAC_REG_RDMCHCR_DSE          (0x00080000U)
#define DRV_DMAC_REG_RDMCHCR_DSIE         (0x00040000U)
#define DRV_DMAC_REG_RDMCHCR_TE           (0x00000002U)
#define DRV_DMAC_REG_RDMCHCR_DE           (0x00000001U)
#define DRV_DMAC_REG_RDMCHCR_DPM          (0x30000000U)
#define DRV_DMAC_REG_RDMCHCR_DPM_REP      (0x20000000U)
#define DRV_DMAC_REG_RDMCHCR_DPM_READ     (0x30000000U)

#define DRV_DMAC_REG_PRIO_FIX             0U
#define DRV_DMAC_REG_PRIO_RR              3

#define DRV_DMAC_REG_TRANS_UNIT_1BYTE     0U
#define DRV_DMAC_REG_TRANS_UNIT_2BYTE     1U
#define DRV_DMAC_REG_TRANS_UNIT_4BYTE     2U
#define DRV_DMAC_REG_TRANS_UNIT_8BYTE     7U
#define DRV_DMAC_REG_TRANS_UNIT_16BYTE    3U
#define DRV_DMAC_REG_TRANS_UNIT_32BYTE    4U
#define DRV_DMAC_REG_TRANS_UNIT_64BYTE    5U

#define DRV_DMAC_RDMOR_DME_ENABLE         1U
#define DRV_DMAC_RDMOR_DME_DISABLE        0U
#define DRV_DMAC_RDMOR_AE_CLEAR           0U
#define DRV_DMAC_RDMCHCR_RS_AUTO_REQEST       4U
#define DRV_DMAC_RDMCHCR_RS_EXTENDED_RESOUSE  8U
#define DRV_DMAC_RDMCHCR_CAIE_ENABLE      1U
#define DRV_DMAC_RDMCHCR_CAIE_DISABLE     0U
#define DRV_DMAC_RDMCHCR_IE_ENABLE        1U
#define DRV_DMAC_RDMCHCR_IE_DISABLE       0U
#define DRV_DMAC_RDMCHCR_DE_ENABLE        1U
#define DRV_DMAC_RDMCHCR_DE_DISABLE       0U
#define DRV_DMAC_RDMCHCR_CAE_CLEAR        0U
#define DRV_DMAC_RDMCHCR_DSE_CLEAR        0U
#define DRV_DMAC_RDMCHCR_TE_CLEAR         0U

#define DRV_DMAC_PRIORITY_MAX             16U

#define DRV_DMAC_MAX_CH                     16U

#define DRV_DMAC_CLR_DE_RETRY             5U

enum REGISTER {
    DRV_AUDMAC_REG0,
    DRV_AUDMAC_REG1,
    DRV_RTDMAC_REG0,
    DRV_RTDMAC_REG1,
    DRV_DMAC_UNIT_NUM
};

/* Structure of register access definitions */
/* Structure of RDMISTA */
typedef union regRDMISTA_un {
    struct {
        uint32_t    I0:      1;     /* R Interrupt State in Channel 0 */
        uint32_t    I1:      1;     /* R Interrupt State in Channel 1 */
        uint32_t    I2:      1;     /* R Interrupt State in Channel 2 */
        uint32_t    I3:      1;     /* R Interrupt State in Channel 3 */
        uint32_t    I4:      1;     /* R Interrupt State in Channel 4 */
        uint32_t    I5:      1;     /* R Interrupt State in Channel 5 */
        uint32_t    I6:      1;     /* R Interrupt State in Channel 6 */
        uint32_t    I7:      1;     /* R Interrupt State in Channel 7 */
        uint32_t    I8:      1;     /* R Interrupt State in Channel 8 */
        uint32_t    I9:      1;     /* R Interrupt State in Channel 9 */
        uint32_t    I10:     1;     /* R Interrupt State in Channel 10 */
        uint32_t    I11:     1;     /* R Interrupt State in Channel 11 */
        uint32_t    I12:     1;     /* R Interrupt State in Channel 12 */
        uint32_t    I13:     1;     /* R Interrupt State in Channel 13 */
        uint32_t    I14:     1;     /* R Interrupt State in Channel 14 */
        uint32_t    I15:     1;     /* R Interrupt State in Channel 15 */
        uint32_t    reserve_1:      16;       /* R     Reserved */
    } bit;
    uint32_t INT;
} regRDMISTA_t;

/* Structure of RDMSEC    */
typedef union regRDMSEC_un {
    struct {
        uint32_t    S0:      1;     /* R/W Secure Mode Setting for Channel 0 */
        uint32_t    S1:      1;     /* R/W Secure Mode Setting for Channel 1 */
        uint32_t    S2:      1;     /* R/W Secure Mode Setting for Channel 2 */
        uint32_t    S3:      1;     /* R/W Secure Mode Setting for Channel 3 */
        uint32_t    S4:      1;     /* R/W Secure Mode Setting for Channel 4 */
        uint32_t    S5:      1;     /* R/W Secure Mode Setting for Channel 5 */
        uint32_t    S6:      1;     /* R/W Secure Mode Setting for Channel 6 */
        uint32_t    S7:      1;     /* R/W Secure Mode Setting for Channel 7 */
        uint32_t    S8:      1;     /* R/W Secure Mode Setting for Channel 8 */
        uint32_t    S9:      1;     /* R/W Secure Mode Setting for Channel 9 */
        uint32_t    S10:     1;     /* R/W Secure Mode Setting for Channel 10 */
        uint32_t    S11:     1;     /* R/W Secure Mode Setting for Channel 11 */
        uint32_t    S12:     1;     /* R/W Secure Mode Setting for Channel 12 */
        uint32_t    S13:     1;     /* R/W Secure Mode Setting for Channel 13 */
        uint32_t    S14:     1;     /* R/W Secure Mode Setting for Channel 14 */
        uint32_t    S15:     1;     /* R/W Secure Mode Setting for Channel 15 */
        uint32_t    reserve_1:      16;       /* R     Reserved */
    } bit;
    uint32_t INT;
} regRDMSEC_t;


/* Structure of RDMOR    */
typedef union regRDMOR_un {
    struct {
        uint32_t    DME:        1;  /* R/W    DMA Master Enable */
        uint32_t    reserve_1:  1;  /* R      Reserved */
        uint32_t    AE:         1;  /* R/(W)* Address Error Flag */
        uint32_t    reserve_2:  5;  /* R      Reserved */
        uint32_t    PR:         2;  /* R/W    Priority Mode */
        uint32_t    reserve_3:  6;  /* R      Reserved */
    } bit;
    uint16_t INT;
} regRDMOR_t;

/* Structure of RDMCHCLR    */
typedef union regRDMCHCLR_un {
    struct {
        uint32_t    CLR:        16;  /* W Writing to a bit leads to clearing of all registers for the corresponding */
        uint32_t    reserve_1:  16;  /* R Reserved */
    } bit;
    uint32_t INT;
} regRDMCHCLR_t;


/* Structure of DMDPSEC    */
typedef union regRDMDPSEC_un {
    struct {
        uint32_t    SM:         9;   /* R/W Security Attribute Setting for Base Address Mask of Descriptor */
        uint32_t    reserve_1:  7;   /* R   Reserved */
        uint32_t    SA:         9;   /* R/W Security Attribute Setting for Base Address of Descriptor Memory */
        uint32_t    reserve_2:  6;   /* R   Reserved */
        uint32_t    SEC:        1;   /* R/W Security Attribute Setting for Descriptor Memory */
    } bit;
    uint32_t INT;
} regRDMDPSEC_t;

/* Structure of RDMSAR_0 to RDMSAR_32    */
typedef union regRDMSAR_un {
    struct {
        uint32_t    SAR:      32;    /* R/W DMA Source Address */
    } bit;
    uint32_t INT;
} regRDMSAR_t;

/* Structure of RDMDAR_0 to RDMDAR_32    */
typedef union regRDMDAR_un {
    struct {
        uint32_t    DAR:      32;    /* R/W DMA Destination Address */
    } bit;
    uint32_t INT;
} regRDMDAR_t;

/* Structure of RDMTCR_0 to RDMTCR_32    */
typedef union regRDMTCR_un {
    struct {
        uint32_t    TSR:      32;    /* R/W DMA Transfer Size */
    } bit;
    uint32_t INT;
} regRDMTCR_t;

/* Structure of RDMCHCR_0 to RDMCHCR_32    */
typedef union regRDMCHCR_un {
    struct {
        uint32_t    DE:         1;   /* R/W    DMA Enable */
        uint32_t    TE:         1;   /* R/(W)* Transfer End Flag */
        uint32_t    IE:         1;   /* R/W    Interrupt Enable */
        uint32_t    TS01:       2;   /* R/W    DMA Transfer Size */
        uint32_t    reserve_1:  3;   /* R      Reserved */
        uint32_t    RS:         4;   /* R/W    Resource Selection */
        uint32_t    SM:         2;   /* R/W    Source Address Mode */
        uint32_t    DM:         2;   /* R/W    Destination Address Mode */
        uint32_t    reserve_2:  2;   /* R      Reserved */
        uint32_t    DSIE:       1;   /* R/W    Descriptor Stage End Interrupt Enable */
        uint32_t    DSE:        1;   /* R/(W)* Descriptor Stage End */
        uint32_t    TS23:       2;   /* R/W    DMA Transfer Size */
        uint32_t    DPB:        1;   /* R/W    Descriptor Start */
        uint32_t    reserve_3:  2;   /* R      Reserved */
        uint32_t    RPT0:       1;   /* R/W    Descriptor Setting Update(RPT[0]) */
        uint32_t    RPT1:       1;   /* R/W    Descriptor Setting Update(RPT[1]) */
        uint32_t    RPT2:       1;   /* R/W    Descriptor Setting Update(RPT[2]) */
        uint32_t    DPM:        2;   /* R/W    Operating Mode of Descriptor Memory */
        uint32_t    CAIE:       1;   /* R/W    Channel Address Error Interrupt Enable */
        uint32_t    CAE:        1;   /* R/(W)* Channel Address Error Flag */
    } bit;
    uint32_t INT;
} regRDMCHCR_t;

/* Structure of RDMTCRB_0 to RDMTCRB_32    */
typedef union regRDMTCRB_un {
    struct {
        uint32_t    TCR:          24; /* R/W DMA Transfer Count B */
        uint32_t    reserve_1:    8;  /* R   Reserved */
    } bit;
    uint32_t INT;
} regRDMTCRB_t;

/* Structure of RDMTSRB_0 to RDMTSRB_32    */
typedef union regRDMTSRB_un {
    struct {
        uint32_t    TSR:      32;     /* R/W DMA Transfer Size */
    } bit;
    uint32_t INT;
} regRDMTSRB_t;

/* Structure of RDMCHCRB_0 to RDMCHCRB_32    */
typedef union regRDMCHCRB_un {
    struct {
        uint32_t    PRI:        4;    /* R/W Channel Request Priority Setting */
        uint32_t    SLM:        4;    /* R/W DMA Transfer Low-Speed Mode */
        uint32_t    DTS:        1;    /* R/W Total Size Transmission under Descriptor Control */
        uint32_t    reserve_1:  6;    /* R   Reserved */
        uint32_t    DRST:       1;    /* W   Descriptor Reset */
        uint32_t    DPTR:       8;    /* R   Descriptor Pointer */
        uint32_t    DCNT:       8;    /* R/W Number of Stages of Descriptor Memory */
    } bit;
    uint32_t INT;
} regRDMCHCRB_t;

/* Structure of RDMRS_0 to RDMRS_32    */
typedef union regRDMRS_un {
    struct {
        uint32_t    RID:        2;    /* R/W DMA Request Source Adoption RID[1] and RID[0] (RID) */
        uint32_t    MID:        6;    /* R/W DMA Request Source Adoption MID[5] to MID[0] (MID) */
        uint32_t    reserve_1:  8;    /* R   Reserved */
    } bit;
    uint16_t INT;
} regRDMRS_t;

/* Structure of RDMBUFCR_0 to RDMBUFCR_32    */
typedef union regRDMBUFCR_un {
    struct {
        uint32_t    ULB:        10;   /* R/W Upper Limit on Buffer Size */
        uint32_t    reserve_1:  6;    /* R   Reserved */
        uint32_t    MBU:        9;    /* R/W Maximum Burst dev for SDRAM */
        uint32_t    reserve_2:  7;    /* R   Reserved */
    } bit;
    uint32_t INT;
} regRDMBUFCR_t;

/* Structure of RDMDPBASE_0 to RDMDPBASE_32    */
typedef union regRDMDPBASE_un {
    struct {
        uint32_t    SEL:        1;    /* R/W */
        uint32_t    reserve_1:  3;    /* R   Reserved */
        uint32_t    DPBASE:     28;   /* R/W Base Address of Descriptor Memory */
    } bit;
    uint32_t INT;
} regRDMDPBASE_t;

/* Structure of RDMDPCR_0 to RDMDPCR_32    */
typedef union regRDMDPCR_un {
    struct {
        uint32_t    reserve_1:  24;    /* R   Reserved */
        uint32_t    DIPT:       8;     /* R/W DMA Descriptor Control */
    } bit;
    uint32_t INT;
} regRDMDPCR_t;

/* Structure of RDMFIXSAR_0 to RDMFIXSAR_32    */
typedef union regRDMFIXSAR_un {
    struct {
        uint32_t    SAR:        8;     /* R/W DMA Fixed Source Address */
        uint32_t    reserve_1:  24;    /* R   Reserved */
    } bit;
    uint32_t INT;
} regRDMFIXSAR_t;

/* Structure of RDMFIXDAR_0 to RDMFIXDAR_32    */
typedef union regRDMFIXDAR_un {
    struct {
        uint32_t    DAR:        8;     /* R/W DMA Fixed Destination Address */
        uint32_t    reserve_1:  24;    /* R   Reserved */
    } bit;
    uint32_t INT;
} regRDMFIXDAR_t;

/* Structure of RDMFIXDPBASE_0 to RDMFIXDPBASE_32    */
typedef union regRDMFIXDPBASE_un {
    struct {
        uint32_t    DPBASE:     8;     /* R/W DMA Fixed Descriptor Base Address */
        uint32_t    reserve_1:  24;    /* R   Reserved */
    } bit;
    uint32_t INT;
} regRDMFIXDPBASE_t;

/* Structure of RDMSES    */
typedef union regRDMSES_un {
    struct {
        uint32_t    Error_1:      1;    /* R/W* Error status of channel 31 to 16 */
        uint32_t    reserve_1:    31;   /* R    Reserved */
    } bit;
    uint32_t INT;
} regRDMSES_t;

/* Structure of RDMSEMID    */
typedef union regRDMSEMID_un {
    struct {
        uint32_t    EID:      32;        /* R First error ID when access to channel 16 to 31 was denied. */
    } bit;
    uint32_t INT;
} regRDMSEMID_t;

/* Structure of FDSDM_CONTROL_0 to FDSDM_CONTROL_2    */
typedef union rDmacFDSDM_CONTROL_un {
    struct {
        uint32_t    D_EN:       1;       /* R/W */
        uint32_t    A_EN:       1;       /* R/W */
        uint32_t    reserve_1:  22;      /* R   Reserved */
        uint32_t    DMA_CH1:    4;       /* R/W */
        uint32_t    DMA_CH2:    4;       /* R/W */
    } bit;
    uint32_t INT;
} rDmacFDSDM_CONTROL_t;

/* Structure of FDSDM_STATUS    */
typedef union rDmacFDSDM_STATUS_un {
    struct {
        uint32_t    ST0:        1;       /* R/W */
        uint32_t    ST1:        1;       /* R/W */
        uint32_t    ST2:        1;       /* R/W */
        uint32_t    reserve_1:  29;      /* R   Reserved */
    } bit;
    uint32_t INT;
} rDmacFDSDM_STATUS_t;

#define RDMISTA            0x00000020U
#define RDMSEC             0x00000030U
#define RDMOR              0x00000060U
#define RDMCHCLR           0x00000080U
#define RDMDPSEC           0x000000A0U
#define RDMSES             0x000000C0U
#define RDMSEMID           0x000000C8U
#define FDSDM_CONTROL_0    0x000000D0U
#define FDSDM_CONTROL_1    0x000000D4U
#define FDSDM_CONTROL_2    0x000000D8U
#define FDSDM_STATUS       0x000000E0U

#define RDMSAR             0x00000000U
#define RDMDAR             0x00000004U
#define RDMTCR             0x00000008U
#define RDMCHCR            0x0000000CU
#define RDMFIXSAR          0x00000010U
#define RDMFIXDAR          0x00000014U
#define RDMTCRB            0x00000018U
#define RDMCHCRB           0x0000001CU
#define RDMSAR2            0x00000020U
#define RDMDAR2            0x00000024U
#define RDMTSR             0x00000028U
#define RDMCHCR2           0x0000002CU
#define RDMTSRB            0x00000038U
#define RDMRS              0x00000040U
#define RDMBUFCR           0x00000048U
#define RDMDPBASE          0x00000050U
#define RDMDPCR            0x00000054U
#define RDMFIXDPBASE       0x00000060U

extern const uint32_t rDmacCH_MaskTable[];

extern uint32_t rDmacGet_RDMISTA(uint8_t dev);
extern uint32_t rDmacGet_RDMOR(uint8_t dev);
extern uint32_t rDmacGet_RDMCHCR(uint8_t dev,uint8_t ch);
extern uint32_t rDmacGet_RDMDPCR(uint8_t dev,uint8_t ch);

extern void rDmacUpdate_RDMOR(uint8_t dev, rDmacPriorityMode_t mode);
extern void rDmacUpdate_RDMCHCLR(uint8_t dev,uint8_t ch);
extern void rDmacUpdate_RDMSAR(uint8_t dev,uint8_t ch, uint64_t Value);
extern void rDmacUpdate_RDMDAR(uint8_t dev,uint8_t ch, uint64_t Value);
extern void rDmacUpdate_RDMTCR(uint8_t dev,uint8_t ch, uint32_t Value);
extern void rDmacUpdate_RDMCHCR(uint8_t dev,uint8_t ch, rDmacCfg_t *cfg, rDmacDescCfg_t *descCfg);
extern uint16_t rDmacClear_RDMCHCR_DE(uint8_t dev,uint8_t ch);
extern void rDmacClear_RDMCHCR_CAE(uint8_t dev,uint8_t ch);
extern void rDmacClear_RDMCHCR_DSE(uint8_t dev,uint8_t ch);
extern void rDmacClear_RDMCHCR_TE(uint8_t dev,uint8_t ch);
extern void rDmacUpdate_RDMCHCRB(uint8_t dev,uint8_t ch, rDmacCfg_t *cfg, rDmacDescCfg_t *descCfg);
extern void rDmacUpdate_RDMCHCRB_RST(uint8_t dev, uint8_t ch);
extern void rDmacUpdate_RDMRS_ID(uint8_t dev,uint8_t ch, uint8_t Value);
extern void rDmacUpdate_RDMDPBASE(uint8_t dev,uint8_t ch, uint64_t Value);
extern void rDmacUpdate_RDMDPCR(uint8_t dev,uint8_t ch, uint8_t Value);

#ifdef __cplusplus
}
#endif

#endif  /* _RDMAC_REG_H_ */
