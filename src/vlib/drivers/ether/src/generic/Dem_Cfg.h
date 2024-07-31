/*============================================================================*/
/* Project      = R-CarGen3 AUTOSAR MCAL Development Project                  */
/* Module       = Dem_Cfg.h                                                   */
/*============================================================================*/
/*                                  COPYRIGHT                                 */
/*============================================================================*/
/* Copyright(c) 2015-2019 Renesas Electronics Corporation                     */
/*============================================================================*/
/* Purpose:                                                                   */
/* This file contains the stub values for Dem_Cfg.h                           */
/*                                                                            */
/*============================================================================*/
/*                                                                            */
/* Unless otherwise agreed upon in writing between your company and           */
/* Renesas Electronics Corporation the following shall apply!                 */
/*                                                                            */
/* Warranty Disclaimer                                                        */
/*                                                                            */
/* There is no warranty of any kind whatsoever granted by Renesas. Any        */
/* warranty is expressly disclaimed and excluded by Renesas, either expressed */
/* or implied, including but not limited to those for non-infringement of     */
/* intellectual property, merchantability and/or fitness for the particular   */
/* purpose.                                                                   */
/*                                                                            */
/* Renesas shall not have any obligation to maintain, service or provide bug  */
/* fixes for the supplied Product(s) and/or the Application.                  */
/*                                                                            */
/* Each User is solely responsible for determining the appropriateness of     */
/* using the Product(s) and assumes all risks associated with its exercise    */
/* of rights under this Agreement, including, but not limited to the risks    */
/* and costs of program errors, compliance with applicable laws, damage to    */
/* or loss of data, programs or equipment, and unavailability or              */
/* interruption of operations.                                                */
/*                                                                            */
/* Limitation of Liability                                                    */
/*                                                                            */
/* In no event shall Renesas be liable to the User for any incidental,        */
/* consequential, indirect, or punitive damage (including but not limited     */
/* to lost profits) regardless of whether such liability is based on breach   */
/* of contract, tort, strict liability, breach of warranties, failure of      */
/* essential purpose or otherwise and even if advised of the possibility of   */
/* such damages. Renesas shall not be liable for any services or products     */
/* provided by third party vendors, developers or consultants identified or   */
/* referred to the User by Renesas in connection with the Product(s) and/or   */
/* the Application.                                                           */
/*                                                                            */
/*============================================================================*/
/* Environment:                                                               */
/*              Devices:        R-Car V3M/V3H                                 */
/*                              R-Car H3/M3                                   */
/*============================================================================*/
/*
 * 1.0.0      30-Oct-2015      Initial Version
 * 1.1.0      30-Sep-2016     Added DemConf_DemEventParameter_DemEventParameter6
 *                            DemConf_DemEventParameter_DemEventParameter7
 *                            DemConf_DemEventParameter_DemEventParameter10
 * 1.1.1      04-Nov-2016     Add DemConf_DemEventParameter_DemEventParameter11
 *                            DemConf_DemEventParameter_DemEventParameter12
 *                            DemConf_DemEventParameter_DemEventParameter13
 *                            DemConf_DemEventParameter_DemEventParameter14
 * 1.1.2      05-Apr-2017     Add DemConf_DemEventParameter_DemEventParameter15
 *                            DemConf_DemEventParameter_DemEventParameter16
 *                            DemConf_DemEventParameter_DemEventParameter17
 *                            DemConf_DemEventParameter_DemEventParameter18
 *                            DemConf_DemEventParameter_DemEventParameter19
 * 1.1.3      11-Sep-2017     Added
 *             DemConf_DemEventParameter_CDDICCOM_E_INTERRUPT_CTL_CHECK
 *             DemConf_DemEventParameter_CDDICCOM_E_UNINTENDED_MODULE_STOP_CHECK
 * 1.1.4      18-Dec-2017     Added
 *             DemConf_DemEventParameter_CAN_E_TX_HISTORY_OVERFLOW
 * 1.1.5      04-Jan-2018     Update device name
 * 1.1.6      31-Jan-2018     Add device name M3 about comment.
 *                            Added
 *                       DemConf_DemEventParameter_FLS_E_CPG_GET_CONTROL_FAILURE
 *                       DemConf_DemEventParameter_FLS_E_CLOCK_SET_FAILURE
 * 1.1.7      20-Mar-2018    Change GPT_E_CONFIGURATION_CHECK_TRANSITION_FAILURE
 *                            to GPT_E_CONFIGURATION_CHECK_TRANSIENT_FAILURE.
 * 1.1.8      21-Mar-2018     Added
 *         DemConf_DemEventParameter_MCU_E_CONFIGURATION_CHECK_TRANSIENT_FAILURE
 * 1.1.9      22-Mar-2018     Added
 *             DemConf_DemEventParameter_DemEventParameter21
 *             DemConf_DemEventParameter_ETH_E_AVB_DMAC
 *
 * 1.1.10      28-Mar-2018     Added
 *             DemConf_DemEventParameter_SPI_E_UNINTENDED_EXECUTION_FAILURE
 *             DemConf_DemEventParameter_SPI_E_DMA_TRANSFER_COUNT_FAILURE
 * 1.1.11      06-Apr-2018     Remove
 *             DemConf_DemEventParameter_CAN_E_TX_HISTORY_OVERFLOW
 * 1.1.12      21-Jun-2018     Change
 *                            from FLS_E_CONFIGURATION_CHECK_TRANSITION_FAILURE
 *                            to FLS_E_CONFIGURATION_CHECK_TRANSIENT_FAILURE
 * 1.1.13      25-Jul-2018    Add DemConf_DemEventParameter_MCU_E_CHECK_FAILURE
 * 1.1.14      06-Oct-2018    Added
 *                            DemConf_DemEventParameter_MCU_E_TIMEOUT_FAILURE
 * 1.1.15      07-Dec-2018    - Remove
 *         DemConf_DemEventParameter_FLS_E_CONFIGURATION_CHECK_PERMANENT_FAILURE
 *         DemConf_DemEventParameter_FLS_E_CONFIGURATION_CHECK_TRANSIENT_FAILURE
 *                            - Change
 *         from DemConf_DemEventParameter_FLS_E_SAFETY_WRITE_VERIFY_FAILURE
 *         to DemConf_DemEventParameter_FLS_E_CPG_WRITE_VERIFY_FAILURE
 * 1.1.16      20-Dec-2018    Remove:
 *             DemConf_DemEventParameter_CDDRFSO_E_TIMEOUT_OPERATION
 *             DemConf_DemEventParameter_CDDRFSO_E_INTERVAL_OPERATION
 *                            Add:
 *           DemConf_DemEventParameter_CDDRFSO_E_CFG_REG_CHECK_PERMANENT_FAILURE
 *           DemConf_DemEventParameter_CDDRFSO_E_CFG_REG_CHECK_TRANISENT_FAILURE
 *
 * 1.1.17      14-Jan-2019    - Add
 *         DemConf_DemEventParameter_CDDIPMMU_E_CFG_REG_CHECK_TRANSIENT_FAILURE
 *         DemConf_DemEventParameter_CDDIPMMU_E_CFG_REG_CHECK_PERMANENT_FAILURE
 * 1.1.18      22-Apr-2019    - Add
 *         DemConf_DemEventParameter_CDDCMT_E_WRITE_COUNTER_FAILURE
 *         DemConf_DemEventParameter_CDDCMT_E_WRITE_VERIFY_FAILURE
 *         DemConf_DemEventParameter_CDDCMT_E_INTERRUPT_CONTROLLER_FAILURE
 *         DemConf_DemEventParameter_CDDCMT_E_TIMEOUT_FAILURE
 * 1.1.19      19-May-2019  Update dem event parameter for IIC driver
 * 1.1.20      07-Jun-2019    Added
 *         DemConf_DemEventParameter_CDDIIC_E_NON_ACKNOWLEDGEMENT
 *             DemConf_DemEventParameter_FR_E_CTRL_TESTRESULT
 *             DemConf_DemEventParameter_FR_E_LPDU_SLOTSTATUS
 *             DemConf_DemEventParameter_FR_E_READ_TIMEOUT_FAILURE
 *             DemConf_DemEventParameter_FR_E_REG_WRITE_VERIFY
 *
 * 1.1.21      26-Jun-2019    - Add
 *         DemConf_DemEventParameter_WDG_E_RCLKOSCILLATING_FAILURE
 */

#ifndef DEM_CFG_H
#define DEM_CFG_H
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/

/*******************************************************************************
**                      Version Information                                   **
*******************************************************************************/
/* AUTOSAR release version information */
#define DEM_CFG_AR_RELEASE_MAJOR_VERSION     4
#define DEM_CFG_AR_RELEASE_MINOR_VERSION     2
#define DEM_CFG_AR_RELEASE_REVISION_VERSION  2

/* Module Software version information */
#define DEM_CFG_SW_MAJOR_VERSION  4
#define DEM_CFG_SW_MINOR_VERSION  0


/*******************************************************************************
**                      Global Symbols                                        **
*******************************************************************************/
#define DemConf_DemEventParameter_DemEventParameter         (Dem_EventIdType)1

#define DemConf_DemEventParameter_DemEventParameter0         (Dem_EventIdType)1

#define DemConf_DemEventParameter_DemEventParameter1         (Dem_EventIdType)2

#define DemConf_DemEventParameter_DemEventParameter2         (Dem_EventIdType)3

#define DemConf_DemEventParameter_DemEventParameter3         (Dem_EventIdType)4

#define DemConf_DemEventParameter_DemEventParameter4         (Dem_EventIdType)5

#define DemConf_DemEventParameter_DemEventParameter5         (Dem_EventIdType)6

#define DemConf_DemEventParameter_DemEventParameter6         (Dem_EventIdType)7

#define DemConf_DemEventParameter_DemEventParameter7         (Dem_EventIdType)8

#define DemConf_DemEventParameter_DemEventParameter8         (Dem_EventIdType)9

#define DemConf_DemEventParameter_DemEventParameter9         (Dem_EventIdType)10

#define DemConf_DemEventParameter_DemEventParameter10        (Dem_EventIdType)11

#define DemConf_DemEventParameter_DemEventParameter11        (Dem_EventIdType)12

#define DemConf_DemEventParameter_DemEventParameter12        (Dem_EventIdType)13

#define DemConf_DemEventParameter_DemEventParameter13        (Dem_EventIdType)14

#define DemConf_DemEventParameter_DemEventParameter14        (Dem_EventIdType)15

#define DemConf_DemEventParameter_DemEventParameter15        (Dem_EventIdType)16

#define DemConf_DemEventParameter_DemEventParameter16        (Dem_EventIdType)17

#define DemConf_DemEventParameter_DemEventParameter17        (Dem_EventIdType)18

#define DemConf_DemEventParameter_DemEventParameter18        (Dem_EventIdType)19

#define DemConf_DemEventParameter_DemEventParameter19        (Dem_EventIdType)20

#define DemConf_DemEventParameter_DemEventParameter20        (Dem_EventIdType)21

#define DemConf_DemEventParameter_DemEventParameter21        (Dem_EventIdType)22

#define DemConf_DemEventParameter_DemEventParameter_001     (Dem_EventIdType)2

#define DemConf_DemEventParameter_DemEventParameter_002     (Dem_EventIdType)3

#define DemConf_DemEventParameter_DemEventParameter_003     (Dem_EventIdType)4

#define DemConf_DemEventParameter_DemEventParameter_004     (Dem_EventIdType)5

#define DemConf_DemEventParameter_DemEventParameter_005     (Dem_EventIdType)6

#define DemConf_DemEventParameter_DemEventParameter_006     (Dem_EventIdType)7

#define DemConf_DemEventParameter_DemEventParameter_007     (Dem_EventIdType)8

#define DemConf_DemEventParameter_DemEventParameter_008     (Dem_EventIdType)9

#define DemConf_DemEventParameter_DemEventParameter_009     (Dem_EventIdType)10

#define DemConf_DemEventParameter_DemEventParameter_010     (Dem_EventIdType)11

#define DemConf_DemEventParameter_DemEventParameter_011     (Dem_EventIdType)12

#define DemConf_DemEventParameter_DemEventParameter_012     (Dem_EventIdType)13

#define DemConf_DemEventParameter_DemEventParameter_013     (Dem_EventIdType)14

#define DemConf_DemEventParameter_DemEventParameter_014     (Dem_EventIdType)15

#define DemConf_DemEventParameter_DemEventParameter_015     (Dem_EventIdType)16

/* SPI DEM error handlers */
#define DemConf_DemEventParameter_SPI_E_HARDWARE_ERROR      (Dem_EventIdType)1

#define DemConf_DemEventParameter_SPI_E_DATA_TX_TIMEOUT_FAILURE \
                                                            (Dem_EventIdType)2

#define DemConf_DemEventParameter_SPI_E_DMA_TRANSFER_COUNT_FAILURE \
                                                            (Dem_EventIdType)3

#define DemConf_DemEventParameter_SPI_E_INTERRUPT_CONTROLLER_FAILURE \
                                                            (Dem_EventIdType)4

#define DemConf_DemEventParameter_SPI_E_UNINTENDED_EXECUTION_FAILURE \
                                                            (Dem_EventIdType)5

#define DemConf_DemEventParameter_SPI_E_UNINTENDED_MODULE_STOP_FAILURE \
                                                            (Dem_EventIdType)6

#define DemConf_DemEventParameter_SPI_E_WRITE_VERIFY_FAILURE \
                                                            (Dem_EventIdType)7

/* DIO DEM error handlers */
#define DemConf_DemEventParameter_DIO_E_GET_CONTROL_FAILURE \
                                                            (Dem_EventIdType)3

/* MCU DEM error handlers */
#define DemConf_DemEventParameter_MCU_E_CLOCK_FAILURE       (Dem_EventIdType)1

#define DemConf_DemEventParameter_MCU_E_WRITE_TIMEOUT_FAILURE \
                                                            (Dem_EventIdType)2

#define DemConf_DemEventParameter_MCU_E_GET_CONTROL_FAILURE   \
                                                            (Dem_EventIdType)3

#define DemConf_DemEventParameter_MCU_E_READ_BACK_FAILURE   (Dem_EventIdType)4

#define DemConf_DemEventParameter_MCU_E_CONFIGURATION_CHECK_PERMANENT_FAILURE \
                                                            (Dem_EventIdType)5

#define DemConf_DemEventParameter_MCU_E_CONFIGURATION_CHECK_TRANSITION_FAILURE\
                                                            (Dem_EventIdType)6
#define DemConf_DemEventParameter_MCU_E_CONFIGURATION_CHECK_TRANSIENT_FAILURE\
                                                            (Dem_EventIdType)6

#define DemConf_DemEventParameter_MCU_E_INTERNAL_BUS_FAILURE  \
                                                            (Dem_EventIdType)7
#define DemConf_DemEventParameter_MCU_E_CHECK_FAILURE       (Dem_EventIdType)8

#define DemConf_DemEventParameter_MCU_E_TIMEOUT_FAILURE     (Dem_EventIdType)9
/* WDG DEM error handlers */
#define DemConf_DemEventParameter_WDG_E_DISABLE_REJECTED    (Dem_EventIdType)1

#define DemConf_DemEventParameter_WDG_E_MODE_FAILED         (Dem_EventIdType)2

#define DemConf_DemEventParameter_WDG_E_WRITE_REGISTER_FAILED \
                                                            (Dem_EventIdType)3

#define DemConf_DemEventParameter_WDG_E_WRITEVERIFY_FAILURE  \
                                                            (Dem_EventIdType)4
#define DemConf_DemEventParameter_WDG_E_VALUE_COUNTER_FAILED  \
                                                            (Dem_EventIdType)5
#define DemConf_DemEventParameter_WDG_E_RCLKOSCILLATING_FAILURE  \
                                                            (Dem_EventIdType)6

/* PORT DEM error handlers */
#define DemConf_DemEventParameter_PORT_E_GET_CONTROL_FAILURE \
                                                            (Dem_EventIdType)5

#define DemConf_DemEventParameter_PORT_E_WRITE_VERIFY_FAILURE \
                                                            (Dem_EventIdType)6

#define DemConf_DemEventParameter_PORT_E_CONFIGURATION_CHECK_TRANSIENT_FAILURE\
                                                            (Dem_EventIdType)7

#define DemConf_DemEventParameter_PORT_E_CONFIGURATION_CHECK_PERMANENT_FAILURE \
                                                            (Dem_EventIdType)8

#define DemConf_DemEventParameter_PORT_E_FUSE_MONITORING_FAILURE \
                                                            (Dem_EventIdType)9

/* PORT DEM error handlers */
#define DemConf_DemEventParameter_GPT_E_UNINTENDED_MODULE_STOP_FAILURE \
                                                            (Dem_EventIdType)4

#define DemConf_DemEventParameter_GPT_E_INTERRUPT_CONTROLLER_FAILURE \
                                                            (Dem_EventIdType)5

#define DemConf_DemEventParameter_GPT_E_WRITE_VERIFY_FAILURE \
                                                            (Dem_EventIdType)6

#define DemConf_DemEventParameter_GPT_E_CONFIGURATION_CHECK_PERMANENT_FAILURE \
                                                            (Dem_EventIdType)7

#define DemConf_DemEventParameter_GPT_E_CONFIGURATION_CHECK_TRANSIENT_FAILURE \
                                                            (Dem_EventIdType)8
/* CAN DEM error handlers */
#define DemConf_DemEventParameter_CAN_E_REG_READ_VERIFY       (Dem_EventIdType)1

#define DemConf_DemEventParameter_CAN_E_REG_WRITE_VERIFY      (Dem_EventIdType)2

#define DemConf_DemEventParameter_CAN_E_TIMEOUT_FAILURE       (Dem_EventIdType)3

#define DemConf_DemEventParameter_CAN_E_UNINTENDED_INTERRUPT  (Dem_EventIdType)4

#define DemConf_DemEventParameter_CAN_E_UNINTENDED_MODULE_STOP \
                                                              (Dem_EventIdType)5


/* ETH DEM error handlers */
#define DemConf_DemEventParameter_ETH_E_ACCESS               (Dem_EventIdType)1

#define DemConf_DemEventParameter_ETH_E_ALIGNMENT            (Dem_EventIdType)2

#define DemConf_DemEventParameter_ETH_E_CRC                  (Dem_EventIdType)3

#define DemConf_DemEventParameter_ETH_E_DESC_DATA            (Dem_EventIdType)4

#define DemConf_DemEventParameter_ETH_E_FRAME_RECEIVE        (Dem_EventIdType)5

#define DemConf_DemEventParameter_ETH_E_INTERRUPT_CTL_CHECK  (Dem_EventIdType)6

#define DemConf_DemEventParameter_ETH_E_LATECOLLISION        (Dem_EventIdType)7

#define DemConf_DemEventParameter_ETH_E_MODULE_STOP_CHECK    (Dem_EventIdType)8

#define DemConf_DemEventParameter_ETH_E_MULTIPLECOLLISION    (Dem_EventIdType)9

#define DemConf_DemEventParameter_ETH_E_OVERSIZEFRAME        (Dem_EventIdType)10

#define DemConf_DemEventParameter_ETH_E_PAYLOAD_CRC          (Dem_EventIdType)11

#define DemConf_DemEventParameter_ETH_E_REG_READ_VERIFY      (Dem_EventIdType)12

#define DemConf_DemEventParameter_ETH_E_REG_WRITE_VERIFY     (Dem_EventIdType)13

#define DemConf_DemEventParameter_ETH_E_RESIDUAL_BIT_FRAME   (Dem_EventIdType)14

#define DemConf_DemEventParameter_ETH_E_RX_FRAMES_LOST       (Dem_EventIdType)15

#define DemConf_DemEventParameter_ETH_E_SINGLECOLLISION      (Dem_EventIdType)16

#define DemConf_DemEventParameter_ETH_E_TIMEOUT              (Dem_EventIdType)17

#define DemConf_DemEventParameter_ETH_E_TIMERINC_FAILED      (Dem_EventIdType)18

#define DemConf_DemEventParameter_ETH_E_TRANSMIT_RETRY_OVER  (Dem_EventIdType)19

#define DemConf_DemEventParameter_ETH_E_UNDERSIZEFRAME       (Dem_EventIdType)20

#define DemConf_DemEventParameter_ETH_E_OPMODE               (Dem_EventIdType)21

#define DemConf_DemEventParameter_ETH_E_AVB_DMAC             (Dem_EventIdType)22

/* THS DEM error handlers */
#define DemConf_DemEventParameter_CDD_THS_E_INTERRUPT_CONTROLLER_FAILURE \
                                                             (Dem_EventIdType)1

#define DemConf_DemEventParameter_CDD_THS_E_WRITEVERIFY_FAILURE \
                                                             (Dem_EventIdType)2

/* To be written in Autosar mindset */

/* ICCOM DEM error handlers */
#define DemConf_DemEventParameter_CDDICCOM_E_FATAL \
                                                            (Dem_EventIdType)1

#define DemConf_DemEventParameter_CDDICCOM_E_INIT_NEGOTIATION \
                                                            (Dem_EventIdType)2
#define DemConf_DemEventParameter_CDDICCOM_E_INTERRUPT_CTL_CHECK \
                                                            (Dem_EventIdType)3

#define DemConf_DemEventParameter_CDDICCOM_E_TIMEOUT \
                                                            (Dem_EventIdType)4
#define DemConf_DemEventParameter_CDDICCOM_E_UNINTENDED_MODULE_STOP_CHECK \
                                                            (Dem_EventIdType)5
#define DemConf_DemEventParameter_CDDICCOM_E_WRITE_VERIFY \
                                                            (Dem_EventIdType)6
#define DemConf_DemEventParameter_CDDICCOM_E_INVALID_ACK \
                                                            (Dem_EventIdType)7

/* IPMMU DEM error handlers */
#define DemConf_DemEventParameter_CDDIPMMU_E_INTERRUPT_CONTROLLER_FAILURE \
                                                            (Dem_EventIdType)1

#define DemConf_DemEventParameter_CDDIPMMU_E_WRITE_VERIFY \
                                                            (Dem_EventIdType)2

#define DemConf_DemEventParameter_CDDIPMMU_E_CFG_REG_CHECK_TRANSIENT_FAILURE \
                                                            (Dem_EventIdType)3

#define DemConf_DemEventParameter_CDDIPMMU_E_CFG_REG_CHECK_PERMANENT_FAILURE \
                                                            (Dem_EventIdType)4

/* RFSO DEM error handlers */
#define DemConf_DemEventParameter_CDDRFSO_E_INTERRUPT_CONTROLLER_FAILURE \
                                                            (Dem_EventIdType)1

#define DemConf_DemEventParameter_CDDRFSO_E_WRITE_VERIFY \
                                                            (Dem_EventIdType)2

#define DemConf_DemEventParameter_CDDRFSO_E_CFG_REG_CHECK_PERMANENT_FAILURE \
                                                            (Dem_EventIdType)3

#define DemConf_DemEventParameter_CDDRFSO_E_CFG_REG_CHECK_TRANSIENT_FAILURE \
                                                            (Dem_EventIdType)4

/* ADC DEM error handlers */
#define DemConf_DemEventParameter_ADC_E_WRITEVERIFY_FAILURE \
                                                            (Dem_EventIdType)4

/* To be written in Autosar mindset */

/* ICCOM DEM error handlers */
#define DemConf_DemEventParameter_CDDCRC_E_WRITE_VERIFY \
                                                            (Dem_EventIdType)1

/* To be written in Autosar mindset */
#define ETH_TIMERINC_FAILED 0xF0

/* EMM DEM error handlers */
#define DemConf_DemEventParameter_CDDEMM_E_REG_WRITE_VERIFY \
  (Dem_EventIdType)1

#define DemConf_DemEventParameter_CDDEMM_E_UNINTENDED_MODULE_STOP_CHECK \
  (Dem_EventIdType)2

#define DemConf_DemEventParameter_CDDEMM_E_UNINTENDED_INTERRUPT_CHECK \
  (Dem_EventIdType)3

#define DemConf_DemEventParameter_CDDEMM_E_CONFIGURATION_CHECK_PERMANENT_FAILURE \
  (Dem_EventIdType)4

#define DemConf_DemEventParameter_CDDEMM_E_CONFIGURATION_CHECK_TRANSIENT_FAILURE \
  (Dem_EventIdType)5

#define DemConf_DemEventParameter_CDDEMM_E_INTERNAL_BUS_FAILURE \
  (Dem_EventIdType)6

#define DemConf_DemEventParameter_CDDEMM_E_FUSE_MONITORING_REGISTER \
  (Dem_EventIdType)7

#define DemConf_DemEventParameter_CDDEMM_E_GET_CONTROL_FAILURE \
  (Dem_EventIdType)8

/* FLS DEM error handlers */
#define DemConf_DemEventParameter_FLS_E_GET_CONTROL_FAILURE \
                                                            (Dem_EventIdType)7
#define DemConf_DemEventParameter_FLS_E_GET_SEMAPHORE_FAILURE \
                                                            (Dem_EventIdType)8
#define DemConf_DemEventParameter_FLS_E_RELEASE_SEMAPHORE_FAILURE \
                                                            (Dem_EventIdType)9
#define DemConf_DemEventParameter_FLS_E_WRITE_VERIFY_FAILURE (Dem_EventIdType)6
#define DemConf_DemEventParameter_FLS_E_CPG_GET_CONTROL_FAILURE \
                                                            (Dem_EventIdType)10
#define DemConf_DemEventParameter_FLS_E_CLOCK_SET_FAILURE \
                                                            (Dem_EventIdType)11
#define DemConf_DemEventParameter_FLS_E_CPG_WRITE_VERIFY_FAILURE \
                                                            (Dem_EventIdType)12
/* CDD IIC DEM error handlers */
#define DemConf_DemEventParameter_CDDIIC_E_NON_ACKNOWLEDGEMENT \
                                                            (Dem_EventIdType)1
#define DemConf_DemEventParameter_CDDIIC_E_REGISTER_WRITE_VERIFY \
                                                            (Dem_EventIdType)2
#define DemConf_DemEventParameter_CDDIIC_E_UNINTENDED_INTERRUPT_CHECK \
                                                            (Dem_EventIdType)3
/* FR DEM error handlers*/
#define DemConf_DemEventParameter_FR_E_CTRL_TESTRESULT \
  (Dem_EventIdType)1
#define DemConf_DemEventParameter_FR_E_LPDU_SLOTSTATUS \
  (Dem_EventIdType)2
#define DemConf_DemEventParameter_FR_E_READ_TIMEOUT_FAILURE \
  (Dem_EventIdType)3
#define DemConf_DemEventParameter_FR_E_REG_WRITE_VERIFY \
  (Dem_EventIdType)4

/* CDD CMT DEM error handlers */
#define DemConf_DemEventParameter_CDDCMT_E_WRITE_COUNTER_FAILURE \
                                                            (Dem_EventIdType)1
#define DemConf_DemEventParameter_CDDCMT_E_WRITE_VERIFY_FAILURE \
                                                            (Dem_EventIdType)2
#define DemConf_DemEventParameter_CDDCMT_E_INTERRUPT_CONTROLLER_FAILURE \
                                                            (Dem_EventIdType)3
#define DemConf_DemEventParameter_CDDCMT_E_TIMEOUT_FAILURE \
                                                            (Dem_EventIdType)4


/* BUSMONITOR DEM error handlers */
#define DemConf_DemEventParameter_CDDBUSMONITOR_E_WRITE_VERIFY \
                                                            (Dem_EventIdType)1

#define DemConf_DemEventParameter_CDDBUSMONITOR_E_BANKSWITCH_OVERRUN \
                                                            (Dem_EventIdType)2
#endif /* DEM_CFG_H */

/*******************************************************************************
**                      End of File                                           **
*******************************************************************************/
