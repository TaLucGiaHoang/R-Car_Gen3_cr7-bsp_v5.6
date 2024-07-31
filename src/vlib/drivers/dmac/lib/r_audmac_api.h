/*************************************************************************************************************
* dmac_audmac_api_h
* Copyright (c) [2022-2023] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : r_audmac_api.h
 * Version      : 1.0
 * Description  :
 *********************************************************************************************************************/
#ifndef R_AUDMAC_API_H_
#define R_AUDMAC_API_H_
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>
#include "rcar-xos/osal/r_osal.h"
#include "audmac.h"

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
/**
 * Function R_AUDMAC_Initialize
 * @brief Initialize DMAC
 *
 * @retval  0 if successful
 */
extern uint32_t R_AUDMAC_Initialize(void);

/**
 * Function R_AUDMAC_DeInitialize
 * @brief DeInitialize DMAC
 *
 * @retval  0 if successful
 */
extern uint32_t R_AUDMAC_DeInitialize(void);

/**
 * Function R_AUDMAC_CopyMemory
 * @brief Copy vin frame to specified buffer
 *
 * @param[in]  dst  - dst data address.(must not be NULL)
 * @param[in]  src  - src data address.(must not be NULL)
 * @param[in]  len  - Set copy size (0x00000004 - 0xffffffff)
 * @retval     0 if successful
 */
extern uint32_t R_AUDMAC_CopyMemory(void* dst, void* src, uint32_t len, rDmacResource_t resource, uintptr_t arg);

/**********************************************************************************************************************
Typedef definitions
 *********************************************************************************************************************/
typedef struct {
    uint8_t             mUnit;
    uint8_t             mSubCh;
    osal_mq_handle_t    mMqHandle;
} R_AUDMAC_Handle_t;

typedef struct {
    R_AUDMAC_Dev_t      mUseAudmac;
    uint8_t             mUseCh;
} R_AUDMAC_AllocateCh_t;

#ifdef __cplusplus
}
#endif
#endif /* R_AUDMAC_API_H_ */
