/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
#include "rcar-xos/osal/r_osal.h"
#include "ether_api.h"
#include "Eth_Irq.h"
#include "r_avb_api.h"
#include "EthIf_Cbk.h"
#include "r_netif_api.h"
#include "r_print_api.h"
#include "SchM_Eth.h"

#include "Os.h"

#define ETH_PHY_TRCV  0
#define ETH_INVALID_TRCV_IDX  32
#define ETH_PHY_TRCV_CTRL_REG  0
#define ETH_PHY_TRCV_STAT_REG  1
#define ETH_PHY_TRCV_1000T_CTRL_REG  9
#define ETH_INVALID_REG_IDX 32
#define ETH_REG_VALUE   (uint16_t)0x1300
#define ETH_AUTO_NEGOTIATION_ACTIVE 0x28
#define ETH_LINK_UP 0x4

#define ETH_TIMEOUT (uint32_t)0x20000

#define ETH_MII_KSZ9031_MOD_DATA_NO_POST_INC  0x4000
#define PHY_MDD_ACCES_CTRL_REG  0x0D
#define PHY_MDD_ACCES_DATA_REG  0x0E
#define LOC_PRIO_THREAD_ETH_MF 195

volatile uint8_t EthModeActiveCnt;
volatile uint8_t EthModeDownCnt;


#if (ETH_CTRL_ENABLE_MII == STD_ON)
static int R_NetIf_PhyExtWrite(uint16_t devAddr, uint16_t regNum, uint16_t mode, uint16_t val)
{
    uint16_t LusEthRegValue = ETH_ZERO;
    /*select register addr for mmd*/
    Eth_WriteMii(0x00, 0x00, PHY_MDD_ACCES_CTRL_REG, devAddr);
    Eth_ReadMii(0x00, 0x00, PHY_MDD_ACCES_CTRL_REG, &LusEthRegValue);
    if(LusEthRegValue != devAddr)
    {
        return R_NETIF_ERR_FAILED;
    }
    /*select register for mmd*/
    Eth_WriteMii(0x00, 0x00, PHY_MDD_ACCES_DATA_REG, regNum);
    Eth_ReadMii(0x00, 0x00, PHY_MDD_ACCES_DATA_REG, &LusEthRegValue);
    if(LusEthRegValue != regNum)
    {
        return R_NETIF_ERR_FAILED;
    }
    /*setup mode*/
    Eth_WriteMii(0x00, 0x00, PHY_MDD_ACCES_CTRL_REG, (mode | devAddr));
    Eth_ReadMii(0x00, 0x00, PHY_MDD_ACCES_CTRL_REG, &LusEthRegValue);
    if(LusEthRegValue != (mode | devAddr))
    {
        return R_NETIF_ERR_FAILED;
    }
    /*write the value*/
    Eth_WriteMii(0x00, 0x00, PHY_MDD_ACCES_DATA_REG, val);
    Eth_ReadMii(0x00, 0x00, PHY_MDD_ACCES_DATA_REG, &LusEthRegValue);
    if(LusEthRegValue != val)
    {
        return R_NETIF_ERR_FAILED;
    }

    return 0;
}

static int R_NetIf_ClockPadSkewSetup(void)
{
    /* Setting (900+600ps delay) */
    return R_NetIf_PhyExtWrite(0x0002, 0x0008,
        ETH_MII_KSZ9031_MOD_DATA_NO_POST_INC, (0x0f<<5) | 0x19);
}
#endif

static void * MainFunctionTask(void * arg)
{
    while(1) {
        R_OSAL_ThreadSleepForTimePeriod(10);
        Eth_MainFunction();
    }
    return 0;
}

int R_NetIf_HwInit(void)
{
    /* Initialize Os Counter */
    // Os_Counter_Init();
    /* rcar ether-avb pfc and cpg init*/
    R_AVB_Init();
    /* Reset ETH external PHY */
    R_AVB_PhyExtReset();

    return R_NETIF_ERR_OK;
}

#define RESOURCE_ID_BSP             0x8000U
#define OSAL_THREAD_R_CAR_ETHER_MT  (RESOURCE_ID_BSP   +  98U)

int R_NetIf_HwStart(void)
{
    Eth_ModeType LenEthTestCtrlMode = ETH_MODE_DOWN;
    st_osal_thread_config_t thread_config;
    osal_thread_handle_t thread;

    thread_config.priority   = OSAL_THREAD_PRIORITY_TYPE12;
    thread_config.task_name  = "EthMainFuncTask";
    thread_config.func       = (p_osal_thread_func_t)MainFunctionTask;
    thread_config.userarg    = (void*)0;
    thread_config.stack_size = 0x4000;

    /* Invoke Eth_Init API with valid configuration pointer */
    Eth_Init(Eth_GstConfiguration);
    /* Set mode to ACTIVE */
    Eth_SetControllerMode(EthConf_EthCtrlConfig_EthCtrlConfig0, ETH_MODE_ACTIVE);

    R_OSAL_ThreadCreate(&thread_config, (osal_thread_id_t)OSAL_THREAD_R_CAR_ETHER_MT, &thread);

    /* Get controller mode */
    Eth_GetControllerMode(EthConf_EthCtrlConfig_EthCtrlConfig0,
    &LenEthTestCtrlMode);

    if(ETH_MODE_ACTIVE != LenEthTestCtrlMode) {
        return R_NETIF_ERR_FAILED;
    }

#if (ETH_CTRL_ENABLE_MII == STD_ON)
    if(0 != R_NetIf_ClockPadSkewSetup()) {
        return R_NETIF_ERR_FAILED;
    }
#endif

    return R_NETIF_ERR_OK;
}

int R_NetIf_HwLink(void)
{
	uint16_t LusEthRegValue = ETH_ZERO;
	uint32_t GulDelayCounter = ETH_ZERO;
    /* Setting PHY to auto-negotiate 100Mbps*/
    /* Remove 1000Mbps speed */
    Eth_WriteMii(EthConf_EthCtrlConfig_EthCtrlConfig0, ETH_PHY_TRCV, ETH_PHY_TRCV_CTRL_REG, ETH_ZERO_16);
    /* Remove auto-negotiation advertisement for 1000Mbps full/half duplex  */
	Eth_WriteMii(EthConf_EthCtrlConfig_EthCtrlConfig0, ETH_PHY_TRCV, ETH_PHY_TRCV_1000T_CTRL_REG, ETH_ZERO_16);
    /* Force restart auto-negotiation */
    Eth_WriteMii(EthConf_EthCtrlConfig_EthCtrlConfig0, ETH_PHY_TRCV, ETH_PHY_TRCV_CTRL_REG, ETH_REG_VALUE);
    while (GulDelayCounter < ETH_TIMEOUT)
    {
        GulDelayCounter++;
		/* Check the Phy interface is Initialized */
        Eth_ReadMii(EthConf_EthCtrlConfig_EthCtrlConfig0, ETH_PHY_TRCV,  ETH_PHY_TRCV_STAT_REG, &LusEthRegValue);

        if (((LusEthRegValue & ETH_AUTO_NEGOTIATION_ACTIVE) == \
        ETH_AUTO_NEGOTIATION_ACTIVE) && ((LusEthRegValue&ETH_LINK_UP) == ETH_LINK_UP))
        {
            R_PRINT_Log("Link is up \r\n");
            return R_NETIF_ERR_OK;
        }
        R_OSAL_ThreadSleepForTimePeriod(10);
    }
	return R_NETIF_ERR_FAILED;
    TxConfirmed = 0;
}

int R_NetIf_HwStop(void)
{
	uint32_t GulDelayCounter = ETH_ZERO;
    Eth_SetControllerMode(EthConf_EthCtrlConfig_EthCtrlConfig0, ETH_MODE_DOWN);
    EthModeDownCnt = ETH_ZERO;
    GulDelayCounter = ETH_ZERO;
    while (GulDelayCounter < ETH_TIMEOUT)
    {
        GulDelayCounter++;
        if (EthModeDownCnt > ETH_ZERO)
        {
        /* Mode transition completed */
            return R_NETIF_ERR_OK;
        }
        R_OSAL_ThreadSleepForTimePeriod(1);
    }
    return R_NETIF_ERR_FAILED;
}

int R_NetIf_ProTxBuffer(uint32_t * const bufIdx, uint8_t ** const bufPtr, uint16_t * const dataLen)
{
    BufReq_ReturnType LenEthBufRet = BUFREQ_E_NOT_OK;
    LenEthBufRet = Eth_ProvideTxBuffer(EthConf_EthCtrlConfig_EthCtrlConfig0, (Eth_BufIdxType * const)bufIdx, bufPtr, dataLen);
    if (BUFREQ_OK == LenEthBufRet)
    {
        return R_NETIF_ERR_OK;
    }
    return R_NETIF_ERR_FAILED;

}

int R_NetIf_Transmit(uint32_t bufIdx, uint16_t type, uint32_t dataLen, uint8_t *tgMacAddr)
{
    Std_ReturnType LddEthReturnvalue = E_NOT_OK;
    LddEthReturnvalue = Eth_Transmit (EthConf_EthCtrlConfig_EthCtrlConfig0, bufIdx, type, ETH_TRUE, dataLen, tgMacAddr);
    if(E_OK == LddEthReturnvalue) {
        return R_NETIF_ERR_OK;
    }
    return R_NETIF_ERR_FAILED;
}

int R_NetIf_TxConfirmation(void)
{
    uint32_t GulDelayCounter = ETH_ZERO;
    while ((TxConfirmed==ETH_ZERO) && GulDelayCounter < ETH_TIMEOUT)
    {
        GulDelayCounter++;
        /* Polling for Tx confirmation */
        Eth_TxConfirmation(EthConf_EthCtrlConfig_EthCtrlConfig0);
    }
    return R_NETIF_ERR_OK;
}

void R_NetIf_RxIrq(void)
{
    /* UNUSED PARAMETERS */
	Eth_AvbCh00Isr(NULL, 0, (void*)0);
}

void R_NetIf_TxIrq(void)
{
	Eth_AvbCh18Isr(NULL, 0, (void*)0);
}

void R_NetIf_Get_MacAddr(uint8_t* addr)
{
    Eth_GetPhysAddr(EthConf_EthCtrlConfig_EthCtrlConfig0, addr);
}
