/*
****************************************************************************
PROJECT : Drw2D driver
FILE    : $Id: r_sys_drw2d_cpu_renderscanlines.h 11006 2016-11-07 16:32:44Z michael.golczewski $
============================================================================
DESCRIPTION
Driver for DRW2D
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2013 - 2014
                                  by
                       Renesas Electronics (Europe) GmbH.
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================

DISCLAIMER

LICENSEE has read, understood and accepted the terms and conditions defined in
the license agreement, especially the usage rights. In any case, it is
LICENSEE's responsibility to make sure that any user of the software complies
with the terms and conditions of the signed license agreement.

****************************************************************************
*/

#ifndef R_DRW2D_CPU_RENDERSCANLINES_H_
#define R_DRW2D_CPU_RENDERSCANLINES_H_


/***************************************************************************
  Section: Types and enumerations

*/


/*******************************************************************************
  Enum: r_drw2d_sys_cpu_Renderer_t

  Specifier for the renderer implementations.

  Values:
    R_DRW2D_SYS_CPU_RENDERER_SCANLINE   - Scan line based render approach.
    R_DRW2D_SYS_CPU_RENDERER_BLITTER    - Simple blitting render approach.
*/
typedef enum {
    R_DRW2D_SYS_CPU_RENDERER_SCANLINE     =  0,
    R_DRW2D_SYS_CPU_RENDERER_BLITTER      =  1,

    R_DRW2D_SYS_CPU_NUM_RENDERERS,
    R_DRW2D_SYS_CPU_RENDERER_NOT_SUPPORTED,
} r_drw2d_sys_cpu_Renderer_t;


/*******************************************************************************
  Enum: r_drw2d_sys_cpu_InputEncoding_t

  Specifier for the input encodings (e.g. RLE, Swizzled, ...).

  Values:
    R_DRW2D_SYS_CPU_RENDERER_NONE       - No filtering or input encoding.
    R_DRW2D_SYS_CPU_RENDERER_RLE        - RLE encoded input.
    R_DRW2D_SYS_CPU_RENDERER_BILINEAR   - Bilinear filtering.
*/
typedef enum {
    R_DRW2D_SYS_CPU_INPUTENCODING_NONE         =  0,
    R_DRW2D_SYS_CPU_INPUTENCODING_RLE          =  1,
    R_DRW2D_SYS_CPU_INPUTENCODING_BILINEAR     =  2,

    R_DRW2D_SYS_CPU_NUM_INPUTENCODINGS,
    R_DRW2D_SYS_CPU_INPUTENCODING_NOT_SUPPORTED,
} r_drw2d_sys_cpu_InputEncoding_t;


/*******************************************************************************
  Enum: r_drw2d_sys_cpu_InputFormat_t

  Specifier for input formats.

  Values:
    R_DRW2D_SYS_CPU_INPUTFORMAT_NONE        - No format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_ALPHA8      - 8 bit alpha format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_LUM8        - 8 bit luminance format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_AL17        - 1 alpha, 7 luminance bits format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_AL44        - 4 alpha, 4 luminance bits format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_AL88        - 8 alpha, 8 luminance bits format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_AL1         - 1 alpha or luminance bit format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_AL2         - 2 alpha or luminance bits format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_AL4         - 4 alpha or luminance bits format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_AL8         - 8 alpha or luminance bits format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_RGB565      - 5 red, 6 green and 5 blue bits format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_ARGB1555    - 1 alpha, 5 red, 5 green and 5 blue bits format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_RGBA5551    - 5 red, 5 green, 5 blue and 1 alpha bits format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_ARGB4444    - 4 alpha, 4 red, 4 green and 4 blue bits format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_RGBA4444    - 4 red, 4 green, 4 blue and 4 alpha bits format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_ARGB8888    - 8 alpha, 8 red, 8 green and 8 blue bits format.
    R_DRW2D_SYS_CPU_INPUTFORMAT_RGBA8888    - 8 red, 8 green, 8 blue and 8 alpha bits format.
*/
typedef enum {
    R_DRW2D_SYS_CPU_INPUTFORMAT_NONE     =  0,
    R_DRW2D_SYS_CPU_INPUTFORMAT_ALPHA8   =  1,
    R_DRW2D_SYS_CPU_INPUTFORMAT_LUM8     =  2,
    R_DRW2D_SYS_CPU_INPUTFORMAT_AL17     =  3,
    R_DRW2D_SYS_CPU_INPUTFORMAT_AL44     =  4,
    R_DRW2D_SYS_CPU_INPUTFORMAT_AL88     =  5,
    R_DRW2D_SYS_CPU_INPUTFORMAT_AL1      =  6,
    R_DRW2D_SYS_CPU_INPUTFORMAT_AL2      =  7,
    R_DRW2D_SYS_CPU_INPUTFORMAT_AL4      =  8,
    R_DRW2D_SYS_CPU_INPUTFORMAT_AL8      =  9,
    R_DRW2D_SYS_CPU_INPUTFORMAT_RGB565   = 10,
    R_DRW2D_SYS_CPU_INPUTFORMAT_ARGB1555 = 11,
    R_DRW2D_SYS_CPU_INPUTFORMAT_RGBA5551 = 12,
    R_DRW2D_SYS_CPU_INPUTFORMAT_ARGB4444 = 13,
    R_DRW2D_SYS_CPU_INPUTFORMAT_RGBA4444 = 14,
    R_DRW2D_SYS_CPU_INPUTFORMAT_ARGB8888 = 15,
    R_DRW2D_SYS_CPU_INPUTFORMAT_RGBA8888 = 16,

    R_DRW2D_SYS_CPU_NUM_INPUTFORMATS,
    R_DRW2D_SYS_CPU_INPUTFORMAT_NOT_SUPPORTED,
} r_drw2d_sys_cpu_InputFormat_t;


/*******************************************************************************
  Enum: r_drw2d_sys_cpu_InputFormat_t

  Specifier for output formats.

  Values:
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_LUM8        - 8 bit luminance format.
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_RGB565      - 5 red, 6 green and 5 blue bits format.
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_ARGB1555    - 1 alpha, 5 red, 5 green and 5 blue bits format.
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_RGBA5551    - 5 red, 5 green, 5 blue and 1 alpha bits format.
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_ARGB4444    - 4 alpha, 4 red, 4 green and 4 blue bits format.
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_RGBA4444    - 4 red, 4 green, 4 blue and 4 alpha bits format.
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_ARGB8888    - 8 alpha, 8 red, 8 green and 8 blue bits format.
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_RGBA8888    - 8 red, 8 green, 8 blue and 8 alpha bits format.
*/
typedef enum {
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_RGB565   = 0,
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_ARGB1555 = 1,
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_RGBA5551 = 2,
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_ARGB4444 = 3,
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_RGBA4444 = 4,
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_ARGB8888 = 5,
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_RGBA8888 = 6,
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_LUM8 = 7,

    R_DRW2D_SYS_CPU_NUM_OUTPUTFORMATS,
    R_DRW2D_SYS_CPU_OUTPUTFORMAT_NOT_SUPPORTED,
} r_drw2d_sys_cpu_OutputFormat_t;


/*******************************************************************************
  Enum: r_drw2d_sys_cpu_BlendMode_t

  Specifier for blend modes.

  Values:
    R_DRW2D_SYS_CPU_BLENDMODE_SRC       -  Porter-Duff Src mode.
    R_DRW2D_SYS_CPU_BLENDMODE_SRC_OVER  -  Porter-Duff Src-Over mode.
*/
typedef enum
{
    R_DRW2D_SYS_CPU_BLENDMODE_SRC       =  0,
    R_DRW2D_SYS_CPU_BLENDMODE_SRC_OVER  =  1,

    R_DRW2D_SYS_CPU_NUM_BLENDMODES,
    R_DRW2D_SYS_CPU_BLENDMODE_NOT_SUPPORTED,
} r_drw2d_sys_cpu_BlendMode_t;

/*******************************************************************************
  Enum: r_drw2d_sys_cpu_BlendMode_t

  Specifier for implemented effects.

  Values:
    R_DRW2D_SYS_CPU_EFFECT_NONE                     - No effect.
    R_DRW2D_SYS_CPU_EFFECT_MODULATE                 - Modulate effect
                                                      (multiply).
    R_DRW2D_SYS_CPU_EFFECT_CONSTANTALPHA            - Constant alpha
                                                      multiplication.
    R_DRW2D_SYS_CPU_EFFECT_MODULATE_CONSTANTALPHA   - Modulate effect combined
                                                      with constant alpha
*/
typedef enum
{
    R_DRW2D_SYS_CPU_EFFECT_NONE                     = 0,
    R_DRW2D_SYS_CPU_EFFECT_MODULATE                 = 1,
    R_DRW2D_SYS_CPU_EFFECT_CONSTANTALPHA            = 2,
    R_DRW2D_SYS_CPU_EFFECT_MODULATE_CONSTANTALPHA   = 3,

    R_DRW2D_SYS_CPU_NUM_EFFECTS,
    R_DRW2D_SYS_CPU_EFFECT_NOT_SUPPORTED,
} r_drw2d_sys_cpu_Effect_t;


/***************************************************************************
  Section: Function declarations

*/

r_drw2d_sys_cpu_PixelProcessing_t R_DRW2D_Platform_CPU_GetPixelProcessing(r_drw2d_DeviceBase_t *DeviceBase, r_drw2d_sys_cpu_BlendMode_t BlendMode, r_drw2d_sys_cpu_Effect_t Effect);
RenderScanLines R_DRW2D_Platform_CPU_GetRenderFunction(r_drw2d_DeviceBase_t *DeviceBase, r_drw2d_sys_cpu_Renderer_t Renderer);
void R_DRW2D_Platform_SetupRasterization(r_drw2d_DeviceBase_t *DeviceBase);

r_drw2d_sys_cpu_Effect_t R_DRW2D_Platform_CPU_GetEffect(r_drw2d_EffectStage_t *EffectStage, uint32_t NumberOfStages);
r_drw2d_sys_cpu_BlendMode_t R_DRW2D_Platform_CPU_GetBlendMode(r_drw2d_BlendMode_t BlendMode);

#endif /* R_DRW2D_CPU_RENDERSCANLINES_H_ */


