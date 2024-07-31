/*
****************************************************************************
PROJECT : Drw2D driver
FILE    : $Id: r_drw2d_sys_cpu.h 14751 2017-09-25 04:27:33Z shinya.tomari $
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

#ifndef R_DRW2D_SYS_CPU_H_
#define R_DRW2D_SYS_CPU_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: DRW2D Support Functions for CPU (internal)

  DRW2D driver support functions

  The generic DRW2D driver uses these functions. They have to be implemented 
  within the driver library for a concrete device. (e.g. D1L, D1M)

  Support functions are functions that are not part of the driver itself 
  but they must be provided to integrate the driver on a particular board.

  It is not recommended to use this API directly.
  Use R_DRW2D_API_* instead.
*/


/***********************************************************
  Section: Global Defines
*/


/***********************************************************
  Section: DRW2D driver support functions

  The DRW2D API supports different devices with different graphics driver macros.

  It is also possible to use different macros in the same device.
*/



/***********************************************************
  Group: DRW2D Basic interface functions
*/

/***********************************************************
  Function: R_DRW2D_Platform_Init_CPU

  Initialize DRW2D API and initialize global DRW2D resources.

  Must be called during system startup.

  Not threadsafe, The application must ensure that only one thread calls this function.

  The underlying drawing engine should be initialized prior to calling this function. 

  Parameters:
  None

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_Init_CPU (void);



/***********************************************************
  Function: R_DRW2D_Platform_Exit_CPU

  Shutdown DRW2D API and de-initialize global DRW2D resources.

  Must be called during system shutdown.

  Must not be called when <R_DRW2D_Init> has failed.

  Not threadsafe, The application must ensure that all DRW2D units have been closed and
  only one thread calls this function.
  
  The underlying drawing engine should be de-initialized after calling this function. 

  Parameters:
  None

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_Exit_CPU (void);



/***********************************************************
  Function: R_DRW2D_Platform_Open_CPU

  Initialize DRW2D unit and driver-dependent graphics engine and return device handle.

  This function initializes the driver-dependent graphics engine to its default configuration.
  It initializes the internal device structure and returns an opaque handle to that structure.

  A default render context is created implicitly.

  <R_DRW2D_Open> must be called before any drawing function can take place.

  The application must ensure that the RetDevice is not used in more than one thread at a time.
  Ensure that you provide a real structure pointer as DeviceInternal, not a void pointer!
  Also ensure that the DeviceInternal structure members are initialised with 0.

  Parameters:
            Unit - DRW2D unit number (0..(R_DRW2D_NUM_UNITS - 1))
    HwInstanceNr - DaveHD hardware instance nr
  DeviceInternal - Pointer to the gfx driver handle,
                     When using DaveHD port, set r_drw2d_DeviceDHD_t.
                     It is defined in r_drw2d_ctx_dhd.h.
                     Normally, this variable is never used by user,
                     but it must be kept until <R_DRW2D_Platform_Close> is called.
       RetDevice - Returns the (opaque) Drw2d device handle

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_Open_CPU (r_drw2d_Unit_t         Unit,
                                           int32_t                HwInstanceNr,
                                           void                  *DeviceInternal,
                                           r_drw2d_Device_t      *RetDevice
                                           );



/***********************************************************
  Function: R_DRW2D_Platform_RenderContext_Init_CPU

  Initialize a render context

  Parameters:
  RenderContext - Reference to uninitialized render context structure
     DeviceBase - Address of device context base class

  Returns:
  None
*/
void R_DRW2D_Platform_RenderContext_Init_CPU (r_drw2d_RenderContext_t *RenderContext,
                                              r_drw2d_DeviceBase_t    *DeviceBase
                                              );




/***********************************************************
  Function: R_DRW2D_Platform_Close_CPU

  Shutdown DRW2D unit, de-initialize driver-dependent graphics engine and cleans up 
  its internally used resources.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_Close_CPU (r_drw2d_Device_t Device);



/***********************************************************
  Function: R_DRW2D_Platform_ContextInit_CPU

  Initialize a render context with default settings.

  Parameters:
         Device - Device handle (see <r_drw2d_Device_t>)
  RenderContext - Reference to uninitialized render context structure. 
                  (see <r_drw2d_RenderContext_s>).
     RetContext - The context pointer that is initialized

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_ContextInit_CPU (r_drw2d_Device_t                Device,
                                                  struct r_drw2d_RenderContext_s *RenderContext,
                                                  r_drw2d_Context_t              *RetContext
                                                  );



/*******************************************************************************
  Function: R_DRW2D_Platform_ContextSelect_CPU

  Sets the given context as the current one. 
  Passing Context handle NULL will select the default context.

  Parameters:
   Device - Device handle (see <r_drw2d_Device_t>)
  Context - Render context handle (see <r_drw2d_Context_t>)

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_ContextSelect_CPU (r_drw2d_Device_t  Device,
                                                    r_drw2d_Context_t Context
                                                    );



/***********************************************************
  Function: R_DRW2D_Platform_CtxBgColor_CPU

  Set the background color to be used for drawing primitives.
  
  The <R_DRW2D_FramebufferClear> function always uses the current background color.

  Between calls to <R_DRW2D_CtxEffectsSet> and <R_DRW2D_CtxEffectsDelete>, 
  this function is invalid and an error will be returned.

  Parameters:
   Device - Device handle (see <r_drw2d_Device_t>)
    Color - The color value (32bit packed ARGB)

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxBgColor_CPU (r_drw2d_Device_t Device,
                                                 r_drw2d_Color_t  Color
                                                 );



/***********************************************************
  Function: R_DRW2D_Platform_CtxClipRect_CPU

  Sets a global clipping rectangle for subsequent drawing operations.
  A clip-rect can be set by calling this function before each R_DRW2D_Draw* API 
  and R_DRW2D_FramebufferClear.

  Parameters:
   Device - Device handle (see <r_drw2d_Device_t>)
     Rect - Rectangular clipping area (window coordinates) (see <r_drw2d_IntRect_t>).
            All elements must be positive numbers (or 0).

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxClipRect_CPU (r_drw2d_Device_t   Device,
                                                  const r_drw2d_IntRect_t *Rect
                                                  );



/***********************************************************
  Function: R_DRW2D_Platform_CtxFillMode_CPU

  Set the filling mode for drawing with primitives.

  This is used for placing e.g. a texture on top of any primitive.

  Between calls to <R_DRW2D_CtxEffectsSet> and <R_DRW2D_CtxEffectsDelete>, 
  this function is invalid and an error will be returned.

  Parameters:
   Device - Device handle (see <r_drw2d_Device_t>)
     Mode - The fill mode to be used. See <r_drw2d_FillMode_t>.

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxFillMode_CPU (r_drw2d_Device_t   Device,
                                                  r_drw2d_FillMode_t Mode
                                                  );



/***********************************************************
  Function: R_DRW2D_Platform_CtxBlendMode_CPU

  Set preset color/alpha source/destination blending equations.

  There are restrictions on the supported blend mode. For the supported blend mode, 
  see <r_drw2d_BlendMode_t>.

  If you set an unsupported blend mode, R_DRW2D_CtxBlendMode returns R_DRW2D_ERR_OK.
  However <R_DRW2D_ERR_NOT_SUPPORTED> will be returned by the drawing call.

  Parameters:
     Device - Device handle (see <r_drw2d_Device_t>)
  BlendMode - The blendmode mode to be used. See <r_drw2d_BlendMode_t>.

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxBlendMode_CPU (r_drw2d_Device_t    Device,
                                                   r_drw2d_BlendMode_t BlendMode
                                                   );



/***********************************************************
  Function: R_DRW2D_Platform_CtxTransformMode_CPU

  Set the vertex transform mode.

  The default is <R_DRW2D_TRANSFORM_2D> (2D vertex matrix transformation).

  When set to <R_DRW2D_TRANSFORM_NONE>, vertices will be used as-is, i.e. they
  will not be transformed by the vertex matrix. This mode can be used as a
  performance optimization.
  When no explicit UV coordinates are given, the texture is mapped to the framebuffer 1:1.

  There are restrictions on the supported transform mode. Specifying an unsupported 
  transform mode returns <R_DRW2D_ERR_NOT_SUPPORTED>.

  For the supported transform mode, see <r_drw2d_TransformMode_t>.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
    Mode - The transform mode to be used. See <r_drw2d_TransformMode_t>.

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTransformMode_CPU (r_drw2d_Device_t        Device,
                                                       r_drw2d_TransformMode_t Mode
                                                       );



/***********************************************************
  Function: R_DRW2D_Platform_CtxViewport_CPU

  Set viewport for drawing operation.

  The default is to use the current framebuffer width/height, offset by (0; 0).

  After setting a custom viewport, set the Rect.Size to (0; 0) to revert to the default behaviour.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
    Rect - The viewport rectangle. See <r_drw2d_IntRect_t>.

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxViewport_CPU (r_drw2d_Device_t         Device,
                                                  const r_drw2d_IntRect_t *Rect
                                                  );



/***********************************************************
  Function: R_DRW2D_Platform_CtxEffectsSet_CPU

  Sets an array of effects to be used for colorization and blending.
  The data in the array must be readable/writeable until a call to <R_DRW2D_CtxEffectsDelete>.

  The content of the array will be modified by <R_DRW2D_CtxEffectsUpdate>.

  Between calls to <R_DRW2D_CtxEffectsSet> and <R_DRW2D_CtxEffectsDelete> any calls to functions
  <R_DRW2D_CtxFgColor>, <R_DRW2D_CtxBgColor>, and <R_DRW2D_CtxFillMode> are invalid and an error
  will be returned.

  There are restrictions on the supported effect. For the supported effect, see <r_drw2d_EffectName_t>.

  The effects <R_DRW2D_EFFECT_REPLACE> and <R_DRW2D_EFFECT_MODULATE> can be combined to calculate 
  a final color.
  If the input source is specified as <R_DRW2D_EFFECT_SOURCE_PREV_STAGE>, the result of the 
  previous stage will be used.

  If the combination of effects can not be realized, <R_DRW2D_ERR_NOT_SUPPORTED> will be returned 
  by the drawing call.

  The combination of effects depends on the value of the count parameter.

  The valid combinations are:
  If the count is 1, the effect specifies "R_DRW2D_EFFECT_MODULATE".
  If the count is 2, the effect specifies "R_DRW2D_EFFECT_CONSTANT_ALPHA" and "R_DRW2D_EFFECT_REPLACE".

  The effects <R_DRW2D_EFFECT_CONSTANT_ALPHA> provide a final alpha blending.
  
  It is necessary to set the image quality to <R_DRW2D_IMGQUALITY_MEDIUM> or 
  <R_DRW2D_IMGQUALITY_HIGH> (see <R_DRW2D_CtxImgQuality>) for these effects to work:
  
  In the case of <R_DRW2D_EFFECT_CONSTANT_ALPHA> the result will be blended with a constant alpha.

  See User's manual for more information on how to use effects.

  Parameters:
   Device - Device handle (see <r_drw2d_Device_t>)
  Effects - Array with effects (see <r_drw2d_EffectStage_t>)
    Count - Number of effects in the array

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxEffectsSet_CPU(r_drw2d_Device_t       Device, 
                                                   r_drw2d_EffectStage_t* Effects, 
                                                   uint32_t               Count
                                                   );



/*******************************************************************************
  Function: R_DRW2D_CtxEffectsUpdate

  Updates effect at stage Stage.

  There are restrictions on the supported effect. For the supported effect, 
  see <r_drw2d_EffectName_t>.

  If the combination of effects cannot be realized, <R_DRW2D_ERR_NOT_SUPPORTED> 
  will be returned by the drawing call.

  Parameters:
     Device - Device handle (see <r_drw2d_Device_t>)
       Name - Effect (see <r_drw2d_EffectName_t>)
      Stage - Stage of effect to be updated
  NumParams - Number of parameters to be passed
     Params - Array of parameters (see <r_drw2d_EffectParam_t>)

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxEffectsUpdate_CPU (r_drw2d_Device_t Device,
                                                       r_drw2d_EffectName_t Name,
                                                       uint32_t Stage,
                                                       uint32_t NumParams,
                                                       const r_drw2d_EffectParam_t *Params
                                                       );



/*******************************************************************************
  Function: R_DRW2D_CtxEffectsDelete

  Deletes all effects.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxEffectsDelete_CPU (r_drw2d_Device_t Device);



/***********************************************************
  Function: R_DRW2D_Platform_CtxTextureSet_CPU

  Set source texture.

  The Drw2D supports the direct / zero-copy use of an application provided texture 
  address.
  
  The texture is used when the fill mode is set to <R_DRW2D_FILLMODE_TEXTURE>.

  Parameters:
       Device - Device handle (see <r_drw2d_Device_t>)
  TextureUnit - Texture Unit Number.
      Texture - Reference to texture structure (see <r_drw2d_Texture_t>). 
                  NULL to deselect current texture.

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTextureSet_CPU (r_drw2d_Device_t         Device,
                                                    uint32_t                 TextureUnit,
                                                    const r_drw2d_Texture_t *Texture
                                                    );



/***********************************************************
  Function: R_DRW2D_Platform_TextureBlit_CPU

  Blit texture from Src to Dest.

  Src can be NULL, in that case the blit origin is 0,0 inside of the texture and dimensions are 
  taken from the DstRect.

  If you have unsupported settings for blend mode or effect, R_DRW2D_TextureBlit 
  will return R_DRW2D_ERR_NOT_SUPPORT.

  Parameters:
   Device - Device handle (see <r_drw2d_Device_t>)
  SrcRect - Source rectangle
  DstRect - Destination rectangle

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_TextureBlit_CPU (r_drw2d_Device_t      Device,
                                                  const r_drw2d_Rect_t *SrcRect,
                                                  const r_drw2d_Rect_t *DstRect
                                                  );



/***********************************************************
  Function: R_DRW2D_Platform_CtxIdentity_CPU

  Reset vertex transformation matrix.

  The identity matrix maps the vertex at (0; 0) to the top/left framebuffer position.

  The vertex at (framebuffer_width-1, framebuffer_height-1) is mapped to the bottom/right
  framebuffer position.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxIdentity_CPU (r_drw2d_Device_t Device);



/***********************************************************
  Function: R_DRW2D_Platform_CtxTransform_CPU

  Set 4x4 vertex transformation matrix.

  Can be used affine transformations (scale, translate, rotate).

  The 4x4 vertex transformation matrix is updated, but does not use elements 
  2, 3, 6, 7, 8, 9, 10, 11, 14, and 15 in drawing call.

  The matrix is expected to be in column-major format and use the following element order:
  > [ 0 4  8 12 ]
  > [ 1 5  9 13 ]
  > [ 2 6 10 14 ]
  > [ 3 7 11 15 ]
  (the translation vector is stored in elements 12, 13, and 14)

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  Matrix - Reference to 4x4 transformation matrix. 
  
  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTransform_CPU (r_drw2d_Device_t        Device,
                                                   const r_drw2d_FixedP_t *Matrix
                                                   );



/***********************************************************
  Function: R_DRW2D_Platform_CtxRotate_CPU

  Multiply current vertex matrix by rotation matrix.

  > [ cos(Angle), -sin(Angle), 0, 0 ]
  > [ sin(Angle),  cos(Angle), 0, 0 ]
  > [          0,           0, 1, 0 ]
  > [          0,           0, 0, 1 ]

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
   Angle - Angle of rotation about z axis.
           Angle is in degrees measure.
           When converting to fixed point value, we recommend using <R_DRW2D_2X> macros.

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxRotate_CPU (r_drw2d_Device_t Device,
                                                r_drw2d_FixedP_t Angle
                                                );



/***********************************************************
  Function: R_DRW2D_Platform_CtxScale_CPU

  Multiply current vertex matrix by scaling matrix.

  The 4x4 vertex transformation matrix is updated, but does not use 
  ScaleZ perameter in drawing call.

  > [ ScaleX, 0     , 0,      0 ]
  > [      0, ScaleY, 0,      0 ]
  > [      0, 0     , ScaleZ, 0 ]
  > [      0, 0     , 0,      1 ]

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  ScaleX - Scale factor for the X axis (1.0 means no scaling)
  ScaleY - Scale factor for the Y axis (1.0 means no scaling)
  ScaleZ - Scale factor for the Z axis (1.0 means no scaling)

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxScale_CPU (r_drw2d_Device_t Device,
                                               r_drw2d_FixedP_t ScaleX,
                                               r_drw2d_FixedP_t ScaleY,
                                               r_drw2d_FixedP_t ScaleZ
                                               );



/***********************************************************
  Function: R_DRW2D_Platform_CtxTranslate_CPU

  Multiply current vertex matrix by translation matrix.

  The 4x4 vertex transformation matrix is updated, but does not use 
  TransZ perameter in drawing call.

  > [ 1, 0, 0, TransX ]
  > [ 0, 1, 0, TransY ]
  > [ 0, 0, 1, TransZ ]
  > [ 0, 0, 0, 1      ]

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
  TransX - X axis translation
  TransY - Y axis translation
  TransZ - Z axis translation
*/
r_drw2d_Error_t R_DRW2D_Platform_CtxTranslate_CPU (r_drw2d_Device_t Device,
                                                   r_drw2d_FixedP_t TransX,
                                                   r_drw2d_FixedP_t TransY,
                                                   r_drw2d_FixedP_t TransZ
                                                   );



/***********************************************************
  Function: R_DRW2D_Platform_CtxMatrix_CPU

  Get the 4x4 vertex transformation matrix and the 3x2 texture transformation matrix.

  Can be used to save and restore (<R_DRW2D_CtxTransform>, <R_DRW2D_CtxTextureTransform>)
  the current transformation matrices.
  This may reduce the CPU overhead of repeatedly calling almost identical transformation
  operations for several similar drawing operations.
  If just one of the matrices is required, set the other pointer to zero.

  VertexMatrix is 4x4 transformation matrix. 
  The matrix is expected to be in column-major format and use the following element order:  
   > [ 0 4  8 12 ]
   > [ 1 5  9 13 ]
   > [ 2 6 10 14 ]
   > [ 3 7 11 15 ]
  (the translation vector is stored in elements 12, 13, and 14)

  TextureMatrix is 3x2 transformation matrix. 
  The matrix is expected to be in column-major format and use the following element order:
   > [ 0 2 4 ]
   > [ 1 3 5 ]
  (the translation vector is stored in elements 4 and 5)
  
  Parameters:
         Device - Device handle (see <r_drw2d_Device_t>)
   VertexMatrix - Reference to 4x4 transformation matrix. 
  TextureMatrix - Reference to 3x2 transformation matrix. 

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_CtxMatrix_CPU (r_drw2d_Device_t  Device,
                                                r_drw2d_FixedP_t *const VertexMatrix,
                                                r_drw2d_FixedP_t *const TextureMatrix
                                                );



/***********************************************************
  Function: R_DRW2D_Platform_FramebufferSet_CPU

  Set current destination framebuffer.
  Allocation of the framebuffer is the responsibility of the application.

  Parameters:
       Device - Device handle (see <r_drw2d_Device_t>)
  Framebuffer - Reference to framebuffer structure (see <r_drw2d_Framebuffer_t>)

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_FramebufferSet_CPU (r_drw2d_Device_t       Device,
                                                     r_drw2d_Framebuffer_t *Framebuffer
                                                     );



/***********************************************************
  Function: R_DRW2D_Platform_FramebufferClear_CPU

  Clears the current clip rectangle with the current background color 
  (can be set with <R_DRW2D_CtxBgColor>)

  <R_DRW2D_FramebufferClear> will always use the SOLID fill mode and ignore current 
  blend mode/factor settings.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_FramebufferClear_CPU (r_drw2d_Device_t Device);



/*******************************************************************************
  Function: R_DRW2D_Platform_GpuFinish_CPU

  It will return immediately without any processing.

  Parameters:
  Device - Device handle (see <r_drw2d_Device_t>)
   Block - Specify to wait for execution or not (see <r_drw2d_Finish_t>)

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_GpuFinish_CPU (r_drw2d_Device_t Device,
                                                r_drw2d_Finish_t Block
                                                );



/*******************************************************************************
  Function: R_DRW2D_Platform_GpuFinished_CPU

  It will return immediately without any processing.

  Parameters:
       Device - Device handle (see <r_drw2d_Device_t>)
  RetFinished - Result whether the GPU is finished (yes/no)

  Returns:
  See <r_drw2d_Error_t>

*/
r_drw2d_Error_t R_DRW2D_Platform_GpuFinished_CPU (r_drw2d_Device_t   Device,
                                                  r_drw2d_Boolean_t *RetFinished);



#ifdef __cplusplus
}
#endif

#endif /* R_DRW2D_SYS_H_ */
