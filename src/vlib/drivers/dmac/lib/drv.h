/*************************************************************************************************************
* dmac_drv
* Copyright (c) [2017-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#ifndef _DRV_H_
#define _DRV_H_

/* Return code of API */
#define drv_OK                                (0U)    /* API completed without any error. */
#define drv_FAIL                              (1U)    /* Failed. */
#define drv_INVALID_SPI                       (2U)    /* Specified SPI number is not available. */
#define drv_NULL_FUNC                         (3U)    /* Callback function is not specified. */
#define drv_INVALID_TARGET                    (4U)    /* Invalid target CPU  for the interrupt was specified. */
#define drv_INVALID_PRIO                      (5U)    /* Invalid priority  for the interrupt is specified. */
#define drv_IS_ENABLED                        (6U)    /* Specified SPI number was already enabled. */
#define drv_NOT_SETUP_SPI                     (7U)    /* Specified SPI number is not registered. */
#define drv_COMSTART                          (8U)    /* Communication has been started. */
#define drv_COMNOTSTART                       (9U)    /* Communication has not been started. */

#define drv_ERR_PARAM                         (101U)  /* Parameter error */
#define drv_ERR_NOT_INITIALIZED               (102U)  /* API called before the driver initialized. */
#define drv_ERR_NOT_OPENED                    (103U)  /* API called before the driver opened. */
#define drv_ERR_OPENED                        (104U)
#define drv_ERR_ALLREADY_OPENED               (105U)  /* The driver re-opened while it is already opened */
#define drv_ERR_ALREADY_RUNNING               (106U)  /* The driver re-run while it is already running */
#define drv_ERR_DETECTED_UNEXPECTED_INTERRUPT (107U)  /* Unexpected interrupt has been detected */
#define drv_ERR_AL_DETECTED                   (108U)  /* Arbitration lost in I2C has occurred */
#define drv_ERR_NACK_RECEIVED                 (109U)  /* NACK in I2C has occurred */
#define drv_ERR_UNUSABLE_CH                   (110U)  /* Invalid channel was specified */
#define drv_ERR_NOT_RUNNING_YET               (111U)  /* API was called before the driver had been running. */
#define drv_ERR_CLOCK_START                   (112U)  /* Clock supply could not be started. */
#define drv_ERR_CLOCK_STOP                    (113U)  /* Clock supply could not be stopped. */
#define drv_ERR_ALREADY_INITIALIZED           (114U)  /* Initialization API called while the driver is already
                                                         initialized */
#define drv_ERR_NOT_RUNNING                   (115U)  /* Not running before */
#define drv_ERR_CANNOT_ENABLE_INTERRUPT       (116U)
#define drv_ERR_OS_SEMAPHORE                  (117U)  /* Detected semaphore error from OS */
#define drv_ERR_BUF_OVER_FLOW                 (118U)  /* Buffer overflow */
#define drv_ERR_INTERNAL                      (199U)

#define drv_DMAC_CH_INVALID                 (301U)  /* Invalid channel was specified. */
#define drv_DMAC_CH_BUSY                    (302U)  /* Specified channel is used. */
#define drv_DMAC_CH_NO_FREE                 (303U)  /* No allocation channel. */
#define drv_DMAC_DESC_NO_FREE               (304U)  /* Descriptor memory could not be allocated. */
#define drv_DMAC_ADDRESS_INVALID            (305U)  /* Specified address could not be found from DDRM table. */
#define drv_DMAC_NOT_STOPPED                (306U)  /* Not stoprd RTDMAC */

#define DRV_DU_ERR_PARAM                      (201U)  /* Illegal parameters were specified. */
#define DRV_DU_ERR_DISPLAYPOSITION_SETTING    (202U)  /* Illegal parameters were specified for display
                                                         position and size. */
#define DRV_DU_ERR_ALREADY_INITIALIZED        (203U)  /* Already initialized. */
#define DRV_DU_ERR_NOT_INITIALIZED            (204U)  /* Not initialized before. */
#define DRV_DU_ERR_ALREADY_OPEN               (205U)  /* Already opened. */
#define DRV_DU_ERR_NOT_OPEN                   (206U)  /* Not opened before. */

#endif  /* _DRV_H_ */
