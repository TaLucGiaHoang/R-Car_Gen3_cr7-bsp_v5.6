/***********************************************************************************************************************
* IMR Driver sample program
* Copyright (c) [2018,2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/****************************************************************************
 * FILE          : p2c_1280_15_dl.c
 * DESCRIPTION   : This module is the sample program to use IMR-LX4 driver.
 * CREATED       : 2018.06.15
 * MODIFIED      : -
 * AUTHOR        : Renesas Electronics Corporation
 * TARGET DEVICE : R-Car Gen3e
 * HISTORY       :
 *                 2018.06.15
 *                 - Created new.
 *                 2020.12.11
 *                 - Changed included header file name.
 ****************************************************************************/

/*******************************************************************************
*   Title: Sample display list
*
*   This module contains the sample display list.
*/


/***************************************************************************
*   Section: Includes
*/
#include <stdint.h>
#include <stddef.h>
#include "project_dl_header.h"


/***************************************************************************
*   Section: Local Variables
*/

/***************************************************************************
*   Section: Global Functions
*/

/***************************************************************************
*   Function: get_display_list_addr
*
*   See imrlxsample.h
*/
const uint32_t * get_display_list_addr(uint32_t * const p_arraynum)
{
    if(p_arraynum != NULL)
    {
        *p_arraynum = sizeof(IMR_DL2) / sizeof(IMR_DL2[0]);
    }
    return IMR_DL2;
}

/* End of File */

