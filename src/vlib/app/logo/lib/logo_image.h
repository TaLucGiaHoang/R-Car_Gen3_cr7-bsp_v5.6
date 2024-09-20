/*************************************************************************************************************
* logo_image
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
/*******************************************************************************
  Title: Logo image header

  This module contains the logo image RGB data.
*******************************************************************************/
#ifndef LOGO_IMAGE_H_
#define LOGO_IMAGE_H_

typedef struct {
    uint16_t width;             /* Width with pixel */
    uint16_t height;            /* Height with line */
    uint32_t buff_size;         /* Buffer sise with byte */
    const uint8_t *pbuffer;     /* Pointer to RGB buffer */
} RGB_IMAGE;

#ifdef R_TARGET_BOARD_EBISU

#ifdef USE_DPAD
extern RGB_IMAGE rgb_image_dpad;
#else  /* USE_LVDS */
extern RGB_IMAGE rgb_image_lvds;
#endif /* USE_DPAD */

#else /* R_TARGET_BOARD_SALVATORXS */

#ifdef USE_DPAD
extern RGB_IMAGE rgb_image_dpad;
#elif USE_LVDS
extern RGB_IMAGE rgb_image_lvds;
#else  /* USE_HDMI0 / USE_HDMI1 */
extern RGB_IMAGE rgb_image_hdmi;
#endif /* USE_DPAD */

#endif  /* R_TARGET_BOARD_EBISU */

#endif  /* LOGO_IMAGE_H_ */
