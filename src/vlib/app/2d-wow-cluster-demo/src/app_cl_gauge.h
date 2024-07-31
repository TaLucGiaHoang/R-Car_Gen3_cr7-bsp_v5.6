/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#ifndef _APP_CL_GAUGE_H_
#define _APP_CL_GAUGE_H_


#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: Gauge Screen Interface
*/

#undef _EXTERN_
#ifndef _APP_CL_GAUGE_GLOBALS_
#define _EXTERN_ extern 
#else
#define _EXTERN_
#endif


/***********************************************************
  Section: Global Functions
*/

/***********************************************************
  Function: RenderClGaugeScreen

  Gauge screen render function.

  Parameters:
  Dev - device pointer

  Returns:
  void
*/

void RenderClGaugeScreen(davehd_dev_t *Dev);

/***********************************************************
  Function: GaugeScreenInit

  Gauge screen init function.

  Parameters:
  device - device pointer

  Returns:
  void
*/

void GaugeScreenInit(davehd_dev_t *Dev);


/***********************************************************
  Function: GaugeScreenDeInit

  Gauge screen deinit function.

  Parameters:
  device - device pointer

  Returns:
  void
*/

void GaugeScreenDeInit(davehd_dev_t *Dev);


#ifdef __cplusplus
}
#endif

#endif /* _APP_CL_GAUGE_H_ */

