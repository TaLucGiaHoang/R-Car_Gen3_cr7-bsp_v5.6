/*******************************************************************************
 *
 * PURPOSE
 *   RIVP Reference Application Sub Window Manger Wrapper Public Defines
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *****************************************************************************/
/*
 * Copyright(C) 2017-2019 Renesas Electronics Corporation. All Rights Reserved.
 * RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
 * This program must be used solely for the purpose for which
 * it was furnished by Renesas Electronics Corporation.
 * No part of this program may be reproduced or disclosed to
 * others, in any form, without the prior written permission
 * of Renesas Electronics Corporation.
 *
 *****************************************************************************/
#ifndef RIVP_APP_DISP_H
#define RIVP_APP_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/*                    INCLUDE FILES                                          */
/*****************************************************************************/
#include "rivp_ldd_types.h"
#include "rivp_ldd_api.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/
/* output unit */
/*  H3  [ unit #1 : HDMI0, unit #2 : HDMI1, unit #3 : Analog RGB ] */
/*  M3  [ unit #1 : HDMI0, unit #2 : Analog RGB ] */
/*  E3  [ unit #0 : HDMI0, unit #1 : Analog RGB ] */
#define RIVP_APP_DISP_UNIT_0        (0u)        /*!< unit #1                  */
#define RIVP_APP_DISP_UNIT_1        (1u)        /*!< unit #1                  */
#define RIVP_APP_DISP_UNIT_2        (2u)        /*!< unit #2                  */
#define RIVP_APP_DISP_UNIT_3        (3u)        /*!< unit #3                  */

/* Upper and lower limit of unit */
#define RIVP_APP_DISP_UNIT_MIN    RIVP_APP_DISP_UNIT_0
#define RIVP_APP_DISP_UNIT_MAX    RIVP_APP_DISP_UNIT_3

#ifndef RIVP_APP_DISP_UNIT_SELECT
#define RIVP_APP_DISP_UNIT_SELECT RIVP_APP_DISP_UNIT_1
#endif

/******************************************************************************/
/*                    TYPE DEFINITION                                         */
/******************************************************************************/
typedef struct {
    RIVP_LDD_U32                 unit;                 /*!< output unit      */
    RIVP_LDD_U32                 width;                /*!< window width     */
    RIVP_LDD_U32                 height;               /*!< window height    */
    RIVP_LDD_U32                 pitch;                /*!< pitch            */
    RIVP_LDD_BOOL                support422Format;     /*!< support YCbCr422 */
} RIVP_APP_DISP_PARAM_T;

/******************************************************************************/
/*                    FUNCTIONS                                               */
/******************************************************************************/
extern RIVP_LDD_BOOL RIVP_AppDispInit(const RIVP_APP_DISP_PARAM_T *prm);
extern RIVP_LDD_BOOL RIVP_AppDispDeinit(void);
extern RIVP_LDD_BOOL RIVP_AppDispUpdate(RIVP_LDD_PICPARAM_T *pic);

#ifdef __cplusplus
}
#endif

#endif /* RIVP_APP_DISP_H */
