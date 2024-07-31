/*
****************************************************************************
PROJECT : Drw2D driver
FILE    : $Id: r_drw2d_sys_dhd.h 510 2019-09-30 02:32:37Z a5110279 $
============================================================================
DESCRIPTION
Driver for DRW2D
============================================================================
                            C O P Y R I G H T                            
============================================================================
                           Copyright (c) 2022
                                  by 
                    Renesas Electronics (Europe) GmbH. 
                           Arcadiastrasse 10
                          D-40472 Duesseldorf
                               Germany
                          All rights reserved.
============================================================================
DISCLAIMER                                                                   
This software is supplied by Renesas Electronics Corporation and is only     
intended for use with Renesas products. No other uses are authorized. This   
software is owned by Renesas Electronics Corporation and is protected under  
all applicable laws, including copyright laws.                               
THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING  
THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT      
LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE   
AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.          
TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS       
ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE  
FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR   
ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE  
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.                             
Renesas reserves the right, without notice, to make changes to this software 
and to discontinue the availability of this software. By using this software,
you agree to the additional terms and conditions found by accessing the      
following link:                                                              
http://www.renesas.com/disclaimer *                                          
Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.     

****************************************************************************
*/

#ifndef R_DRW2D_SYS_DHD_H_
#define R_DRW2D_SYS_DHD_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: DRW2D Support Functions for DaveHD (internal)

  DRW2D driver support functions

  The generic DRW2D driver uses these functions. They have to be implemented 
  within the driver library for a concrete device. (e.g. D1L, D1M)

  Support functions are functions that are not part of the driver itself 
  but they must be provided to integrate the driver on a particular board.

  It is not recommended to use this API directly.
  Use R_DRW2D_API_* instead.
*/


/***************************************************************************
  Section: Global Functions
*/

/***********************************************************
  Function: R_DRW2D_Platform_Init_DHD

  Global initialization of DRW2D DaveHD API.

  Parameters:
  None

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_Init_DHD (void);



/*******************************************************************************
  Function: R_DRW2D_Platform_Open_DHD
  
  Initialize DRW2D unit and driver-dependent graphics engine and return device
  handle.

  This function initializes the driver-dependent graphics engine to its default
  configuration.
  It initializes the internal device structure and returns an opaque handle to
  that structure.

  A default render context is created implicitly.

  <R_DRW2D_Open> must be called before any drawing function can take place.

  The application must ensure that the RetDevice is not used in more than one
  thread at a time.
  Ensure that you provide a real structure pointer as DeviceInternal, not a
  void pointer!
  Also ensure that the DeviceInternal structure members are initialised with 0.


  Parameters:
            Unit - Unit number (see <r_drw2d_Unit_t>)
                   This parameter can only be set to 0.
      DriverUnit - Driver unit number
                   This parameter can only be set to 0.
  DeviceInternal - Pointer to the gfx driver handle,
                   Set <r_drw2d_DeviceDHD_t> defined in "r_drw2d_ctx_dhd.h" 
                   in case of Drw2D with D/AVE HD.
                   Set <r_drw2d_DeviceCPU_t> defined in "r_drw2d_ctx_cpu.h" 
                   in case of Drw2D with CPU drawing.
                   Normally, this variable is never used by user, 
                   but it must be kept until <R_DRW2D_Close> is called.
       RetDevice - Returns the (opaque) Drw2d device handle

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_Open_DHD (r_drw2d_Unit_t    Unit,
                                           int32_t           DriverUnit,
                                           void             *DeviceInternal,
                                           r_drw2d_Device_t *RetDevice
                                           );



/***********************************************************
  Function: R_DRW2D_Platform_RenderContext_Init_DHD

  Initialize a render context

  Parameters:
  RenderContext - Reference to uninitialized render context structure
     DeviceBase - Address of device context base class

  Returns:
  None

*/
void R_DRW2D_Platform_RenderContext_Init_DHD (r_drw2d_RenderContext_t *RenderContext,
                                              r_drw2d_DeviceBase_t    *DeviceBase
                                              );



/***********************************************************
  Function: R_DRW2D_Platform_Exit_DHD

  Global shutdown of DRW2D DaveHD API.

  Parameters:
  None

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_Exit_DHD (void);



/*******************************************************************************
  Function: R_DRW2D_Platform_Close_DHD

  Shutdown DRW2D unit, de-initialize driver-dependent graphics engine and
  cleans up its internally used resources.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_Close_DHD (r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_Platform_NativeDriverHandleGet_DHD

  Returns a handle to the low level driver instance.

  DRW2D must have been initialized using <R_DRW2D_Init> prior to calling this
  function.

  Parameters:
              Device - Device handle (see <r_drw2d_Device_t>)
  RetNativeDrvHandle - Returns Native driver handle

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_NativeDriverHandleGet_DHD (r_drw2d_Device_t   Device,
                                                            void             **RetNativeDriverHandle
                                                            );



/*******************************************************************************
  Function: R_DRW2D_Platform_NativeDriverBegin_DHD

  Notify DRW2D that the application wants to access the low level,
  hardware-specific driver directly.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  Flags  - One of <R_DRW2D_NATIVEDRV_SAVESTATE>,
           <R_DRW2D_NATIVEDRV_KEEPSTATE>, <R_DRW2D_NATIVEDRV_APPCONTEXT>. 
           The upper 16bits of this parameter are reserved for
           driver-specific extensions.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_NativeDriverBegin_DHD (r_drw2d_Device_t Device,
                                                        uint32_t         Flags
                                                        );



/*******************************************************************************
  Function: R_DRW2D_Platform_NativeDriverEnd_DHD

  Notify DRW2D that the application has finished accessing the low level,
  hardware-specific driver directly.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
   Flags - Reserved for future extensions. Pass 0 for now.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_NativeDriverEnd_DHD (r_drw2d_Device_t Device,
                                                      uint32_t         Flags
                                                      );



/*******************************************************************************
  Function: R_DRW2D_Platform_ContextInit_DHD

  Initialize a render context with default settings.

  Parameters:
         Device - Device handle (see <r_drw2d_Device_t>)
  RenderContext - Reference to uninitialized render context structure. 
                  (see <r_drw2d_RenderContext_s>).
     RetContext - The context pointer that is initialized

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_ContextInit_DHD (r_drw2d_Device_t                Device,
                                                  struct r_drw2d_RenderContext_s *RenderContext,
                                                  r_drw2d_Context_t              *RetContext
                                                  );



/*******************************************************************************
  Function: R_DRW2D_Platform_ContextSelect_DHD

  Sets the given context as the current one. 
  Passing Context handle NULL will select the default context.

  Parameters:
   Device - Device handle (see <r_drw2d_Device_t>)
  Context - Render context handle (see <r_drw2d_Context_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_ContextSelect_DHD (r_drw2d_Device_t  Device,
                                                    r_drw2d_Context_t Context
                                                    );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxFgColor_DHD

  Set the foreground color to be used for drawing primitives with solid mode.

  Between calls to <R_DRW2D_CtxEffectsSet> and <R_DRW2D_CtxEffectsDelete>, 
  this function is invalid and an error will be returned.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
   Color - The color value (32bit packed ARGB)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxFgColor_DHD (r_drw2d_Device_t Device,
                                                 r_drw2d_Color_t  Color
                                                 );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxBgColor_DHD

  Set the background color to be used for drawing primitives.
  
  The <R_DRW2D_FramebufferClear> function always uses the current background
  color.

  Between calls to <R_DRW2D_CtxEffectsSet> and <R_DRW2D_CtxEffectsDelete>, 
  this function is invalid and an error will be returned.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
   Color - The color value (32bit packed ARGB)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxBgColor_DHD (r_drw2d_Device_t Device,
                                                 r_drw2d_Color_t  Color
                                                 );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxClipRect_DHD

  Sets a global clipping rectangle for subsequent drawing operations.
  A clip-rect can be set by calling this function before each R_DRW2D_Draw* API 
  and R_DRW2D_FramebufferClear.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
    Rect - Rectangular clipping area (window coordinates)
           (see <r_drw2d_IntRect_t>).
           All elements must be positive numbers (or 0).

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxClipRect_DHD (r_drw2d_Device_t         Device,
                                                  const r_drw2d_IntRect_t *Rect
                                                  );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxFillMode_DHD

  Set the filling mode for drawing with primitives.

  This is used for placing e.g. a texture on top of any primitive.

  Between calls to <R_DRW2D_CtxEffectsSet> and <R_DRW2D_CtxEffectsDelete>, 
  this function is invalid and an error will be returned.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
    Mode - The fill mode to be used. See <r_drw2d_FillMode_t>.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxFillMode_DHD (r_drw2d_Device_t   Device,
                                                  r_drw2d_FillMode_t Mode
                                                  );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxCullMode_DHD

  Set the culling mode for drawing with primitives.

  This is used to discard triangles or rectangles depending on their winding
  order.

  The default cull mode is <R_DRW2D_CULLMODE_NONE>.

  Parameters:
    Device - Device handle (see <r_drw2d_Device_t>)
  CullMode - The cull mode to be used. See <r_drw2d_CullMode_t>.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxCullMode_DHD (r_drw2d_Device_t   Device,
                                                  r_drw2d_CullMode_t CullMode
                                                  );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxLineStyle_DHD

  Set the line drawing style (e.g. line caps, line width, ..).

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
   Style - Pointer to line style struct (see <r_drw2d_LineStyle_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxLineStyle_DHD (r_drw2d_Device_t           Device,
                                                   const r_drw2d_LineStyle_t *Style
                                                   );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxBlendMode_DHD

  Set preset color/alpha source/destination blending equations.

  When <R_DRW2D_BLENDMODE_CUSTOM> is selected, the color/alpha blending
  equations are determined by the blend factors set by
  <R_DRW2D_CtxBlendFactors>.

  Parameters:
     Device - Device handle (see <r_drw2d_Device_t>)
  BlendMode - The blendmode mode to be used. See <r_drw2d_BlendMode_t>.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxBlendMode_DHD (r_drw2d_Device_t    Device,
                                                   r_drw2d_BlendMode_t BlendMode
                                                   );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxBlendFactors_DHD

  Set color/alpha source/destination blending factors.

  In order for these to have an effect, the <R_DRW2D_BLENDMODE_CUSTOM>
  blendmode must be selected (see <R_DRW2D_CtxBlendMode>).

  The effective color/alpha blend equation is determined by
    > dst_color = src_color*src_factor_color + dst_color*dst_factor_color
    > dst_alpha = src_alpha*src_factor_alpha + dst_alpha*dst_factor_alpha

  Parameters:
    Device - Device handle (see <r_drw2d_Device_t>)
    SrcRGB - The blendfactor to be used for source RGB values
             (see <r_drw2d_BlendFactor_t>)
    DstRGB - The blendfactor to be used for destination RGB values
             (see <r_drw2d_BlendFactor_t>)
  SrcAlpha - The blendfactor to be used for source alpha values
             (see <r_drw2d_BlendFactor_t>)
  DstAlpha - The blendfactor to be used for destination alpha values
             (see <r_drw2d_BlendFactor_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxBlendFactors_DHD (r_drw2d_Device_t      Device,
                                                      r_drw2d_BlendFactor_t SrcRGB,
                                                      r_drw2d_BlendFactor_t DstRGB,
                                                      r_drw2d_BlendFactor_t SrcAlpha,
                                                      r_drw2d_BlendFactor_t DstAlpha
                                                      );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxImgQuality_DHD

  Sets for the current context a global quality value used for graphics
  primitives.
  
  If Quality is R_DRW2D_IMGQUALITY_LOW, antialiasing and some effect features
  are not enabled. Refer to <R_DRW2D_CtxEffectsSet> for details of effect
  features. 
  This mode can be used as a performance optimization.

  Note that the DHD platform is limited to an edge width/height of max. 2048
  when using non antialiased (R_DRW2D_IMGQUALITY_LOW) edges.
  
  No restrictions on R_DRW2D_IMGQUALITY_MIDDLE and R_DRW2D_IMGQUALITY_HIGH.

  Parameters:
   Device - Device handle (see <r_drw2d_Device_t>)
  Quality - Image quality/antialiasing mode. See <r_drw2d_ImgQuality_t>.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxImgQuality_DHD (r_drw2d_Device_t     Device,
                                                    r_drw2d_ImgQuality_t Quality
                                                    );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxTransformMode_DHD

  Set the vertex transform/projection mode.

  The default is <R_DRW2D_TRANSFORM_2D> (2D vertex matrix transformation).

  When set to <R_DRW2D_TRANSFORM_NONE>, vertices will be used as-is, i.e. they
  will not be transformed by the vertex matrix. This mode can be used as a
  performance optimization.
  When no explicit UV coordinates are given, the texture is mapped to the
  framebuffer 1:1.

  When set to <R_DRW2D_TRANSFORM_3D>, vertices will be transformed by the
  4x4 vertex matrix and projected onto the current viewport.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
    Mode - The transform mode to be used. See <r_drw2d_TransformMode_t>.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTransformMode_DHD (r_drw2d_Device_t        Device,
                                                       r_drw2d_TransformMode_t Mode
                                                       );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxTextureTransformMode_DHD

  Set the texture coordinate transformation mode.

  The default is <R_DRW2D_TEX_TRANSFORM_2D> (2D texture matrix transformation).

  When set to <R_DRW2D_TEX_TRANSFORM_NONE>, texture coordinates will be used
  as-is, i.e. they will not be transformed by the texture matrix.
  This mode can be used as a performance optimization. When no explicit UV
  coordinates are given, the texture is mapped to the screen 1:1.

  When set to <R_DRW2D_TEX_TRANSFORM_2D>, texture coordinates will be
  transformed by the 3x2 texture matrix.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
    Mode - The transform mode to be used.
           See <r_drw2d_TextureTransformMode_t>.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTextureTransformMode_DHD (r_drw2d_Device_t               Device,
                                                              r_drw2d_TextureTransformMode_t Mode
                                                              );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxViewport_DHD

  Set viewport for 3D vertex transformation/projection.

  The default is to use the current framebuffer width/height, offset by (0; 0).

  After setting a custom viewport, set the Rect.Size to (0; 0) to revert to the
  default behaviour.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
    Rect - The viewport rectangle. See <r_drw2d_IntRect_t>.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxViewport_DHD (r_drw2d_Device_t         Device,
                                                  const r_drw2d_IntRect_t *Rect
                                                  );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxStripingEnable_DHD

  Enable striped pixel enumeration (for performance reasons).

  This should only be enabled if texture mapping is used and larger
  triangles/quads are being rendered.

  The actual stripe settings are determined per-primitive.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxStripingEnable_DHD (r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxStripingDisable_DHD

  Disable striped pixel enumeration.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxStripingDisable_DHD (r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxEffectsSet_DHD

  Sets an array of effects to be used for colorization and blending.
  The data in the array must be readable/writeable until a call to
  <R_DRW2D_CtxEffectsDelete>.

  The content of the array will be modified by <R_DRW2D_CtxEffectsUpdate>.

  Between calls to <R_DRW2D_CtxEffectsSet> and <R_DRW2D_CtxEffectsDelete>
  any calls to functions
  <R_DRW2D_CtxFgColor>, <R_DRW2D_CtxBgColor>, and <R_DRW2D_CtxFillMode> are
  invalid and an error will be returned.

  The effects <R_DRW2D_EFFECT_REPLACE>, <R_DRW2D_EFFECT_MODULATE>,
  <R_DRW2D_EFFECT_ADD>, <R_DRW2D_EFFECT_SUBTRACT>, <R_DRW2D_EFFECT_ADD_SIGNED>,
  <R_DRW2D_EFFECT_INTERPOLATE>, and <R_DRW2D_EFFECT_DOT3> can be combined to
  calculate a final color.  
  If the input source is specified as <R_DRW2D_EFFECT_SOURCE_PREV_STAGE>,
  the result of the previous stage will be used.

  If the combination of effects can not be realized, an error will be returned
  by the drawing call.

  The effects <R_DRW2D_EFFECT_CONSTANT_ALPHA> and <R_DRW2D_EFFECT_GRADIENT>
  provide a final alpha blending.
  
  It is necessary to set the image quality to <R_DRW2D_IMGQUALITY_MEDIUM> or 
  <R_DRW2D_IMGQUALITY_HIGH> (see <R_DRW2D_CtxImgQuality>) for these effects
  to work:
  
  In the case of <R_DRW2D_EFFECT_CONSTANT_ALPHA> the result will be blended
  with a constant alpha.
  The <R_DRW2D_EFFECT_GRADIENT> can be used to specify two points and two
  alpha values to calculate a linear gradient, which will be used to blend the
  final color. 
  
  The effects <R_DRW2D_EFFECT_GRADIENT> and <R_DRW2D_EFFECT_CONSTANT_ALPHA>
  can be combined.

  See User's manual for more information on how to use effects.

  Parameters:
   Device - Device handle (see <r_drw2d_Device_t>)
  Effects - Array with effects (see <r_drw2d_EffectStage_t>)
    Count - Number of effects in the array

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxEffectsSet_DHD (r_drw2d_Device_t       Device, 
                                                    r_drw2d_EffectStage_t* Effects, 
                                                    uint32_t               Count);



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxEffectsUpdate_DHD

  updates effect at stage Stage.

  Parameters:
     Device - Device handle (see <r_drw2d_Device_t>)
       Name - Effect (see <r_drw2d_EffectName_t>)
      Stage - Stage of effect to be updated
  NumParams - Number of parameters to be passed
     Params - Array of parameters (see <r_drw2d_EffectParam_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxEffectsUpdate_DHD (r_drw2d_Device_t Device,
                                                       r_drw2d_EffectName_t Name,
                                                       uint32_t Stage,
                                                       uint32_t NumParams,
                                                       const r_drw2d_EffectParam_t *Params);



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxEffectsDelete_DHD

  Deletes all effects.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxEffectsDelete_DHD (r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxTextureSet_DHD

  Set source texture.

  The Drw2D with D/AVE HD supports the direct / zero-copy use of an
  application provided texture address.
  
  The texture is used when the fill mode is set to <R_DRW2D_FILLMODE_TEXTURE>.

  Parameters:
       Device - Device handle (see <r_drw2d_Device_t>)
  TextureUnit - Texture Unit Number.
      Texture - Reference to texture structure (see <r_drw2d_Texture_t>). 
                NULL to deselect current texture.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTextureSet_DHD (r_drw2d_Device_t         Device,
                                                    uint32_t                 TextureUnit,
                                                    const r_drw2d_Texture_t *Texture
                                                    );



/*******************************************************************************
  Function: R_DRW2D_Platform_TextureBlit_DHD

  Blit texture from Src to Dest.

  Src can be NULL, in that case the blit origin is 0,0 inside of the texture
  and dimensions are taken from the DstRect.

  Parameters:
   Device - Device handle (see <r_drw2d_Device_t>)
  SrcRect - Source rectangle.
  DstRect - Destination rectangle.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_TextureBlit_DHD (r_drw2d_Device_t      Device,
                                                  const r_drw2d_Rect_t *SrcRect,
                                                  const r_drw2d_Rect_t *DstRect
                                                  );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxTextureColorKeyEnable_DHD

  Enables Color Keying for the provided RGB color.
  The color after replacement will be fully transparent.

  Parameters:
    Device - Device handle (see <r_drw2d_Device_t>)
  ColorKey - The color before replacement in RGB
             (alpha component is ignored)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTextureColorKeyEnable_DHD (r_drw2d_Device_t Device,
                                                               r_drw2d_Color_t ColorKey
                                                               );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxTextureColorKeyDisable_DHD

  Disabled a previously set color keying.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTextureColorKeyDisable_DHD (r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxIdentity_DHD

  Reset vertex transformation matrix.

  The identity matrix maps the vertex at (0; 0) to the top/left framebuffer
  position.

  The vertex at (framebuffer_width-1, framebuffer_height-1) is mapped to the
  bottom/right framebuffer position.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxIdentity_DHD (r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxTextureIdentity_DHD

  Reset texture matrix.

  If both texture and vertex matrices are set to identity, vertices and
  texture coordinates will use the same coordinate system.

  I.e. drawing a textured rectangle at (10;10) with size (40;30) will result
  in a 1:1 mapping of the respective texture area.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTextureIdentity_DHD (r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxTransform_DHD

  Set 4x4 vertex transformation matrix.

  Can be used for 3D perspective mapping or affine transformations
  (scale, translate, rotate).

  The matrix is expected to be in column-major format and use the following
  element order:
  > [ 0 4  8 12 ]
  > [ 1 5  9 13 ]
  > [ 2 6 10 14 ]
  > [ 3 7 11 15 ]
  (the translation vector is stored in elements 12, 13, and 14)

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  Matrix - Reference to 4x4 transformation matrix. 
  
  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTransform_DHD (r_drw2d_Device_t        Device,
                                                   const r_drw2d_FixedP_t *Matrix
                                                   );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxTextureTransform_DHD

  Set 3x2 texture transformation matrix.

  Can be used for e.g. affine transformations (scale, translate, rotate).

  The matrix is expected to be in column-major format and use the following
  element order:
  > [ 0 2 4 ]
  > [ 1 3 5 ]
  (the translation vector is stored in elements 4 and 5)

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  Matrix - Reference to 3x2 transformation matrix. 

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTextureTransform_DHD (r_drw2d_Device_t        Device,
                                                          const r_drw2d_FixedP_t *Matrix
                                                          );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxRotate_DHD

  Multiply current vertex matrix by rotation matrix.

  > [ cos(Angle), -sin(Angle), 0, 0 ]
  > [ sin(Angle),  cos(Angle), 0, 0 ]
  > [          0,           0, 1, 0 ]
  > [          0,           0, 0, 1 ]

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
   Angle - Angle of rotation about z axis.
           Angle is in degrees measure.
           When converting to fixed point value, we recommend using
           <R_DRW2D_2X> macros.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxRotate_DHD (r_drw2d_Device_t Device,
                                                r_drw2d_FixedP_t Angle
                                                );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxRotate3d_DHD

  Multiply current vertex matrix by 3d rotation matrix.

   > [ xx(1-c)+c    xy(1-c)-zs   xz(1-c)+ys   0 ]
   > [ yx(1-c)+zs   yy(1-c)+c    yz(1-c)-xs   0 ]
   > [ xz(1-c)-ys   yz(1-c)+xs   zz(1-c)+c    0 ]
   > [ 0            0            0            1 ]

  c=cos(Angle), s= sin(Angle)

  The argument (X, Y, Z) should be unit vector. 

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
       X - Coordinate of vector starting in origin (0,0,0)
       Y - Coordinate of vector starting in origin (0,0,0)
       Z - Coordinate of vector starting in origin (0,0,0)
   Angle - Angle of rotation around axis of vector defined by (X,Y,Z) in
           (0..360 degrees)
           Angle is in degrees measure.
           When converting to fixed point value, we recommend using
           <R_DRW2D_2X> macros.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxRotate3d_DHD (r_drw2d_Device_t Device,
                                                  r_drw2d_FixedP_t X,
                                                  r_drw2d_FixedP_t Y,
                                                  r_drw2d_FixedP_t Z,
                                                  r_drw2d_FixedP_t Angle
                                                  );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxTextureRotate_DHD

  Multiply current texture matrix by rotation matrix.

  > [ cos(Angle), -sin(Angle), 0 ]
  > [ sin(Angle),  cos(Angle), 0 ]


  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
   Angle - Angle of rotation about z axis (0..360 degrees)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTextureRotate_DHD (r_drw2d_Device_t Device,
                                                       r_drw2d_FixedP_t Angle
                                                       );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxScale_DHD

  Multiply current texture matrix by scaling matrix.

  > [ ScaleX, 0     , 0 ]
  > [      0, ScaleY, 0 ]

  When performing the same transformation as <R_DRW2D_CtxScale>,
  texture matrix should be inverse matrix of vertex matrix.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  ScaleX - Scale factor for the X axis (1.0 means no scaling)
  ScaleY - Scale factor for the Y axis (1.0 means no scaling)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxScale_DHD (r_drw2d_Device_t Device,
                                               r_drw2d_FixedP_t ScaleX,
                                               r_drw2d_FixedP_t ScaleY,
                                               r_drw2d_FixedP_t ScaleZ
                                               );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxTextureScale_DHD

  Multiply current texture matrix by scaling matrix.

  > [ ScaleX, 0     , 0 ]
  > [      0, ScaleY, 0 ]

  When performing the same transformation as <R_DRW2D_CtxScale>,
  texture matrix should be inverse matrix of vertex matrix.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  ScaleX - Scale factor for the X axis (1.0 means no scaling)
  ScaleY - Scale factor for the Y axis (1.0 means no scaling)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTextureScale_DHD (r_drw2d_Device_t Device,
                                                      r_drw2d_FixedP_t ScaleX,
                                                      r_drw2d_FixedP_t ScaleY
                                                      );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxTranslate_DHD

  Multiply current vertex matrix by translation matrix.

  > [ 1, 0, 0, TransX ]
  > [ 0, 1, 0, TransY ]
  > [ 0, 0, 1, TransZ ]
  > [ 0, 0, 0, 1      ]

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  TransX - X axis translation
  TransY - Y axis translation
  TransZ - Z axis translation

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTranslate_DHD (r_drw2d_Device_t Device,
                                                   r_drw2d_FixedP_t TransX,
                                                   r_drw2d_FixedP_t TransY,
                                                   r_drw2d_FixedP_t TransZ
                                                   );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxTextureTranslate_DHD

  Multiply current texture matrix by translation matrix.

  > [ 1, 0  TransX ]
  > [ 0, 1, TransY ]

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  TransX - X axis translation
  TransY - Y axis translation

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTextureTranslate_DHD (r_drw2d_Device_t Device,
                                                          r_drw2d_FixedP_t TransX,
                                                          r_drw2d_FixedP_t TransY
                                                          );



/***********************************************************
  Function: R_DRW2D_Platform_CtxFrustum_DHD

  Multiply current vertex matrix by perspective matrix.

  > [ (2*near)/(right-left), 0                    , (right+left)/(right-left), 0]
  > [ 0                    , (2*near)/(top-bottom), (top+bottom)/(top-bottom), 0]
  > [ 0                    , 0                    , -((far+near)/(far-near)) , -((2*far*near) / (far-near))]
  > [ 0                    , 0                    , -1                       , 0]

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
    Left - Left vertical clipping plane
   Right - Right vertical clipping plane
  Bottom - Bottom horizontal clipping plane
     Top - Top horizontal clipping plane
   ZNear - Distance to near clipping plane
    ZFar - Distance to far clipping plane

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxFrustum_DHD (r_drw2d_Device_t Device,
                                                 r_drw2d_FixedP_t Left,
                                                 r_drw2d_FixedP_t Right,
                                                 r_drw2d_FixedP_t Bottom,
                                                 r_drw2d_FixedP_t Top,
                                                 r_drw2d_FixedP_t ZNear,
                                                 r_drw2d_FixedP_t ZFar
                                                 );



/***********************************************************
  Function: R_DRW2D_Platform_VtxTransform_DHD

  Transform a list of vertices by the current vertex transformation matrix.

  This function applies the current transformation matrix to an arbitrary
  number of points supplied via the parameters Vec and NumVertices.

  This function can be used to get the scope of the next drawing operation
  before executing it.
  This information can be used to allocate buffers with as small as
  possible size, as the target scope of the drawing operation is already known.

  Parameters:
       Device - Device handle (see <r_drw2d_Device_t>)
     Vertices - Pointer to vertices
  NumVertices - Number of vertices provided by "Vertices" parameter

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_VtxTransform_DHD (r_drw2d_Device_t  Device, 
                                                   r_drw2d_Vec4_t   *Vertices,
                                                   uint32_t          NumVertices
                                                   );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxMatrix_DHD

  Get the 4x4 vertex transformation matrix and the 3x2 texture transformation
  matrix.

  Can be used to save and restore (<R_DRW2D_CtxTransform>,
  <R_DRW2D_CtxTextureTransform>)
  the current transformation matrices.
  This may reduce the CPU overhead of repeatedly calling almost identical
  transformation operations for several similar drawing operations.
  If just one of the matrices is required, set the other pointer to zero.

  VertexMatrix is 4x4 transformation matrix. 
  The matrix is expected to be in column-major format and use the following
  element order:  
   > [ 0 4  8 12 ]
   > [ 1 5  9 13 ]
   > [ 2 6 10 14 ]
   > [ 3 7 11 15 ]
  (the translation vector is stored in elements 12, 13, and 14)

  TextureMatrix is 3x2 transformation matrix. 
  The matrix is expected to be in column-major format and use the following
  element order:
   > [ 0 2 4 ]
   > [ 1 3 5 ]
  (the translation vector is stored in elements 4 and 5)
  
  Parameters:
         Device - Device handle (see <r_drw2d_Device_t>)
   VertexMatrix - Reference to 4x4 transformation matrix. 
  TextureMatrix - Reference to 3x2 transformation matrix. 

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxMatrix_DHD (r_drw2d_Device_t  Device,
                                                r_drw2d_FixedP_t *const VertexMatrix,
                                                r_drw2d_FixedP_t *const TextureMatrix
                                                );



/***********************************************************
  Function: R_DRW2D_Platform_ClutAlloc_DHD

  Allocates space for a CLUT used by <R_DRW2D_ClutSet> and
  <R_DRW2D_CtxClutSet>
  Call this function first when using CLUT.
  CLUT memory can store 512 elements in total.
  Control it so that it does not become fragmented.

  Parameters:
    Device - Device handle (see <r_drw2d_Device_t>)
      Size - Size of CLUT entries to allocate.
             Specify the Size according to the pixel format to use.
             When using R_DRW2D_PIXELFORMAT_CLUT_1, specify "2" for Size.
             When using R_DRW2D_PIXELFORMAT_CLUT_2, specify "4" for Size.
             When using R_DRW2D_PIXELFORMAT_CLUT_4, specify "16" for Size.
             When using R_DRW2D_PIXELFORMAT_CLUT_8, specify "256" for Size.
  ClutBase - This function will write the offset address of CLUT memory to
             store specified size.
             Clutbase is used as a handle value for subsequent CLUT APIs.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_ClutAlloc_DHD (r_drw2d_Device_t  Device,
                                                uint32_t          Size,
                                                uint32_t         *ClutBase
                                                );



/***********************************************************
  Function: R_DRW2D_Platform_ClutFree_DHD

  Frees CLUT memory previously allocated with <R_DRW2D_ClutAlloc>

  Parameters:
    Device - Device handle (see <r_drw2d_Device_t>)
      Size - Number of CLUT entries to free
  ClutBase - Offset address of CLUT memory.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_ClutFree_DHD (r_drw2d_Device_t Device,
                                               uint32_t        Size,
                                               uint32_t        ClutBase
                                               );



/***********************************************************
  Function: R_DRW2D_Platform_CtxClutSet_DHD

  Assign a previously created CLUT with the Offset ClutBase
  (as returned by <R_DRW2D_ClutAlloc>)
  to the texture.
  
  Before calling this function, call <R_DRW2D_ClutAlloc> and acquire ClutBase.

  Parameters:
    Device - Device handle (see <r_drw2d_Device_t>)
  ClutBase - Offset address of CLUT memory.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxClutSet_DHD (r_drw2d_Device_t  Device,
                                                 uint32_t          ClutBase);



/***********************************************************
  Function: R_DRW2D_Platform_ClutSet_DHD

  Create and set a CLUT that was previously allocated with <R_DRW2D_ClutAlloc>
  
  Before calling this function, call <R_DRW2D_ClutAlloc> and acquire ClutBase.
  
  The relationship between pixel format and "Data" and "Size" are as follows.
  * R_DRW2D_PIXELFORMAT_CLUT_1 - "Data" will be 2 colors of data.
                                 Specify 2 for "Size".
  * R_DRW2D_PIXELFORMAT_CLUT_2 - "Data" will be 4 colors of data.
                                 Specify 4 for "Size".
  * R_DRW2D_PIXELFORMAT_CLUT_4 - "Data" will be 16 colors of data.
                                 Specify 16 for "Size".
  * R_DRW2D_PIXELFORMAT_CLUT_8 - "Data" will be 256 colors of data.
                                 Specify 256 for "Size".

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
    Data - Pointer to the array of CLUT data. The data consists of
           32 bit ARGB color data.
   Start - Offset address of CLUT memory.
           Specify the ClutBase acquired with <R_DRW2D_ClutAlloc>.
    Size - Size of CLUT elements.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_ClutSet_DHD (r_drw2d_Device_t  Device,
                                              uint32_t         *Data,
                                              uint32_t          Start,
                                              uint32_t          Size
                                              );



/*******************************************************************************
  Function: R_DRW2D_Platform_FramebufferSet_DHD

  Set current destination framebuffer.
  Allocation of the framebuffer is the responsibility of the application.

  Parameters:
       Device - Device handle (see <r_drw2d_Device_t>)
  Framebuffer - Reference to framebuffer structure
                (see <r_drw2d_Framebuffer_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_FramebufferSet_DHD (r_drw2d_Device_t       Device,
                                                     r_drw2d_Framebuffer_t *Framebuffer
                                                     );



/*******************************************************************************
  Function: R_DRW2D_Platform_FramebufferClear_DHD

  Clears the current clip rectangle with the current background color 
  (can be set with <R_DRW2D_CtxBgColor>)

  <R_DRW2D_FramebufferClear> will always use the SOLID fill mode and ignore
  current blend mode/factor settings.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_FramebufferClear_DHD (r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawTriangles_DHD

  Render an array of triangles.

  Each triangle vertex will be transformed by the current vertex
  transformation matrix.

  If texture mapping is enabled, the texture will be applied according to the
  current texture transformation matrix.

  Note that the Dave HD platform is limited to an edge width/height of
  max. 2048 when using non antialiased edges (Edge flag set to 0).

  Parameters:
     Device - Device handle (see <r_drw2d_Device_t>)
     Points - Pointer to array of vertices (see <r_drw2d_Point_t>).
                The array should consist of Count elements.
      Count - Number of vertices. (Count/3) triangles will be drawn.
  EdgeFlags - Pointer to array of edge flag. The array should consist of
             (Count/3) elements.
             One byte per triangle. Specifies which triangles edge will be
             antialiased. 
             can be combined with OR setting. See <r_drw2d_EdgeFlag_t>.
             If NULL, do not use antialiasing. 

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_DrawTriangles_DHD (r_drw2d_Device_t       Device,
                                                    const r_drw2d_Point_t *Points,
                                                    uint32_t               Count,
                                                    const uint8_t         *EdgeFlags
                                                    );



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawTrianglesUV_DHD

  Render an array of UV texture mapped triangles.

  Each triangle vertex will be transformed by the current vertex
  transformation matrix.

  Each UV coordinate will be transformed by the current texture
  transformation matrix if the texture transform mode
  (<R_DRW2D_CtxTextureTransformMode>) is set to <R_DRW2D_TEX_TRANSFORM_2D>.

  If the fill mode is set to <R_DRW2D_FILLMODE_SOLID>, this function behaves
  like <R_DRW2D_DrawTriangles> and the UV coordinate array is ignored.

  Note that the Dave HD platform is limited to an edge width/height of
  max. 2048 when using non antialiased edges (Edge flag set to 0).

  Parameters:
     Device - Device handle (see <r_drw2d_Device_t>)
     Points - Pointer to array of vertices (see <r_drw2d_Point_t>).
              The array should consist of Count elements.
      Count - Number of vertices. (Count/3) triangles will be drawn.
  EdgeFlags - Pointer to array of edge flag. The array should consist of
              (Count/3) elements.
              One byte per triangle. Specifies which triangles edge will be
              antialiased. 
              Can be combined with OR setting. See <r_drw2d_EdgeFlag_t>.
              If NULL, do not use antialiasing. 
   UVCoords - Pointer to array of UV coordinates (see <r_drw2d_UVCoord_t>).
              The array should consist of Count elements.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_DrawTrianglesUV_DHD (r_drw2d_Device_t        Device,
                                                      const r_drw2d_Point_t   *Points,
                                                      uint32_t                 Count,
                                                      const uint8_t           *EdgeFlags,
                                                      const r_drw2d_UVCoord_t *UVCoords
                                                      );



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawRect_DHD

  Render a rectangle.

  The *Pos* and *Size* fields of the Rect argument are used to construct
  a quad which will then be transformed by the current vertex matrix.

  If texture mapping is enabled, the texture will be applied according to the
  current texture transformation matrix.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
    Rect - Rectangle position and size.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_DrawRect_DHD (r_drw2d_Device_t      Device,
                                               const r_drw2d_Rect_t *Rect
                                               );



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawRectUV_DHD

  Render a UV texture mapped rectangle.

  The *Pos* and *Size* fields of the Rect argument are used to construct
  a quad which will then be transformed by the current vertex matrix.

  Each UV coordinate will be transformed by the current texture
  transformation matrix if the texture transform mode
  (<R_DRW2D_CtxTextureTransformMode>) is set to <R_DRW2D_TEX_TRANSFORM_2D>.

  If the fill mode is set to <R_DRW2D_FILLMODE_SOLID>, this function behaves
  like <R_DRW2D_DrawRect> and the UV coordinate array is ignored.

  Parameters:
    Device - Device handle (see <r_drw2d_Device_t>)
      Rect - Rectangle position and size.
  UVCoords - Reference to an array of 4 UV coordinates
             (left/top, right/top, right/bottom, left/bottom order)
             (see <r_drw2d_UVCoord_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_DrawRectUV_DHD (r_drw2d_Device_t         Device,
                                                 const r_drw2d_Rect_t    *Rect,
                                                 const r_drw2d_UVCoord_t *UVCoords
                                                 );



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawQuads_DHD

  Renders an array of quadrilaterals.

  Each vertex will be transformed by the current vertex matrix.

  If texture mapping is enabled, the texture will be applied according to the
  current texture transformation matrix.

  Note that the Dave HD platform is limited to an edge width/height of
  max. 2048 when using non antialiased edges (Edge flag set to 0).

  Parameters:
     Device - Device handle (see <r_drw2d_Device_t>)
     Points - Pointer to array of vertices (see <r_drw2d_Point_t>).
              The array should consist of Count elements.
      Count - Number of vertices. (Count/4) quadrilaterals will be drawn.
  EdgeFlags - Pointer to array of edge flag. The array should consist of
              (Count/4) elements.
              One byte per quadrilaterals. Specifies which quadrilaterals
              edge will be antialiased.
              Can be combined with OR setting. See <r_drw2d_EdgeFlag_t>.
              If NULL, do not use antialiasing. 

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_DrawQuads_DHD (r_drw2d_Device_t       Device,
                                                const r_drw2d_Point_t *Points,
                                                uint32_t               Count,
                                                const uint8_t *        EdgeFlags
                                                );



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawQuadsUV_DHD

  Renders an array of UV texture mapped quadrilaterals.

  Each vertex will be transformed by the current vertex matrix.

  Each UV coordinate will be transformed by the current texture transformation
  matrix if the texture  transform mode (<R_DRW2D_CtxTextureTransformMode>) is
  set to <R_DRW2D_TEX_TRANSFORM_2D>.

  If the fill mode is set to <R_DRW2D_FILLMODE_SOLID>, this function behaves
  like <R_DRW2D_DrawQuads> and the UV coordinate array is ignored.

  Note that the Dave HD platform is limited to an edge width/height of
  max. 2048 when using non antialiased edges (Edge flag set to 0).

  Parameters:
     Device - Device handle (see <r_drw2d_Device_t>)
     Points - Pointer to array of vertices (see <r_drw2d_Point_t>)
              The array should consist of Count elements.
      Count - Number of vertices. (Count/4) quadrilaterals will be drawn.
  EdgeFlags - Pointer to array of edge flag. The array should consist of
              (Count/4) elements.
              One byte per quadrilaterals. Specifies which quadrilaterals
              edge will be antialiased.
              Can be combined with OR setting. See <r_drw2d_EdgeFlag_t>.
              If NULL, do not use antialiasing. 
   UVCoords - Pointer to array of UV coordinates (see <r_drw2d_UVCoord_t>).
              The array should consist of Count elements.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_DrawQuadsUV_DHD (r_drw2d_Device_t         Device,
                                                  const  r_drw2d_Point_t   *Points,
                                                  uint32_t                 Count,
                                                  uint8_t                 *EdgeFlags,
                                                  const r_drw2d_UVCoord_t *UVCoords
                                                  );



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawQuads3dUV_DHD

  Renders an array of UV texture mapped 3D-quadrilaterals.

  Each vertex will be transformed by the current vertex matrix.

  Each UV coordinate will be transformed by the current texture transformation
  matrix if the texture transform mode (<R_DRW2D_CtxTextureTransformMode>) is
  set to <R_DRW2D_TEX_TRANSFORM_2D>.

  If the fill mode is set to <R_DRW2D_FILLMODE_SOLID>, this function behaves
  like <R_DRW2D_DrawQuads> and the UV coordinate array is ignored.

  Note that the Dave HD platform is limited to an edge width/height of
  max. 2048 when using non antialiased edges (Edge flag set to 0).

  Parameters:
     Device - Device handle (see <r_drw2d_Device_t>)
     Points - Pointer to array of 3D vertices (see <r_drw2d_Vec4_t>).
              The array should consist of Count elements.
      Count - Number of vertices. (Count/4) quadrilaterals will be drawn.
  EdgeFlags - Pointer to array of edge flag. The array should consist of
              (Count/4) elements.
              One byte per quadrilaterals. Specifies which quadrilaterals
              edge will be antialiased.
              Can be combined with OR setting. See <r_drw2d_EdgeFlag_t>.
              If NULL, do not use antialiasing. 
   UVCoords - Pointer to array of UV coordinates (see <r_drw2d_UVCoord_t>).
              The array should consist of Count elements.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_DrawQuads3dUV_DHD (r_drw2d_Device_t         Device,
                                                    const r_drw2d_Vec4_t    *Points,
                                                    uint32_t                 Count,
                                                    uint8_t                 *EdgeFlags,
                                                    const r_drw2d_UVCoord_t *UVCoords
                                                    );



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawEllipse_DHD

  Render an ellipse at Point with the specified x and y radius.

  The center point will be transformed by the current vertex matrix.

  In order to draw a circle, use the same values for RadiusX and RadiusY.

  If texture mapping is enabled, the texture will be applied according to the
  current texture transformation matrix.

  Parameters:
   Device - Device handle (see <r_drw2d_Device_t>)
    Point - Center point (see <r_drw2d_Point_t>)
  RadiusX - Horizontal radius
  RadiusY - Vertical radius

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_DrawEllipse_DHD (
                                     r_drw2d_Device_t Device, 
                                     r_drw2d_Point_t  Point, 
                                     r_drw2d_FixedP_t RadiusX, 
                                     r_drw2d_FixedP_t RadiusY
                                     );



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawLines_DHD

  Render an array of lines. The rendered lines include the start and
  end points.

  Each line point will be transformed by the current transformation matrix.

  The *Width* field of <r_drw2d_LineStyle_t> specifies the line width.

  The *LineCap* field of <r_drw2d_LineStyle_t> specifies how to render the
  line starts/ends (see <r_drw2d_LineCap_t>).

  If texture mapping is enabled, the texture will be applied according to the
  current texture transformation matrix.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  Points - Pointer to array of Line start/end vertices
           (see r_drw2d_Point_t).
           The array should consist of Count elements.
           The order is (start of the 1st Line), (end of the 1st Line), 
           (start of the 2nd Line), (end of the 2nd Line), ...
   Count - Number of vertices. (Count/2) lines will be drawn.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_DrawLines_DHD (
        r_drw2d_Device_t Device,
  const r_drw2d_Point_t *Points,
        uint32_t         Count
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawPolyline_DHD

  Render a polyline consisting of one or many line segments.

  Each line point will be transformed by the current transformation matrix.

  The *Width* field of <r_drw2d_LineStyle_t> specifies the line width.

  If the *IsClosed* field of <r_drw2d_LineStyle_t> is set to 1 (true),
  the last segment will be connected to the first segment.

  The line segments will be connected as specified by the *LineJoin* field
  of <r_drw2d_LineStyle_t> 
  (see <R_DRW2D_CtxLineStyle>, <r_drw2d_LineStyle_t>, <r_drw2d_LineJoin_t>).

  If the <R_DRW2D_LINEJOIN_MITER> join type is selected, the *MiterLimit*
  field of <r_drw2d_LineStyle_t> specifies the maximum distance between the
  line join tip and line point.
  If the miter limit is exceeded, a bevel joint will be drawn at the miter
  limit position.

  If the polyline is not closed, the *LineCap* field of <r_drw2d_LineStyle_t>
  specifies how to render the polyline start/end (see <r_drw2d_LineCap_t>).

  If texture mapping is enabled, the texture will be applied according to the
  current texture transformation matrix.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  Points - Pointer to array of Line start/end vertices
           (see r_drw2d_Point_t).
           The array should consist of Count elements.
   Count - Number of vertices. (Count-1) or (Count) lines will be drawn
           depending on IsClosed setting.

  Returns:
  See <r_drw2d_Error_t>.

*/

r_drw2d_Error_t R_DRW2D_Platform_DrawPolyline_DHD (
        r_drw2d_Device_t  Device,
  const r_drw2d_Point_t  *Points,
        uint32_t          Count
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawBezierCurves_DHD

  Render a bezier curve consisting of one or more quadratic bezier segments.

  Each curve point will be transformed by the current transformation matrix.

  The *Width* field of <r_drw2d_LineStyle_t> specifies the line width. 
  The value must be less than 16.

  If the *IsClosed* field of <r_drw2d_LineStyle_t> is set to 1 (true),
  the last segment will be connected to the first segment by a straight line.

  The bezier segments will be connected by round joints.
  The ends of the curve will be flat.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  Points - Pointer to array of curve point vertices (see r_drw2d_Point_t).
           The array should consist of Count elements.
   Count - Number of vertices. ((Count - 1) / 2) curves will be drawn

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_DrawBezierCurves_DHD (
        r_drw2d_Device_t  Device, 
  const r_drw2d_Point_t  *Points,
        uint32_t          Count
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawRectConvolve1dx_DHD

  Apply one dimensional convolution filter to texture and
  store result in framebuffer.

  The currently selected 1D convolution kernel is applied in the
  horizontal direction, i.e. the kernel coefficients determine the weights
  of the pixel(s) to the left and right of the current texel while the
  source texture is being traversed.

  The *Pos* field of the Rect argument is used to select the
  framebuffer destination position.

  The *Size* field of the Rect argument is used to select the
  width and height of the convoluted area.

  The *TextureOffX* and *TextureOffY* arguments are used to select the
  left/top texel of the convolution source area.

  Unless the R_DRW2D_TEX_WRAPU flag is used, the texture width must be equal
  or greater than the kernel size.

  This function does not regard the vertex and texture transformation matrices.

  The fill mode must be set to <R_DRW2D_FILLMODE_TEXTURE>.

  Note that using textures with an improper pitch or height on D/AVE HD
  will lead to *severe performance loss*.
  The texture's pitch should be a multiple of 256 / BPP,
  where BPP is the current texture format's number of bits per pixel.
  The texture's height should be a multiple of 4. Furthermore,
  the texture's start address should be 8 byte aligned.

  Parameters:
       Device - Device handle (see <r_drw2d_Device_t>)
         Rect - Rectangle position and size (see <r_drw2d_IntRect_t>)
  TextureOffX - Horizontal texture offset (number of texels)
  TextureOffY - Vertical texture offset (number of texels)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_DrawRectConvolve1dx_DHD (
        r_drw2d_Device_t   Device,
  const r_drw2d_IntRect_t *Rect,
        uint16_t           TextureOffX,
        uint16_t           TextureOffY
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawRectConvolve1dy_DHD

  Apply one dimensional convolution filter to texture and store result
  in framebuffer.

  The currently selected 1D convolution kernel is applied in the
  vertical direction, i.e. the kernel coefficients determine the weights
  of the pixel(s) above and below the current texel while the source texture
  is being traversed.

  The *Pos* field of the Rect argument is used to select the
  framebufferdestination position.

  The *Size* field of the Rect argument is used to select the
  width and height of the convoluted area.

  The *TextureOffX* and *TextureOffY* arguments are used to select the
  left/top texel of the convolution source area.

  Unless the <R_DRW2D_TEX_WRAPV> flag is used, the texture height must be
  equal or greater than the kernel size.

  This function does not regard the vertex and texture transformation matrices.

  The fill mode must be set to <R_DRW2D_FILLMODE_TEXTURE>.

  Note that using textures with an improper pitch or height on D/AVE HD will
  lead to *severe performance loss*.
  The texture's pitch should be a multiple of 256 / BPP,
  where BPP is the current texture format's number of bits per pixel.
  The texture's height should be a multiple of 4. Furthermore,
  the texture's start address should be 8 byte aligned.

  Parameters:
       Device - Device handle (see <r_drw2d_Device_t>)
         Rect - Rectangle position and size (see <r_drw2d_IntRect_t>)
  TextureOffX - Horizontal texture offset (number of texels)
  TextureOffY - Vertical texture offset (number of texels)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_DrawRectConvolve1dy_DHD (
        r_drw2d_Device_t   Device,
  const r_drw2d_IntRect_t *Rect,
        uint16_t           TextureOffX,
        uint16_t           TextureOffY
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawRectConvolve2d_DHD

  Apply two dimensional convolution filter to texture and store result
  in framebuffer.

  The currently selected 2D convolution kernel is applied in both
  x and y directions, i.e. the kernel coefficients determine the weights
  of the pixel(s) above and below resp. to the left and right of the
  current texel while the source texture is being traversed.

  The *Pos* field of the Rect argument is used to select the
  framebuffer destination position.

  The *Size* field of the Rect argument is used to select the
  width and height of the convoluted 
  area.

  The *TextureOffX* and *TextureOffY* arguments are used to select the
  left/top texel of the 
  convolution source area.

  Unless the <R_DRW2D_TEX_WRAPU> flag is used, the texture width must be
  equal or greater than the kernel width.

  Unless the <R_DRW2D_TEX_WRAPV> flag is used, the texture height must be
  equal or greater than the kernel height.

  This function does not regard the vertex and texture transformation matrices.

  If the selected convolution kernel is separable, i.e. if the
  2d kernel matrix can be expressed as a product of a row and column vector
  (e.g. Gaussian blur), applications should convolute the texture in
  two passes using <R_DRW2D_DrawRectConvolve1dx> and
  <R_DRW2D_DrawRectConvolve1dy>. 
  Doing so will decrease computational complexity from O(n^2) to O(n).
  The drawback of this approach is that a temporary buffer is required
  to store the result of the first pass.

  Due to HW restrictions, not all separable kernels can be implemented
  using aforementioned two-pass rendering technique.
  In particular, the D/AVE HD can not process separable kernels
  that contain negative weights (e.g. Sobel edge detect),
  since the result pixels are clamped to the 0..255 range
  after the first pass.

  The fill mode must be set to <R_DRW2D_FILLMODE_TEXTURE>.

  Note that using textures with an improper pitch or height on D/AVE HD
  will lead to *severe performance loss*.
  The texture's pitch should be a multiple of 256 / BPP,
  where BPP is the current texture format's number of bits per pixel.
  The texture's height should be a multiple of 4. Furthermore,
  the texture's start address should be 8 byte aligned.

  Parameters:
       Device - Device handle (see <r_drw2d_Device_t>)
         Rect - Rectangle position and size (see <r_drw2d_IntRect_t>)
  TextureOffX - Horizontal texture offset (number of texels)
  TextureOffY - Vertical texture offset (number of texels)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_DrawRectConvolve2d_DHD (
        r_drw2d_Device_t   Device,
  const r_drw2d_IntRect_t *Rect,
        uint16_t           TextureOffX,
        uint16_t           TextureOffY
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_DrawRectConvolve_DHD

  Apply two dimensional convolution filter to texture and store result
  in framebuffer.

  The currently selected 2D convolution kernel is applied in both
  x and y directions, i.e. the kernel coefficients determine the
  weights of the pixel(s) above and below resp. to the left and right of the
  current texel while the source texture is being traversed.

  The *Pos* field of the Rect argument is used to select the
  framebuffer destination position.

  The *Size* field of the Rect argument is used to select the
  width and height of the convoluted area.

  The *TextureOffX* and *TextureOffY* arguments are used to select the
  left/top texel of the convolution source area.

  Unless the <R_DRW2D_TEX_WRAPU> flag is used, the texture width must be
  equal or greater than the kernel width.

  Unless the <R_DRW2D_TEX_WRAPV> flag is used, the texture height must be
  equal or greater than the kernel height.

  This function does not regard the vertex and texture transformation matrices.

  If the selected convolution kernel is separable, i.e. if the
  2d kernel matrix can be expressed as a product of a row and column vector
  (e.g. Gaussian blur), applications should convolute the texture in
  two passes using <R_DRW2D_DrawRectConvolve1dx> and
  <R_DRW2D_DrawRectConvolve1dy>. 
  Doing so will decrease computational complexity from O(n^2) to O(n).
  The drawback of this approach is that a temporary buffer is required
  to store the result of the first pass.

  Due to HW restrictions, not all separable kernels can be implemented
  using aforementioned two-pass rendering technique.
  In particular, the D/AVE HD can not process separable kernels
  that contain negative weights (e.g. Sobel edge detect),
  since the result pixels are clamped to the 0..255 range
  after the first pass.

  The fill mode must be set to <R_DRW2D_FILLMODE_TEXTURE>.

  Note that using textures with an improper pitch or height on D/AVE HD
  will lead to *severe performance loss*.
  The texture's pitch should be a multiple of 256 / BPP,
  where BPP is the current texture format's number of bits per pixel.
  The texture's height should be a multiple of 4. Furthermore,
  the texture's start address should be 8 byte aligned.

  Parameters:
       Device - Device handle (see <r_drw2d_Device_t>)
         Rect - Rectangle position and size (see <r_drw2d_IntRect_t>)
  TextureOffX - Horizontal texture offset (number of texels)
  TextureOffY - Vertical texture offset (number of texels)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_DrawRectConvolve_DHD (
        r_drw2d_Device_t   Device,
  const r_drw2d_IntRect_t *Rect,
        uint16_t           TextureOffX,
        uint16_t           TextureOffY
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxConvolutionKernelPreset1d_DHD

  Select 1d convolution kernel size and weights.


  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  Preset - Kernel preset (see <r_drw2d_ConvolutionKernelPreset1d_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxConvolutionKernelPreset1d_DHD (
        r_drw2d_Device_t                    Device,
        r_drw2d_ConvolutionKernelPreset1d_t Preset
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxConvolutionKernelPreset2d_DHD

  Select 2d convolution kernel size and weights.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  Preset - Kernel preset (see <r_drw2d_ConvolutionKernelPreset2d_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxConvolutionKernelPreset2d_DHD (
        r_drw2d_Device_t                    Device,
        r_drw2d_ConvolutionKernelPreset2d_t Preset
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_GetGaussKernel_DHD

  Computes a gauss gernel with the given size and sigma.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  Kernel - Pointer to an array that has a size of (Width*Height)
   Width - Width of the kernel (needs to be an odd value!).
  Height - Height of the kernel (needs to be an odd value!).
   Sigma - The sigma used to compute the gauss coefficients.

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_GetGaussKernel_DHD (
        r_drw2d_Device_t     Device,
        r_drw2d_FixedP_t    *Kernel,
        int32_t              Width,
        int32_t              Height,
        r_drw2d_FixedP_t     Sigma
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxConvolutionKernel_DHD

  Select 2d convolution kernel size and weights.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  Kernel - Convolution kernel (see <r_drw2d_ConvKernel_t>).
           Kernel width and height needs to be an odd value

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxConvolutionKernel_DHD (
        r_drw2d_Device_t           Device,
        const r_drw2d_ConvKernel_t *Kernel
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_CtxConvolutionMode_DHD

  Sets the convolution mode.

  Restriction: 
  If <R_DRW2D_CONVMODE_BLEEDING> is selcted,
  the convolution filter will output incorrect pixels 
  infrequently.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  Mode - Convolution mode (see <r_drw2d_ConvMode_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxConvolutionMode_DHD (
        r_drw2d_Device_t   Device,
        r_drw2d_ConvMode_t Mode
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_GpuFinish_DHD

  Tell the driver to explicitly trigger the finishing of the current
  drawing scene operation (display list execution).

  This function can block. i.e. wait for all commands in the GPU
  to be processed if the Block parameter is <R_DRW2D_FINISH_WAIT> or
  return immediately if the parameter is <R_DRW2D_FINISH_NOWAIT>. 
  
  Alternatively, a non-blocking approach can be used:
  Call once <R_DRW2D_FINISH_NOWAIT_MARK> 
  and later query with <R_DRW2D_GpuFinished>.
  
  Use of <R_DRW2D_FINISH_NOWAIT_MARK> and confirmation of
  RetFinshed = R_TRUE by <R_DRW2D_GpuFinished> are one to one relationship.
  If it doesn't confirm completion for the previous
  <R_DRW2D_FINISH_NOWAIT_MARK>, calling this function with 
  <R_DRW2D_FINISH_NOWAIT_MARK> has no effect.

 Parameters:
 Device - Device handle (see <r_drw2d_Device_t>)
  Block - Specify to wait for execution or not (see <r_drw2d_Finish_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_GpuFinish_DHD (r_drw2d_Device_t Device,
                                                r_drw2d_Finish_t Block
                                                );



/*******************************************************************************
  Function: R_DRW2D_Platform_GpuFinished_DHD

  Queries the driver for a yes/no whether there are still pending jobs
  in its pipeline.

  Can be used for single-threaded non-blocking use cases.
  <R_DRW2D_GpuFinish> with <R_DRW2D_FINISH_NOWAIT_MARK> must be be
  called once before this function.
  RetFinished = R_TRUE can only be checked once per
  <R_DRW2D_FINISH_NOWAIT_MARK>.

  Parameters:
       Device - Device handle (see <r_drw2d_Device_t>)
  RetFinished - Result whether the GPU is finished (yes/no)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_GpuFinished_DHD (
        r_drw2d_Device_t   Device, 
        r_drw2d_Boolean_t *RetFinished
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_GpuCmdListCreate_DHD

  Allocate empty command list.

  The application must call <R_DRW2D_GpuCmdListDelete> to delete th
  command list when it is no longer used.

  Parameters:
      Device - Device handle (see <r_drw2d_Device_t>)
  RetCmdList - Receives the allocated command list address
               (see <r_drw2d_GpuCmdList_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_GpuCmdListCreate_DHD (
        r_drw2d_Device_t      Device,
        r_drw2d_GpuCmdList_t *RetCmdList
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_GpuCmdListGenerate_DHD

  Record command list by calling an application provided function
  that invokes render commands.

  Any previously recorded command list data will be discarded.

  The command list must have been created using <R_DRW2D_GpuCmdListCreate>.

  Please notice that not all API functions may be called in the
  callback functions. 
  Drawing API functions are supported.

  Parameters:
    Device - Device handle (see <r_drw2d_Device_t>)
   CmdList - Command list handle (see <r_drw2d_GpuCmdList_t>)
       Cbk - Callback function (see <r_drw2d_GpuCmdListCallback_t>)
  UserData - Arbitrary user data which will be passed to the
             callback function

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_GpuCmdListGenerate_DHD (
        r_drw2d_Device_t             Device,
        r_drw2d_GpuCmdList_t         CmdList,
        r_drw2d_GpuCmdListCallback_t Cbk,
        void*                        UserData
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_GpuCmdListExec_DHD

  Execute previously recorded command list.

  Parameters:
    Device - Device handle (see <r_drw2d_Device_t>)
   CmdList - Command list handle (see <r_drw2d_GpuCmdList_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_GpuCmdListExec_DHD (
        r_drw2d_Device_t      Device,
        r_drw2d_GpuCmdList_t  CmdList
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_GpuCmdListCopy_DHD

  Copy command list data to memory area.

  If DestAddr is NULL, Size returns the required size (in bytes) and
  no command list data is copied.

  If RelocBaseAddr is != NULL, relocate jump commands so that the
  command list can later be executed from the given address
  (e.g. in flash memory).

  The copied command list may not be re-recorded or deleted using
  <R_DRW2D_GpuCmdListDelete>.

  Parameters:
         Device - Device handle (see <r_drw2d_Device_t>)
        CmdList - Command list handle (see <r_drw2d_GpuCmdList_t>)
       DestAddr - Where to copy the commandlist.
                  NULL to query required size.
           Size - If DestAddr is null, returns the required size. 
                  Otherwise this parameters determines the maximum number
                  of bytes that DestAddr can hold.
  RelocBaseAddr - If not null, specifies the start address
                  from where the command list can be executed later on
                  (e.g. a flash memory address).

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_GpuCmdListCopy_DHD (
        r_drw2d_Device_t      Device,
        r_drw2d_GpuCmdList_t  CmdList,
        void                 *DestAddr,
        uint32_t             *Size,
        void                 *RelocBaseAddr
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_GpuCmdListDelete_DHD

  Delete command list.

  The command list must have been created using <R_DRW2D_GpuCmdListCreate>.

  Parameters:
   Device - Device handle (see <r_drw2d_Device_t>)
  CmdList - Command list handle (see <r_drw2d_GpuCmdList_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_GpuCmdListDelete_DHD (
        r_drw2d_Device_t      Device,
        r_drw2d_GpuCmdList_t  CmdList
        );



/*******************************************************************************
  Function: R_DRW2D_Platform_PerfCountersAlloc_DHD

  Allocate hardware performance counter resources for this device context.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_PerfCountersAlloc_DHD (r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_Platform_PerfCountersFree_DHD

  Free hardware performance counter resources for this device context.


  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_PerfCountersFree_DHD (r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_Platform_PerfValueGet_DHD

  Query the driver for HW cycles of type Type and return the value
  in RetValue.

  The cycle counts are reset when <R_DRW2D_PerfValueReset> is called.

  <R_DRW2D_PerfCountersAlloc> must have been called to allocate the
  hardware performance counters for the DRW2D device context.

  Parameters:
    Device - Device handle (see <r_drw2d_Device_t>)
      Type - Performance type to query (see <r_drw2d_Performance_t>)
  RetValue - Cycle count is stored here (must not be NULL).

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_PerfValueGet_DHD (  r_drw2d_Device_t      Device,
                                                     r_drw2d_Performance_t Type,
                                                     uint32_t *            RetValue
                                                     ); 



/*******************************************************************************
  Function: R_DRW2D_Platform_PerfValueReset_DHD

  Reset the HW cycles of the given performance type to 0.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
    Type - Performance type to query (see <r_drw2d_Performance_t>)

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_PerfValueReset_DHD (  r_drw2d_Device_t      Device,
                                                       r_drw2d_Performance_t Type
                                                       ); 



#ifdef __cplusplus
}
#endif

#endif /* R_DRW2D_SYS_DHD_H_ */

