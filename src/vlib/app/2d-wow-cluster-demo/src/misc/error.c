/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/


/***********************************************************
  Title: Error Handler Module
  
  This is the application's error handler.
*/


/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"     /* Renesas basic types, e.g. uint32_t */
#include "error.h"
#ifdef WIN32
#include <windows.h>
#endif

/***********************************************************
  Section: Global Functions

  Description see: <error.h>
*/

/***********************************************************
  Function: ClusterError
*/

void ClusterError(int Err) {
    while (Err) {
        /* jtodoDHD Sleep(100);*/
    }
}
