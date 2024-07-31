/*************************************************************************************************************
* cio_ether_irq_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

/*******************************************************************************
   Section: Includes
 */

#include "rcar-xos/osal/r_osal.h"
#include "r_cio_bridge.h"
#include "r_cio_ether_main.h"
#include "r_cio_ether_irq.h"
#include "r_cio_ether_write.h"
#include "r_cio_ether_public.h"
#include "Eth_Irq.h"
#include "r_print_api.h"

#include "RCar_FuSa.h"
#include "Eth_Cfg.h"

#if (ETH_RUNTIME_ERROR_DETECT == STD_ON)
/* Included for the declaration of Det_ReportRuntimeError() */
#include "Det.h"
#endif
/* Included for declaration of the function Dem_ReportErrorStatus() */
#include "Dem.h"

/*******************************************************************************
   Section: Local Defines
 */

#define ETHER_RX_THREAD_ID   (0x8000U + 89U)
#define ETHER_RX_MQ_ID       (0x8000U + 66U)

#define ETHER_MQ_NUM_OF_MSG    1
#define ETHER_MQ_MSG_SIZE      sizeof(uint32_t)
#define TIMEOUT_MS 1000 /* 1000 millisecond */


/*******************************************************************************
   Section: Global Variables
 */
extern osal_device_handle_t Ether_device_handle;
extern osal_mq_handle_t Ether_mq_handle_rx;

/*******************************************************************************
   Section: Local Variables
 */
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
static int quit = 0;
st_osal_thread_config_t p_thread_control_ether_rx;
osal_thread_handle_t eth_rx_irq_thread_handle;
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
   Local Function: Ether IRQ thread
 */
#if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
static int64_t loc_Eth_RxIrqThread(void * Arg)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    uint32_t recvdata = 0;

    do {
        /* receive the message */
        osal_ret = R_OSAL_MqReceiveForTimePeriod(Ether_mq_handle_rx, (osal_milli_sec_t)TIMEOUT_MS, (void*)&recvdata, ETHER_MQ_MSG_SIZE);
        if ((OSAL_RETURN_OK != osal_ret) && (OSAL_RETURN_TIME != osal_ret))
        {
            R_PRINT_Log("[loc_Eth_RxIrqThread]: R_OSAL_MqReceiveForTimePeriod failed(%d)\r\n", osal_ret);
        }

        if (OSAL_RETURN_OK == osal_ret)
        {
            if (ETH_SCHANNEL16 < recvdata)
            {
                R_PRINT_Log("[loc_Eth_RxIrqThread]: Invalid interrupt channel(%d)\r\n", recvdata);
            }
            else
            {
                /* Interrupt handling */
                Eth_ProcessRxInterrupt(recvdata);
                /* IRQ Enable */
                osal_ret = R_OSAL_InterruptEnableIsr(Ether_device_handle, (uint64_t)recvdata);
                if (OSAL_RETURN_OK != osal_ret)
                {
                    R_PRINT_Log("[loc_Eth_RxIrqThread]: R_OSAL_InterruptEnableIsr failed(%d)\r\n", osal_ret);
                }
            }
        }
    } while (quit != 1);

    return 0;
}
#endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

/*******************************************************************************
   Function: R_CIO_ETHER_PRV_IrqInit
 */

int32_t R_CIO_ETHER_PRV_IrqInit(void)
{
    int32_t ret = -1;
    e_osal_return_t osal_ret = OSAL_RETURN_OK;

    #if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
    /* Create the thread of Ether Rx IRQ process */
    p_thread_control_ether_rx.func = (p_osal_thread_func_t)loc_Eth_RxIrqThread;
    p_thread_control_ether_rx.userarg = NULL;
    quit = 0;
    osal_ret = R_OSAL_ThreadCreate(&p_thread_control_ether_rx, ETHER_RX_THREAD_ID, &eth_rx_irq_thread_handle);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[R_CIO_ETHER_PRV_IrqInit]: ERROR!!! loc_Eth_RxIrqThread R_OSAL_ThreadCreate return value = %d\n", osal_ret);
    }
    #endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */

    if (OSAL_RETURN_OK == osal_ret)
    {
        #if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
        /* Set interrupt Eth_AvbCh00Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH00ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh00Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH00ISR_ID);
        /* Set interrupt Eth_AvbCh01Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH01ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh01Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH01ISR_ID);
        /* Set interrupt Eth_AvbCh02Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH02ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh02Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH02ISR_ID);
        /* Set interrupt Eth_AvbCh03Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH03ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh03Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH03ISR_ID);
        /* Set interrupt Eth_AvbCh04Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH04ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh04Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH04ISR_ID);
        /* Set interrupt Eth_AvbCh05Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH05ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh05Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH05ISR_ID);
        /* Set interrupt Eth_AvbCh06Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH06ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh06Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH06ISR_ID);
        /* Set interrupt Eth_AvbCh07Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH07ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh07Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH07ISR_ID);
        /* Set interrupt Eth_AvbCh08Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH08ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh08Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH08ISR_ID);
        /* Set interrupt Eth_AvbCh09Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH09ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh09Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH09ISR_ID);
        /* Set interrupt Eth_AvbCh10Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH10ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh10Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH10ISR_ID);
        /* Set interrupt Eth_AvbCh11Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH11ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh11Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH11ISR_ID);
        /* Set interrupt Eth_AvbCh12Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH12ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh12Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH12ISR_ID);
        /* Set interrupt Eth_AvbCh13Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH13ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh13Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH13ISR_ID);
        /* Set interrupt Eth_AvbCh14Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH14ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh14Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH14ISR_ID);
        /* Set interrupt Eth_AvbCh15Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH15ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh15Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH15ISR_ID);
        /* Set interrupt Eth_AvbCh16Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH16ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh16Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH16ISR_ID);
        /* Set interrupt Eth_AvbCh17Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH17ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh17Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH17ISR_ID);
        #endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */
        #if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)
        /* Set interrupt Eth_AvbCh18Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH18ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh18Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH18ISR_ID);
        /* Set interrupt Eth_AvbCh19Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH19ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh19Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH19ISR_ID);
        /* Set interrupt Eth_AvbCh20Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH20ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh20Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH20ISR_ID);
        /* Set interrupt Eth_AvbCh21Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH21ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh21Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH21ISR_ID);
        #endif /* (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON) */
        /* Set interrupt Eth_AvbCh22Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH22ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh22Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH22ISR_ID);
        /* Set interrupt Eth_AvbCh23Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH23ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh23Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH23ISR_ID);
        /* Set interrupt Eth_AvbCh24Isr*/
        osal_ret |= R_OSAL_InterruptRegisterIsr(Ether_device_handle, ETH_AVBCH24ISR_ID, 0, (p_osal_isr_func_t)Eth_AvbCh24Isr, NULL);
        osal_ret |= R_OSAL_InterruptEnableIsr(Ether_device_handle, ETH_AVBCH24ISR_ID);

        if(osal_ret == OSAL_RETURN_OK)
        {
            ret = 0;
        }
        else
        {
            R_PRINT_Log("[CioEtherIrq]: R_CIO_ETHER_PRV_IrqInit Interrupt setting failed(%d)\r\n", osal_ret);
        }
    }

    return ret;
}

/*******************************************************************************
   Function: R_CIO_ETHER_PRV_IrqDeInit
 */

int32_t R_CIO_ETHER_PRV_IrqDeInit(void)
{
    e_osal_return_t osal_ret = OSAL_RETURN_OK;
    int32_t ret = -1;

    #if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH00ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH00ISR_ID, (void *)Eth_AvbCh00Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH01ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH01ISR_ID, (void *)Eth_AvbCh01Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH02ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH02ISR_ID, (void *)Eth_AvbCh02Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH03ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH03ISR_ID, (void *)Eth_AvbCh03Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH04ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH04ISR_ID, (void *)Eth_AvbCh04Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH05ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH05ISR_ID, (void *)Eth_AvbCh05Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH06ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH06ISR_ID, (void *)Eth_AvbCh06Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH07ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH07ISR_ID, (void *)Eth_AvbCh07Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH08ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH08ISR_ID, (void *)Eth_AvbCh08Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH09ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH09ISR_ID, (void *)Eth_AvbCh09Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH10ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH10ISR_ID, (void *)Eth_AvbCh10Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH11ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH11ISR_ID, (void *)Eth_AvbCh11Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH12ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH12ISR_ID, (void *)Eth_AvbCh12Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH13ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH13ISR_ID, (void *)Eth_AvbCh13Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH14ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH14ISR_ID, (void *)Eth_AvbCh14Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH15ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH15ISR_ID, (void *)Eth_AvbCh15Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH16ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH16ISR_ID, (void *)Eth_AvbCh16Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH17ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH17ISR_ID, (void *)Eth_AvbCh17Isr);
    #endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */
    #if (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON)
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH18ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH18ISR_ID, (void *)Eth_AvbCh18Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH19ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH19ISR_ID, (void *)Eth_AvbCh19Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH20ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH20ISR_ID, (void *)Eth_AvbCh20Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH21ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH21ISR_ID, (void *)Eth_AvbCh21Isr);
    #endif /* (ETH_CTRL_ENABLE_TX_INTERRUPT == STD_ON) */
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH22ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH22ISR_ID, (void *)Eth_AvbCh22Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH23ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH23ISR_ID, (void *)Eth_AvbCh23Isr);
    osal_ret |= R_OSAL_InterruptDisableIsr(Ether_device_handle, ETH_AVBCH24ISR_ID);
    osal_ret |= R_OSAL_InterruptUnregisterIsr(Ether_device_handle, ETH_AVBCH24ISR_ID, (void *)Eth_AvbCh24Isr);

    if(osal_ret == OSAL_RETURN_OK)
    {
        #if (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON)
        /* Finish Ether RxIRQ thread loop */
        quit = 1;
        osal_ret = R_OSAL_ThreadJoin(eth_rx_irq_thread_handle, 0);
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("[CioEtherIrq]: R_OSAL_ThreadJoin error(%d)\r\n", osal_ret);
        }
        else
        #endif /* (ETH_CTRL_ENABLE_RX_INTERRUPT == STD_ON) */
        {
            /* success */
            ret = 0;
        }
    }
    else
    {
        R_PRINT_Log("[CioEtherIrq]: R_CIO_ETHER_PRV_IrqDeInit Interrupt setting failed(%d)\r\n", osal_ret);
    }

    return ret;
}

