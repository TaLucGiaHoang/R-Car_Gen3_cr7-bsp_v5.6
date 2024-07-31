/*************************************************************************************************************
* cio_dummy_main
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#include "rcar-xos/osal/r_osal.h"
#include "r_cio_bridge.h"
#include "r_print_api.h"



static int locInit(size_t Addr, int IrqNum)
{
    R_PRINT_Log("CIO Dummy locInit\r\n");
    return 0;
}

static int locDeInit(size_t Addr)
{
    R_PRINT_Log("CIO Dummy locDeInit\r\n");
    return 0;
}

static int locOpen(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    R_PRINT_Log("CIO Dummy locOpen\r\n");
    return 0;
}

static int locClose(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    R_PRINT_Log("CIO Dummy locClose\r\n");
    return 0;
}

static ssize_t locRead(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    ssize_t len;
    char        *buf;
    char ch;

    R_PRINT_Log("CIO Dummy locRead\r\n");
    buf = Par1;
    len = (ssize_t)Par2;
    ch = 0;
    while (len > 0) {
        *buf = ch;
        buf++;
        ch++;
        len--;
    }
    len = (ssize_t)Par2 - len;
    return len;
}

static ssize_t locWrite(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    size_t len;

    R_PRINT_Log("CIO Dummy locWrite\r\n");
    len = (size_t)Par2;
    return len;
}

static ssize_t locIoCtl(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    R_PRINT_Log("CIO Dummy locIoCtl\r\n");
    return 0;
}

static ssize_t locStatus(size_t Addr, void *Par1, void *Par2, void *Par3)
{
    R_PRINT_Log("CIO Dummy locStatus\r\n");
    return 0;
}


const r_cio_Driver_t R_CIO_DummyDriver = {
    "Dummy",
    locInit,
    locDeInit,
    locOpen,
    locClose,
    locRead,
    locWrite,
    locIoCtl,
    locStatus
};

