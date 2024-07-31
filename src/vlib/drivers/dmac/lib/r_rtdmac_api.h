/*************************************************************************************************************
* dmac_rtdmac_api_h
* Copyright (c) [2019-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : r_rtdmac_api.h
 * Version      : 1.0
 * Description  :
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 05.08.2020 1.00     First Release
 *         : 08.11.2021          Delete R_RTDMAC_SetMemory function.
 *********************************************************************************************************************/
#ifndef R_RTDMAC_API_H_
#define R_RTDMAC_API_H_
#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdint.h>
#include "rcar-xos/osal/r_osal.h"
#include "rtdmac.h"

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
/**
 * Function R_RTDMAC_Initialize
 * @brief Initialize DMAC
 *
 * @retval  0 if successful
 */
extern uint32_t R_RTDMAC_Initialize(void);

/**
 * Function R_RTDMAC_DeInitialize
 * @brief DeInitialize DMAC
 *
 * @retval  0 if successful
 */
extern uint32_t R_RTDMAC_DeInitialize(void);

/**
 * Function R_RTDMAC_CopyMemory
 * @brief Copy vin frame to specified buffer
 *
 * @param[in]  dst  - dst data address.(must not be NULL)
 * @param[in]  src  - src data address.(must not be NULL)
 * @param[in]  len  - Set copy size (0x00000004 - 0xffffffff)
 * @retval     0 if successful
 */
extern uint32_t R_RTDMAC_CopyMemory(void* dst, void* src, uint32_t len);

/**********************************************************************************************************************
Typedef definitions
 *********************************************************************************************************************/
typedef struct {
    uint8_t             mUnit;
    uint8_t             mSubCh;
    osal_mq_handle_t    mMqHandle;
} R_RTDMAC_Handle_t;

typedef struct {
    R_RTDMAC_Dev_t      mUseRtdmac;
    uint8_t             mUseCh;
} R_RTDMAC_AllocateCh_t;

#ifdef __cplusplus
}
#endif
#endif /* R_RTDMAC_API_H_ */
