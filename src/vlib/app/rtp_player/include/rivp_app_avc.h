/**********************************************************************
 *
 * PURPOSE
 *   RIVP Reference Application AVC Decoder Public Defines
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 **********************************************************************/
/*
 * Copyright(C) 2015-2019 Renesas Electronics Corporation. All Rights Reserved.
 * RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
 * This program must be used solely for the purpose for which
 * it was furnished by Renesas Electronics Corporation.
 * No part of this program may be reproduced or disclosed to
 * others, in any form, without the prior written permission
 * of Renesas Electronics Corporation.
 *
 **********************************************************************/
#ifndef RIVP_APP_AVC_H
#define RIVP_APP_AVC_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/*                    INCLUDE FILES                                          */
/*****************************************************************************/
#include "rivp_ldd_api.h"

/******************************************************************************/
/*                    MACROS/DEFINES                                          */
/******************************************************************************/

/******************************************************************************/
/*                    TYPE DEFINITION                                         */
/******************************************************************************/

/******************************************************************************/
/*                    FUNCTIONS                                               */
/******************************************************************************/
extern RIVP_LDD_RESULT RIVP_AppLddAvcMain( void );

#ifdef __cplusplus
}
#endif

#endif /* RIVP_APP_AVC_H */
