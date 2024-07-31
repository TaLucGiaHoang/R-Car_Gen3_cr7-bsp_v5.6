/***********************************************************************************************************************
* IMR Driver
* Copyright (c) [2020-2021] Renesas Electronics Corporation
* This software is released under the MIT License.
* http://opensource.org/licenses/mit-license.php
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : r_imrdrv_version.h
* Version      : 2.4.0
* Product Name : IMR Driver
* Device(s)    : R-Car
* Description  : IMR Driver public header of version
***********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 14.06.2021 0.01     New create
 *         : 29.10.2021 0.02     Changed Version to 2.4.0
 *********************************************************************************************************************/
#ifndef R_IMRDRV_VERSION_H_
#define R_IMRDRV_VERSION_H_

#ifdef __cplusplus
extern "C"
{
#endif


/***************************************************************************
*   Section: Includes
*/
#include <stdint.h>

/***************************************************************************
*   Section: Defines
*/

#define IMRDRV_VERSION_MAJOR (2U)
#define IMRDRV_VERSION_MINOR (4U)
#define IMRDRV_VERSION_PATCH (0U)


/***************************************************************************
*   Section: Types
*/

typedef struct st_imrdrv_version {
   uint32_t major;
   uint32_t minor;
   uint32_t patch;
} st_imrdrv_version_t;


/***************************************************************************
*   Section: Global Variables
*/

const st_imrdrv_version_t* R_IMRDRV_GetVersion(void);

#ifdef __cplusplus
}
#endif
#endif /* R_IMRDRV_VERSION_H_ */

