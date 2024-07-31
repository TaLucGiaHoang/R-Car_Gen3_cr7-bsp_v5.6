/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/
 #include "r_config_drw2d.h"
#include "r_drw2d_types.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_drw2d_ctx_dhd.h"

#include "img_format.h"
#include "demo.h"

typedef  struct {
    Img_ColourFormat_t    img;
    r_drw2d_PixelFormat_t drw2d;
} color_translate_t;

//static color_translate_t translation[6]={
static r_drw2d_PixelFormat_t translation[6]={
    [IMG_ALPHA8]  =R_DRW2D_PIXELFORMAT_ALPHA8,
    [IMG_RGB565]  =R_DRW2D_PIXELFORMAT_RGB565,
    [IMG_ARGB8888]=R_DRW2D_PIXELFORMAT_ARGB8888,
    [IMG_ARGB1555]=R_DRW2D_PIXELFORMAT_ARGB1555,
    [IMG_RGB888]  =R_DRW2D_PIXELFORMAT_ARGB8888,
};

static int trans_size=sizeof(translation)/sizeof(translation[0]);

r_drw2d_PixelFormat_t GetDrw2dFormat(Img_ColourFormat_t ColorFormat){
    ColorFormat &= 0xf;
    if(ColorFormat >= trans_size){
        return R_DRW2D_PIXELFORMAT_NONE;
    }
    return translation[ColorFormat];
}
