/*************************************************************************************************************
* cio_vin_main_h
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/**@defgroup vinmain1 VIN Driver main
 * @ingroup cio_vin_driver
 *
 * @brief CIO VIN Driver main module
 *
 * Typedefs, macros and functions that are accesed by other VIN driver modules.
 * The main module also contains error and interrupt handling functions.
 *
 * @{
 */
#ifndef R_CIO_VIN_MAIN_H
#define R_CIO_VIN_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Section: Includes
*/
#include "r_cio_vin_public.h"
#include "r_vin_api.h"
#include "rcar-xos/osal/r_osal.h"


/*******************************************************************************
  Section: VIN driver internal defines
*/
#define R_CIO_VIN_MAX_INSTANCE_NUM            (8)
#define R_CIO_VIN_MAX_CONSUMER_PER_INSTANCE   (4)

#define TIMEOUT_MS 1000 /* 1000 milisecond */

/**@enum R_CIO_VIN_PRV_State_t
 * @brief states of the VIN
 */
typedef enum {
    R_CIO_VIN_STATE_UNINIT,     /**< uninit */
    R_CIO_VIN_STATE_CONFIGURED, /**< configured */
    R_CIO_VIN_STATE_STARTED,    /**< started */
    R_CIO_VIN_STATE_STOPPED     /**< stopped */
} R_CIO_VIN_PRV_State_t;

/**@struct  R_CIO_VIN_RPV_Consumer_t
 * @brief Consumer of Vin
 */
typedef struct {
    osal_thread_handle_t ThreadId; /**< osal thred handle */
    osal_mq_handle_t * mq_handle;  /**< osal mq handle */
    uint32_t IsCapturing;          /**< 0:not capturing, 1:capturing */
} R_CIO_VIN_RPV_Consumer_t;

/**@struct  R_CIO_VIN_PRV_Instance_t
 * @brief Consumer of Vin
 */
typedef struct {
    uint32_t VinIdx;             /**< Vin Idx */
    R_CIO_VIN_PRV_State_t State; /**< State @ref R_CIO_VIN_PRV_State_t */

    volatile int32_t IrqQuit;    /**< IRQ Quit */
    int32_t IrqNum;              /**< Interrupt ID of VIN global */

    r_cio_vin_Config_t Config;   /**< Config @ref r_cio_vin_Config_t */
    r_vin_Config_t Setting;      /**< Setting of VIN @ref r_vin_Config_t */

    uint32_t ConsumerCnt;        /**< Consumer count  */
    R_CIO_VIN_RPV_Consumer_t Consumer[R_CIO_VIN_MAX_CONSUMER_PER_INSTANCE]; /**< Consumer @ref R_CIO_VIN_RPV_Consumer_t */
    osal_mutex_handle_t mtx_handle;     /**< osal mutex handle */
    osal_device_handle_t device_handle; /**< osal device handle */
    uint32_t Error;                     /**< Error */
} R_CIO_VIN_PRV_Instance_t;


/*******************************************************************************
  Section: Global functions
*/


/*******************************************************************************
 * Function Name: R_CIO_VIN_PRV_Instance_t
 */
/**
 * @brief  Find the one matching the given base addres in the local list of
 *  initialised instances
 *
 *  @param[in] Addr     - Channel number of the given VIN instance
 *
 *  @return  >0 - pointer to the instance
 *  @return ==0 - NG
 *
 */
R_CIO_VIN_PRV_Instance_t *R_CIO_VIN_PRV_FindInstance(size_t Addr);

/*******************************************************************************
 * Function Name: R_CIO_VIN_RPV_FindConsumer
 */
/**
 * @brief  Find the consumer
 *
 *  @param[in] *pInst    - pointer of instance
 *  @param[in] pThread   - osal thread handle
 *
 *  @return >=0  - idx
 *  @return ==-1 - NG
 *
 */
int32_t R_CIO_VIN_RPV_FindConsumer(R_CIO_VIN_PRV_Instance_t *pInst, osal_thread_handle_t pThread);

/*******************************************************************************
   Function: R_CIO_VIN_PRV_Init
 */
/**
 * @brief  VIN internal initialization
 *
 * Initialize one VIN driver instance and VIN device instance (global and specified channel).
 * Initialize the hardware VIN interrupts.
 *
 * @param[in] Addr       - Channel number of the given VIN instance
 * @param[in] IrqNum     - Interrupt ID of VIN global
 *
 * @return R_CIO_VIN_ERR_OK
 * @return R_CIO_VIN_ERR_FAILED
 */
int R_CIO_VIN_PRV_Init(size_t Addr, int IrqNum);

/*******************************************************************************
   Function: R_CIO_VIN_PRV_DeInit
 */
/**
 * @brief  VIN internal de-initialization
 *
 * De-initialize one VIN driver instance and release the system resource.
 *
 * @param[in] Addr       - Channel number of the given VIN instance
 * @param[in] IrqNum     - Interrupt ID of VIN global
 *
 * @return R_CIO_VIN_ERR_OK
 * @return R_CIO_VIN_ERR_FAILED
 */
int R_CIO_VIN_PRV_DeInit(size_t Addr);

extern const r_cio_Driver_t R_CIO_VIN_Driver;

#ifdef __cplusplus
}
#endif

#endif /* R_CIO_VIN_MAIN_H */
