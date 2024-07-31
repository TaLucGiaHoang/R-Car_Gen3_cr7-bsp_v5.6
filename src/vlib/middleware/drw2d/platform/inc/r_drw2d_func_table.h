/*
****************************************************************************
PROJECT : Drw2D driver
FILE    : $Id: r_drw2d_func_table.h 17204 2020-06-03 02:57:47Z shinya.tomari.wj@renesas.com $
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

#ifndef R_DRW2D_FUNC_TABLE_H_
#define R_DRW2D_FUNC_TABLE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <r_drw2d_api.h>


/*******************************************************************************
  Title: DRW2D FUNC TABLE

  DRW2D (simple and slim 2D rendering) FUNC TABLE.
*/


/***************************************************************************
  Section: Global Defines
*/

/***************************************************************************
  Section: Global Types
*/



/*******************************************************************************
  Type: r_drw2d_Platform_Close_t

  For parameter specifications, see the R_DRW2D_Close function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_Close_t)(r_drw2d_Device_t Device);



/*******************************************************************************
  Type: r_drw2d_Platform_NativeDriverHandleGet_t

  For parameter specifications, see the R_DRW2D_NativeDriverHandleGet function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_NativeDriverHandleGet_t)(r_drw2d_Device_t   Device,
                                                                    void             **RetNativeDrvHandle
                                                                    );



/*******************************************************************************
  Type: r_drw2d_Platform_NativeDriverBegin_t

  For parameter specifications, see the R_DRW2D_NativeDriverBegin function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_NativeDriverBegin_t)(r_drw2d_Device_t Device,
                                                                uint32_t     Flags
                                                                );



/*******************************************************************************
  Type: r_drw2d_Platform_NativeDriverEnd_t

  For parameter specifications, see the R_DRW2D_NativeDriverEnd function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_NativeDriverEnd_t)(r_drw2d_Device_t Device,
                                                              uint32_t         Flags
                                                              );



/*******************************************************************************
  Type: r_drw2d_Platform_ContextInit_t

  For parameter specifications, see the R_DRW2D_ContextInit function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_ContextInit_t)(r_drw2d_Device_t                Device,
                                                          struct r_drw2d_RenderContext_s *RenderContext,
                                                          r_drw2d_Context_t              *RetContext
                                                          );



/*******************************************************************************
  Type: r_drw2d_Platform_ContextSelect_t

  For parameter specifications, see the R_DRW2D_ContextSelect function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_ContextSelect_t)(r_drw2d_Device_t  Device,
                                                            r_drw2d_Context_t Context
                                                            );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxFgColor_t

  For parameter specifications, see the R_DRW2D_CtxFgColor function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxFgColor_t)(r_drw2d_Device_t Device,
                                                         r_drw2d_Color_t  Color
                                                         );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxBgColor_t

  For parameter specifications, see the R_DRW2D_CtxBgColor function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxBgColor_t)(r_drw2d_Device_t Device,
                                                         r_drw2d_Color_t  Color
                                                         );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxClipRect_t

  For parameter specifications, see the R_DRW2D_CtxClipRect function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxClipRect_t)(r_drw2d_Device_t         Device,
                                                          const r_drw2d_IntRect_t *Rect
                                                          );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxFillMode_t

  For parameter specifications, see the R_DRW2D_CtxFillMode function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxFillMode_t)(r_drw2d_Device_t   Device,
                                                          r_drw2d_FillMode_t Mode
                                                          );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxCullMode_t

  For parameter specifications, see the R_DRW2D_CtxCullMode function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxCullMode_t)(r_drw2d_Device_t   Device,
                                                          r_drw2d_CullMode_t CullMode
                                                          );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxLineStyle_t

  For parameter specifications, see the R_DRW2D_CtxLineStyle function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxLineStyle_t)(r_drw2d_Device_t           Device,
                                                           const r_drw2d_LineStyle_t *Style
                                                           );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxBlendMode_t

  For parameter specifications, see the R_DRW2D_CtxBlendMode function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxBlendMode_t)(r_drw2d_Device_t    Device,
                                                           r_drw2d_BlendMode_t BlendMode
                                                           );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxBlendFactors_t

  For parameter specifications, see the R_DRW2D_CtxBlendFactors function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxBlendFactors_t)(r_drw2d_Device_t      Device,
                                                              r_drw2d_BlendFactor_t SrcRGB,
                                                              r_drw2d_BlendFactor_t DstRGB,
                                                              r_drw2d_BlendFactor_t SrcAlpha,
                                                              r_drw2d_BlendFactor_t DstAlpha
                                                              );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxImgQuality_t

  For parameter specifications, see the R_DRW2D_CtxImgQuality function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxImgQuality_t)(r_drw2d_Device_t     Device,
                                                            r_drw2d_ImgQuality_t Quality
                                                            );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxTransformMode_t

  For parameter specifications, see the R_DRW2D_CtxTransformMode function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxTransformMode_t)(r_drw2d_Device_t        Device,
                                                               r_drw2d_TransformMode_t Mode
                                                               );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxTextureTransformMode_t

  For parameter specifications, see the R_DRW2D_CtxTextureTransformMode function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxTextureTransformMode_t)(r_drw2d_Device_t               Device,
                                                                      r_drw2d_TextureTransformMode_t Mode
                                                                      );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxViewport_t

  For parameter specifications, see the R_DRW2D_CtxViewport function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxViewport_t)(r_drw2d_Device_t         Device,
                                                          const r_drw2d_IntRect_t *Rect
                                                          );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxStripingEnable_t

  For parameter specifications, see the R_DRW2D_CtxStripingEnable function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxStripingEnable_t)(r_drw2d_Device_t Device);



/*******************************************************************************
  Type: r_drw2d_Platform_CtxStripingDisable_t

  For parameter specifications, see the R_DRW2D_CtxStripingDisable function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxStripingDisable_t)(r_drw2d_Device_t Device);



/*******************************************************************************
  Type: r_drw2d_Platform_CtxEffectsSet_t

  For parameter specifications, see the R_DRW2D_CtxEffectsSet function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxEffectsSet_t)(r_drw2d_Device_t       Device,
                                                            r_drw2d_EffectStage_t *Effects,
                                                            uint32_t               Count
                                                            );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxEffectsUpdate_t

  For parameter specifications, see the R_DRW2D_CtxEffectsUpdate function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxEffectsUpdate_t)(r_drw2d_Device_t             Device,
                                                               r_drw2d_EffectName_t         Name,
                                                               uint32_t                     Stage,
                                                               uint32_t                     NumParams,
                                                               const r_drw2d_EffectParam_t *Params
                                                               );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxEffectsDelete_t

  For parameter specifications, see the R_DRW2D_CtxEffectsDelete function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxEffectsDelete_t)(r_drw2d_Device_t Device);



/*******************************************************************************
  Type: r_drw2d_Platform_CtxTextureSet_t

  For parameter specifications, see the R_DRW2D_CtxTextureSet function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxTextureSet_t)(r_drw2d_Device_t         Device,
                                                            uint32_t                 TextureUnit,
                                                            const r_drw2d_Texture_t *Texture
                                                            );



/*******************************************************************************
  Type: r_drw2d_Platform_TextureBlit_t

  For parameter specifications, see the R_DRW2D_TextureBlit function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_TextureBlit_t)(r_drw2d_Device_t      Device,
                                                          const r_drw2d_Rect_t *SrcRect,
                                                          const r_drw2d_Rect_t *DstRect
                                                          );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxTextureColorKeyEnable_t

  For parameter specifications, see the R_DRW2D_CtxTextureColorKeyEnable function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxTextureColorKeyEnable_t)(r_drw2d_Device_t Device,
                                                                       r_drw2d_Color_t  ColorKey
                                                                       );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxTextureColorKeyDisable_t

  For parameter specifications, see the R_DRW2D_CtxTextureColorKeyDisable function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxTextureColorKeyDisable_t)(r_drw2d_Device_t Device);



/*******************************************************************************
  Type: r_drw2d_Platform_CtxIdentity_t

  For parameter specifications, see the R_DRW2D_CtxIdentity function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxIdentity_t)(r_drw2d_Device_t Device);



/*******************************************************************************
  Type: r_drw2d_Platform_CtxTextureIdentity_t

  For parameter specifications, see the R_DRW2D_CtxTextureIdentity function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxTextureIdentity_t)(r_drw2d_Device_t Device);



/*******************************************************************************
  Type: r_drw2d_Platform_CtxTransform_t

  For parameter specifications, see the R_DRW2D_CtxTransform function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxTransform_t)(r_drw2d_Device_t        Device,
                                                           const r_drw2d_FixedP_t *Matrix
                                                           );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxTextureTransform_t

  For parameter specifications, see the R_DRW2D_CtxTextureTransform function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxTextureTransform_t)(r_drw2d_Device_t        Device,
                                                                  const r_drw2d_FixedP_t *Matrix
                                                                  );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxRotate_t

  For parameter specifications, see the R_DRW2D_CtxRotate function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxRotate_t)(r_drw2d_Device_t Device,
                                                        r_drw2d_FixedP_t Angle
                                                        );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxRotate3d_t

  For parameter specifications, see the R_DRW2D_CtxRotate3d function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxRotate3d_t)(r_drw2d_Device_t Device,
                                                          r_drw2d_FixedP_t X,
                                                          r_drw2d_FixedP_t Y,
                                                          r_drw2d_FixedP_t Z,
                                                          r_drw2d_FixedP_t Angle
                                                          );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxTextureRotate_t

  For parameter specifications, see the R_DRW2D_CtxTextureRotate function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxTextureRotate_t)(r_drw2d_Device_t Device,
                                                               r_drw2d_FixedP_t Angle
                                                               );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxScale_t

  For parameter specifications, see the R_DRW2D_CtxScale function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxScale_t)(r_drw2d_Device_t Device,
                                                       r_drw2d_FixedP_t ScaleX,
                                                       r_drw2d_FixedP_t ScaleY,
                                                       r_drw2d_FixedP_t ScaleZ
                                                       );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxTextureScale_t

  For parameter specifications, see the R_DRW2D_CtxTextureScale function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxTextureScale_t)(r_drw2d_Device_t Device,
                                                              r_drw2d_FixedP_t ScaleX,
                                                              r_drw2d_FixedP_t ScaleY
                                                              );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxTranslate_t

  For parameter specifications, see the R_DRW2D_CtxTranslate function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxTranslate_t)(r_drw2d_Device_t Device,
                                                           r_drw2d_FixedP_t TransX,
                                                           r_drw2d_FixedP_t TransY,
                                                           r_drw2d_FixedP_t TransZ
                                                           );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxTextureTranslate_t

  For parameter specifications, see the R_DRW2D_CtxTextureTranslate function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxTextureTranslate_t)(r_drw2d_Device_t Device,
                                                                  r_drw2d_FixedP_t TransX,
                                                                  r_drw2d_FixedP_t TransY
                                                                  );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxFrustum_t

  For parameter specifications, see the R_DRW2D_CtxFrustumfunction in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxFrustum_t)(r_drw2d_Device_t Device,
                                                         r_drw2d_FixedP_t Left,
                                                         r_drw2d_FixedP_t Right,
                                                         r_drw2d_FixedP_t Bottom,
                                                         r_drw2d_FixedP_t Top,
                                                         r_drw2d_FixedP_t ZNear,
                                                         r_drw2d_FixedP_t ZFar
                                                         );



/*******************************************************************************
  Type: r_drw2d_Platform_VtxTransform_t

  For parameter specifications, see the R_DRW2D_VtxTransform function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_VtxTransform_t)(r_drw2d_Device_t  Device,
                                                           r_drw2d_Vec4_t   *Vertices,
                                                           uint32_t          NumVertices
                                                           );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxMatrix_t

  For parameter specifications, see the R_DRW2D_CtxMatrix function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxMatrix_t)(r_drw2d_Device_t        Device,
                                                        r_drw2d_FixedP_t *const VertexMatrix,
                                                        r_drw2d_FixedP_t *const TextureMatrix
                                                        );



/*******************************************************************************
  Type: r_drw2d_Platform_ClutAlloc_t

  For parameter specifications, see the R_DRW2D_ClutAlloc function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_ClutAlloc_t)(r_drw2d_Device_t  Device,
                                                        uint32_t          Size,
                                                        uint32_t         *ClutBase
                                                        );



/*******************************************************************************
  Type: r_drw2d_Platform_ClutFree_t

  For parameter specifications, see the R_DRW2D_ClutFree function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_ClutFree_t)(r_drw2d_Device_t Device,
                                                       uint32_t         Size,
                                                       uint32_t         ClutBase
                                                       );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxClutSet_t

  For parameter specifications, see the R_DRW2D_CtxClutSet function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxClutSet_t)(r_drw2d_Device_t Device,
                                                         uint32_t         ClutBase
                                                         );



/*******************************************************************************
  Type: r_drw2d_Platform_ClutSet_t

  For parameter specifications, see the R_DRW2D_ClutSet function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_ClutSet_t)(r_drw2d_Device_t  Device,
                                                      uint32_t         *Data,
                                                      uint32_t          Start,
                                                      uint32_t          Size
                                                      );



/*******************************************************************************
  Type: r_drw2d_Platform_FramebufferSet_t

  For parameter specifications, see the R_DRW2D_FramebufferSet function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_FramebufferSet_t)(r_drw2d_Device_t       Device,
                                                             r_drw2d_Framebuffer_t *Framebuffer
                                                             );



/*******************************************************************************
  Type: r_drw2d_Platform_FramebufferClear_t

  For parameter specifications, see the R_DRW2D_FramebufferClear function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_FramebufferClear_t)(r_drw2d_Device_t Device);



/*******************************************************************************
  Type: r_drw2d_Platform_DrawTriangles_t

  For parameter specifications, see the R_DRW2D_DrawTriangles function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawTriangles_t)(r_drw2d_Device_t       Device,
                                                            const r_drw2d_Point_t *Points,
                                                            uint32_t               Count,
                                                            const uint8_t         *EdgeFlags
                                                            );



/*******************************************************************************
  Type: r_drw2d_Platform_DrawTrianglesUV_t

  For parameter specifications, see the R_DRW2D_DrawTrianglesUV function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawTrianglesUV_t)(r_drw2d_Device_t         Device,
                                                              const r_drw2d_Point_t   *Points,
                                                              uint32_t                 Count,
                                                              const uint8_t           *EdgeFlags,
                                                              const r_drw2d_UVCoord_t *UVCoords
                                                              );



/*******************************************************************************
  Type: r_drw2d_Platform_DrawRect_t

  For parameter specifications, see the R_DRW2D_DrawRect function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawRect_t)(r_drw2d_Device_t      Device,
                                                       const r_drw2d_Rect_t *Rect
                                                       );



/*******************************************************************************
  Type: r_drw2d_Platform_DrawRectUV_t

  For parameter specifications, see the R_DRW2D_DrawRectUV function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawRectUV_t)(r_drw2d_Device_t         Device,
                                                         const r_drw2d_Rect_t    *Rect,
                                                         const r_drw2d_UVCoord_t *UVCoords
                                                         );



/*******************************************************************************
  Type: r_drw2d_Platform_DrawQuads_t

  For parameter specifications, see the R_DRW2D_DrawQuads function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawQuads_t)(r_drw2d_Device_t       Device,
                                                        const r_drw2d_Point_t *Points,
                                                        uint32_t               Count,
                                                        const uint8_t         *EdgeFlags
                                                        );



/*******************************************************************************
  Type: r_drw2d_Platform_DrawQuadsUV_t

  For parameter specifications, see the R_DRW2D_DrawQuadsUV function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawQuadsUV_t)(r_drw2d_Device_t         Device,
                                                          const r_drw2d_Point_t   *Points,
                                                          uint32_t                 Count,
                                                          uint8_t                 *EdgeFlags,
                                                          const r_drw2d_UVCoord_t *UVCoords
                                                          );



/*******************************************************************************
  Type: r_drw2d_Platform_DrawQuads3dUV_t

  For parameter specifications, see the R_DRW2D_DrawQuads3dUV function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawQuads3dUV_t)(r_drw2d_Device_t         Device,
                                                            const r_drw2d_Vec4_t    *Points,
                                                            uint32_t                 Count,
                                                            uint8_t                 *EdgeFlags,
                                                            const r_drw2d_UVCoord_t *UVCoords
                                                            );



/*******************************************************************************
  Type: r_drw2d_Platform_DrawEllipse_t

  For parameter specifications, see the R_DRW2D_DrawEllipse function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawEllipse_t)(r_drw2d_Device_t Device,
                                                          r_drw2d_Point_t  Point,
                                                          r_drw2d_FixedP_t RadiusX,
                                                          r_drw2d_FixedP_t RadiusY
                                                          );



/*******************************************************************************
  Type: r_drw2d_Platform_DrawLines_t

  For parameter specifications, see the R_DRW2D_DrawLines function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawLines_t)(r_drw2d_Device_t       Device,
                                                        const r_drw2d_Point_t *Points,
                                                        uint32_t               Count
                                                        );



/*******************************************************************************
  Type: r_drw2d_Platform_DrawPolyline_t

  For parameter specifications, see the R_DRW2D_DrawPolyline function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawPolyline_t)(r_drw2d_Device_t       Device,
                                                           const r_drw2d_Point_t *Points,
                                                           uint32_t               Count
                                                           );



/*******************************************************************************
  Type: r_drw2d_Platform_DrawBezierCurves_t

  For parameter specifications, see the R_DRW2D_DrawBezierCurves function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawBezierCurves_t)(r_drw2d_Device_t       Device,
                                                               const r_drw2d_Point_t *Points,
                                                               uint32_t               Count
                                                               );



/*******************************************************************************
  Type: r_drw2d_Platform_DrawRectConvolve1dx_t

  For parameter specifications, see the R_DRW2D_DrawRectConvolve1dx function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawRectConvolve1dx_t)(r_drw2d_Device_t         Device,
                                                                  const r_drw2d_IntRect_t *Rect,
                                                                  uint16_t                 TextureOffX,
                                                                  uint16_t                 TextureOffY
                                                                  );



/*******************************************************************************
  Type: r_drw2d_Platform_DrawRectConvolve1dy_t

  For parameter specifications, see the R_DRW2D_DrawRectConvolve1dy function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawRectConvolve1dy_t)(r_drw2d_Device_t         Device,
                                                                  const r_drw2d_IntRect_t *Rect,
                                                                  uint16_t                 TextureOffX,
                                                                  uint16_t                 TextureOffY
                                                                  );



/*******************************************************************************
  Type: r_drw2d_Platform_DrawRectConvolve2d_t

  For parameter specifications, see the R_DRW2D_DrawRectConvolve2d function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawRectConvolve2d_t)(r_drw2d_Device_t         Device,
                                                                 const r_drw2d_IntRect_t *Rect,
                                                                 uint16_t                 TextureOffX,
                                                                 uint16_t                 TextureOffY
                                                                 );



/*******************************************************************************
  Type: r_drw2d_Platform_DrawRectConvolve_t

  For parameter specifications, see the R_DRW2D_DrawRectConvolve function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_DrawRectConvolve_t)(r_drw2d_Device_t         Device,
                                                               const r_drw2d_IntRect_t *Rect,
                                                               uint16_t                 TextureOffX,
                                                               uint16_t                 TextureOffY
                                                               );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxConvolutionKernelPreset1d_t

  For parameter specifications, see the R_DRW2D_CtxConvolutionKernelPreset1d function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxConvolutionKernelPreset1d_t)(r_drw2d_Device_t                    Device,
                                                                           r_drw2d_ConvolutionKernelPreset1d_t Preset
                                                                           );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxConvolutionKernelPreset2d_t

  For parameter specifications, see the R_DRW2D_CtxConvolutionKernelPreset2d function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxConvolutionKernelPreset2d_t)(r_drw2d_Device_t                    Device,
                                                                           r_drw2d_ConvolutionKernelPreset2d_t Preset
                                                                           );



/*******************************************************************************
  Type: r_drw2d_Platform_GetGaussKernel_t

  For parameter specifications, see the R_DRW2D_GetGaussKernel function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_GetGaussKernel_t)(r_drw2d_Device_t  Device,
                                                             r_drw2d_FixedP_t *Kernel,
                                                             int32_t           Width,
                                                             int32_t           Height,
                                                             r_drw2d_FixedP_t  Sigma
                                                             );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxConvolutionKernel_t

  For parameter specifications, see the R_DRW2D_CtxConvolutionKernel function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxConvolutionKernel_t)(r_drw2d_Device_t      Device,
                                                             const r_drw2d_ConvKernel_t *Kernel
                                                                   );



/*******************************************************************************
  Type: r_drw2d_Platform_CtxConvolutionMode_t

  For parameter specifications, see the R_DRW2D_CtxConvolutionMode function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_CtxConvolutionMode_t)(r_drw2d_Device_t   Device,
                                                                 r_drw2d_ConvMode_t Mode
                                                                 );



/*******************************************************************************
  Type: r_drw2d_Platform_GpuFinish_t

  For parameter specifications, see the R_DRW2D_GpuFinish function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_GpuFinish_t)(r_drw2d_Device_t Device,
                                                        r_drw2d_Finish_t Block
                                                        );



/*******************************************************************************
  Type: r_drw2d_Platform_GpuFinished_t

  For parameter specifications, see the R_DRW2D_GpuFinished function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_GpuFinished_t)(r_drw2d_Device_t   Device,
                                                          r_drw2d_Boolean_t *RetFinished
                                                          );



/*******************************************************************************
  Type: r_drw2d_Platform_GpuCmdListCreate_t

  For parameter specifications, see the R_DRW2D_GpuCmdListCreate function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_GpuCmdListCreate_t)(r_drw2d_Device_t      Device,
                                                               r_drw2d_GpuCmdList_t *RetCmdList
                                                               );



/*******************************************************************************
  Type: r_drw2d_Platform_GpuCmdListGenerate_t

  For parameter specifications, see the R_DRW2D_GpuCmdListGenerate function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_GpuCmdListGenerate_t)(r_drw2d_Device_t              Device,
                                                                 r_drw2d_GpuCmdList_t          CmdList,
                                                                 r_drw2d_GpuCmdListCallback_t  Cbk,
                                                                 void                         *UserData
                                                                 );



/*******************************************************************************
  Type: r_drw2d_Platform_GpuCmdListExec_t

  For parameter specifications, see the R_DRW2D_GpuCmdListExec function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_GpuCmdListExec_t)(r_drw2d_Device_t     Device,
                                                             r_drw2d_GpuCmdList_t CmdList
                                                             );



/*******************************************************************************
  Type: r_drw2d_Platform_GpuCmdListCopy_t

  For parameter specifications, see the R_DRW2D_GpuCmdListCopy function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_GpuCmdListCopy_t)(r_drw2d_Device_t      Device,
                                                             r_drw2d_GpuCmdList_t  CmdList,
                                                             void                 *DestAddr,
                                                             uint32_t             *Size,
                                                             void                 *RelocBaseAddr
                                                             );



/*******************************************************************************
  Type: r_drw2d_Platform_GpuCmdListDelete_t

  For parameter specifications, see the R_DRW2D_GpuCmdListDelete function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_GpuCmdListDelete_t)(r_drw2d_Device_t     Device,
                                                               r_drw2d_GpuCmdList_t CmdList
                                                               );



/*******************************************************************************
  Type: r_drw2d_Platform_PerfCountersAlloc_t

  For parameter specifications, see the R_DRW2D_PerfCountersAlloc function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_PerfCountersAlloc_t)(r_drw2d_Device_t Device);



/*******************************************************************************
  Type: r_drw2d_Platform_PerfCountersFree_t

  For parameter specifications, see the R_DRW2D_PerfCountersFree function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_PerfCountersFree_t)(r_drw2d_Device_t Device);



/*******************************************************************************
  Type: r_drw2d_Platform_PerfValueGet_t

  For parameter specifications, see the R_DRW2D_PerfValueGet function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_PerfValueGet_t)(r_drw2d_Device_t       Device,
                                                           r_drw2d_Performance_t  Type,
                                                           uint32_t              *RetValue
                                                           );



/*******************************************************************************
  Type: r_drw2d_Platform_PerfValueReset_t

  For parameter specifications, see the R_DRW2D_PerfValueReset function in r_drw2d_api.h.
*/
typedef r_drw2d_Error_t (*r_drw2d_Platform_PerfValueReset_t)(r_drw2d_Device_t      Device,
                                                             r_drw2d_Performance_t Type
                                                             );



/*******************************************************************************
  Type: r_drw2d_sys_function_table_t

  Description:
    This structure defined to have a common interface to the processing that depends on the drawing engine used.

    <Drawing Engine> indicates "DHD" or "CPU".


  Fields:
                           Close - Function pointer for R_DRW2D_Platform_Close_<Drawing Engine>.
           NativeDriverHandleGet - Function pointer for R_DRW2D_Platform_NativeDriverHandleGet_<Drawing Engine>.
               NativeDriverBegin - Function pointer for R_DRW2D_Platform_NativeDriverBegin_<Drawing Engine>.
                 NativeDriverEnd - Function pointer for R_DRW2D_Platform_NativeDriverEnd_<Drawing Engine>.
                     ContextInit - Function pointer for R_DRW2D_Platform_ContextInit_<Drawing Engine>.
                   ContextSelect - Function pointer for R_DRW2D_Platform_ContextSelect_<Drawing Engine>.
                      CtxFgColor - Function pointer for R_DRW2D_Platform_CtxFgColor_<Drawing Engine>.
                      CtxBgColor - Function pointer for R_DRW2D_Platform_CtxBgColor_<Drawing Engine>.
                     CtxClipRect - Function pointer for R_DRW2D_Platform_CtxClipRect_<Drawing Engine>.
                     CtxFillMode - Function pointer for R_DRW2D_Platform_CtxFillMode_<Drawing Engine>.
                     CtxCullMode - Function pointer for R_DRW2D_Platform_CtxCullMode_<Drawing Engine>.
                    CtxLineStyle - Function pointer for R_DRW2D_Platform_CtxLineStyle_<Drawing Engine>.
                    CtxBlendMode - Function pointer for R_DRW2D_Platform_CtxBlendMode_<Drawing Engine>.
                 CtxBlendFactors - Function pointer for R_DRW2D_Platform_CtxBlendFactors_<Drawing Engine>.
                   CtxImgQuality - Function pointer for R_DRW2D_Platform_CtxImgQuality_<Drawing Engine>.
                CtxTransformMode - Function pointer for R_DRW2D_Platform_CtxTransformMode_<Drawing Engine>.
         CtxTextureTransformMode - Function pointer for R_DRW2D_Platform_CtxTextureTransformMode_<Drawing Engine>.
                     CtxViewport - Function pointer for R_DRW2D_Platform_CtxViewport_<Drawing Engine>.
               CtxStripingEnable - Function pointer for R_DRW2D_Platform_CtxStripingEnable_<Drawing Engine>.
              CtxStripingDisable - Function pointer for R_DRW2D_Platform_CtxStripingDisable_<Drawing Engine>.
                   CtxEffectsSet - Function pointer for R_DRW2D_Platform_CtxEffectsSet_<Drawing Engine>.
                CtxEffectsUpdate - Function pointer for R_DRW2D_Platform_CtxEffectsUpdate_<Drawing Engine>.
                CtxEffectsDelete - Function pointer for R_DRW2D_Platform_CtxEffectsDelete_<Drawing Engine>.
                   CtxTextureSet - Function pointer for R_DRW2D_Platform_CtxTextureSet_<Drawing Engine>.
                     TextureBlit - Function pointer for R_DRW2D_Platform_TextureBlit_<Drawing Engine>.
        CtxTextureColorKeyEnable - Function pointer for R_DRW2D_Platform_CtxTextureColorKeyEnable_<Drawing Engine>.
       CtxTextureColorKeyDisable - Function pointer for R_DRW2D_Platform_CtxTextureColorKeyDisable_<Drawing Engine>.
                     CtxIdentity - Function pointer for R_DRW2D_Platform_CtxIdentity_<Drawing Engine>.
              CtxTextureIdentity - Function pointer for R_DRW2D_Platform_CtxTextureIdentity_<Drawing Engine>.
                    CtxTransform - Function pointer for R_DRW2D_Platform_CtxTransform_<Drawing Engine>.
             CtxTextureTransform - Function pointer for R_DRW2D_Platform_CtxTextureTransform_<Drawing Engine>.
                       CtxRotate - Function pointer for R_DRW2D_Platform_CtxRotate_<Drawing Engine>.
                     CtxRotate3d - Function pointer for R_DRW2D_Platform_CtxRotate3d_<Drawing Engine>.
                CtxTextureRotate - Function pointer for R_DRW2D_Platform_CtxTextureRotate_<Drawing Engine>.
                        CtxScale - Function pointer for R_DRW2D_Platform_CtxScale_<Drawing Engine>.
                 CtxTextureScale - Function pointer for R_DRW2D_Platform_CtxTextureScale_<Drawing Engine>.
                    CtxTranslate - Function pointer for R_DRW2D_Platform_CtxTextureTranslate_<Drawing Engine>.
             CtxTextureTranslate - Function pointer for R_DRW2D_Platform_CtxTextureTranslate_<Drawing Engine>.
                      CtxFrustum - Function pointer for R_DRW2D_Platform_CtxFrustum_<Drawing Engine>.
                    VtxTransform - Function pointer for R_DRW2D_Platform_VtxTransform_<Drawing Engine>.
                       CtxMatrix - Function pointer for R_DRW2D_Platform_CtxMatrix_<Drawing Engine>.
                       ClutAlloc - Function pointer for R_DRW2D_Platform_ClutAlloc_<Drawing Engine>.
                        ClutFree - Function pointer for R_DRW2D_Platform_ClutFree_<Drawing Engine>.
                      CtxClutSet - Function pointer for R_DRW2D_Platform_CtxClutSet_<Drawing Engine>.
                         ClutSet - Function pointer for R_DRW2D_Platform_ClutSet_<Drawing Engine>.
                  FramebufferSet - Function pointer for R_DRW2D_Platform_FramebufferSet_<Drawing Engine>.
                FramebufferClear - Function pointer for R_DRW2D_Platform_FramebufferClear_<Drawing Engine>.
                   DrawTriangles - Function pointer for R_DRW2D_Platform_DrawTriangles_<Drawing Engine>.
                 DrawTrianglesUV - Function pointer for R_DRW2D_Platform_DrawTrianglesUV_<Drawing Engine>.
                        DrawRect - Function pointer for R_DRW2D_Platform_DrawRect_<Drawing Engine>.
                      DrawRectUV - Function pointer for R_DRW2D_Platform_DrawRectUV_<Drawing Engine>.
                       DrawQuads - Function pointer for R_DRW2D_Platform_DrawQuads_<Drawing Engine>.
                     DrawQuadsUV - Function pointer for R_DRW2D_Platform_DrawQuadsUV_<Drawing Engine>.
                   DrawQuads3dUV - Function pointer for R_DRW2D_Platform_DrawQuads3dUV_<Drawing Engine>.
                     DrawEllipse - Function pointer for R_DRW2D_Platform_DrawEllipse_<Drawing Engine>.
                       DrawLines - Function pointer for R_DRW2D_Platform_DrawLines_<Drawing Engine>.
                    DrawPolyline - Function pointer for R_DRW2D_Platform_DrawPolyline_<Drawing Engine>.
                DrawBezierCurves - Function pointer for R_DRW2D_Platform_DrawBezierCurves_<Drawing Engine>.
             DrawRectConvolve1dx - Function pointer for R_DRW2D_Platform_DrawRectConvolve1dx_<Drawing Engine>.
             DrawRectConvolve1dy - Function pointer for R_DRW2D_Platform_DrawRectConvolve1dy_<Drawing Engine>.
              DrawRectConvolve2d - Function pointer for R_DRW2D_Platform_DrawRectConvolve2d_<Drawing Engine>.
                DrawRectConvolve - Function pointer for R_DRW2D_Platform_DrawRectConvolve_<Drawing Engine>.
    CtxConvolutionKernelPreset1d - Function pointer for R_DRW2D_Platform_CtxConvolutionKernelPreset1d_<Drawing Engine>.
    CtxConvolutionKernelPreset2d - Function pointer for R_DRW2D_Platform_CtxConvolutionKernelPreset2d_<Drawing Engine>.
                  GetGaussKernel - Function pointer for R_DRW2D_Platform_GetGaussKernel_<Drawing Engine>.
            CtxConvolutionKernel - Function pointer for R_DRW2D_Platform_CtxConvolutionKernel_<Drawing Engine>.
              CtxConvolutionMode - Function pointer for R_DRW2D_Platform_CtxConvolutionMode_<Drawing Engine>.
                       GpuFinish - Function pointer for R_DRW2D_Platform_GpuFinish_<Drawing Engine>.
                     GpuFinished - Function pointer for R_DRW2D_Platform_GpuFinished_<Drawing Engine>.
                GpuCmdListCreate - Function pointer for R_DRW2D_Platform_GpuCmdListCreate_<Drawing Engine>.
              GpuCmdListGenerate - Function pointer for R_DRW2D_Platform_GpuCmdListGenerate_<Drawing Engine>.
                  GpuCmdListExec - Function pointer for R_DRW2D_Platform_GpuCmdListExec_<Drawing Engine>.
                  GpuCmdListCopy - Function pointer for R_DRW2D_Platform_GpuCmdListCopy_<Drawing Engine>.
                GpuCmdListDelete - Function pointer for R_DRW2D_Platform_GpuCmdListDelete_<Drawing Engine>.
               PerfCountersAlloc - Function pointer for R_DRW2D_Platform_PerfCountersAlloc_<Drawing Engine>.
                PerfCountersFree - Function pointer for R_DRW2D_Platform_PerfCountersFree_<Drawing Engine>.
                    PerfValueGet - Function pointer for R_DRW2D_Platform_PerfValueGet_<Drawing Engine>.
                  PerfValueReset - Function pointer for R_DRW2D_Platform_PerfValueReset_<Drawing Engine>.

*/
typedef struct
{
    r_drw2d_Platform_Close_t                            Close;
    r_drw2d_Platform_NativeDriverHandleGet_t            NativeDriverHandleGet;
    r_drw2d_Platform_NativeDriverBegin_t                NativeDriverBegin;
    r_drw2d_Platform_NativeDriverEnd_t                  NativeDriverEnd;
    r_drw2d_Platform_ContextInit_t                      ContextInit;
    r_drw2d_Platform_ContextSelect_t                    ContextSelect;
    r_drw2d_Platform_CtxFgColor_t                       CtxFgColor;
    r_drw2d_Platform_CtxBgColor_t                       CtxBgColor;
    r_drw2d_Platform_CtxClipRect_t                      CtxClipRect;
    r_drw2d_Platform_CtxFillMode_t                      CtxFillMode;
    r_drw2d_Platform_CtxCullMode_t                      CtxCullMode;
    r_drw2d_Platform_CtxLineStyle_t                     CtxLineStyle;
    r_drw2d_Platform_CtxBlendMode_t                     CtxBlendMode;
    r_drw2d_Platform_CtxBlendFactors_t                  CtxBlendFactors;
    r_drw2d_Platform_CtxImgQuality_t                    CtxImgQuality;
    r_drw2d_Platform_CtxTransformMode_t                 CtxTransformMode;
    r_drw2d_Platform_CtxTextureTransformMode_t          CtxTextureTransformMode;
    r_drw2d_Platform_CtxViewport_t                      CtxViewport;
    r_drw2d_Platform_CtxStripingEnable_t                CtxStripingEnable;
    r_drw2d_Platform_CtxStripingDisable_t               CtxStripingDisable;
    r_drw2d_Platform_CtxEffectsSet_t                    CtxEffectsSet;
    r_drw2d_Platform_CtxEffectsUpdate_t                 CtxEffectsUpdate;
    r_drw2d_Platform_CtxEffectsDelete_t                 CtxEffectsDelete;
    r_drw2d_Platform_CtxTextureSet_t                    CtxTextureSet;
    r_drw2d_Platform_TextureBlit_t                      TextureBlit;
    r_drw2d_Platform_CtxTextureColorKeyEnable_t         CtxTextureColorKeyEnable;
    r_drw2d_Platform_CtxTextureColorKeyDisable_t        CtxTextureColorKeyDisable;
    r_drw2d_Platform_CtxIdentity_t                      CtxIdentity;
    r_drw2d_Platform_CtxTextureIdentity_t               CtxTextureIdentity;
    r_drw2d_Platform_CtxTransform_t                     CtxTransform;
    r_drw2d_Platform_CtxTextureTransform_t              CtxTextureTransform;
    r_drw2d_Platform_CtxRotate_t                        CtxRotate;
    r_drw2d_Platform_CtxRotate3d_t                      CtxRotate3d;
    r_drw2d_Platform_CtxTextureRotate_t                 CtxTextureRotate;
    r_drw2d_Platform_CtxScale_t                         CtxScale;
    r_drw2d_Platform_CtxTextureScale_t                  CtxTextureScale;
    r_drw2d_Platform_CtxTranslate_t                     CtxTranslate;
    r_drw2d_Platform_CtxTextureTranslate_t              CtxTextureTranslate;
    r_drw2d_Platform_CtxFrustum_t                       CtxFrustum;
    r_drw2d_Platform_VtxTransform_t                     VtxTransform;
    r_drw2d_Platform_CtxMatrix_t                        CtxMatrix;
    r_drw2d_Platform_ClutAlloc_t                        ClutAlloc;
    r_drw2d_Platform_ClutFree_t                         ClutFree;
    r_drw2d_Platform_CtxClutSet_t                       CtxClutSet;
    r_drw2d_Platform_ClutSet_t                          ClutSet;
    r_drw2d_Platform_FramebufferSet_t                   FramebufferSet;
    r_drw2d_Platform_FramebufferClear_t                 FramebufferClear;
    r_drw2d_Platform_DrawTriangles_t                    DrawTriangles;
    r_drw2d_Platform_DrawTrianglesUV_t                  DrawTrianglesUV;
    r_drw2d_Platform_DrawRect_t                         DrawRect;
    r_drw2d_Platform_DrawRectUV_t                       DrawRectUV;
    r_drw2d_Platform_DrawQuads_t                        DrawQuads;
    r_drw2d_Platform_DrawQuadsUV_t                      DrawQuadsUV;
    r_drw2d_Platform_DrawQuads3dUV_t                    DrawQuads3dUV;
    r_drw2d_Platform_DrawEllipse_t                      DrawEllipse;
    r_drw2d_Platform_DrawLines_t                        DrawLines;
    r_drw2d_Platform_DrawPolyline_t                     DrawPolyline;
    r_drw2d_Platform_DrawBezierCurves_t                 DrawBezierCurves;
    r_drw2d_Platform_DrawRectConvolve1dx_t              DrawRectConvolve1dx;
    r_drw2d_Platform_DrawRectConvolve1dy_t              DrawRectConvolve1dy;
    r_drw2d_Platform_DrawRectConvolve2d_t               DrawRectConvolve2d;
    r_drw2d_Platform_DrawRectConvolve_t                 DrawRectConvolve;
    r_drw2d_Platform_CtxConvolutionKernelPreset1d_t     CtxConvolutionKernelPreset1d;
    r_drw2d_Platform_CtxConvolutionKernelPreset2d_t     CtxConvolutionKernelPreset2d;
    r_drw2d_Platform_GetGaussKernel_t                   GetGaussKernel;
    r_drw2d_Platform_CtxConvolutionKernel_t             CtxConvolutionKernel;
    r_drw2d_Platform_CtxConvolutionMode_t               CtxConvolutionMode;
    r_drw2d_Platform_GpuFinish_t                        GpuFinish;
    r_drw2d_Platform_GpuFinished_t                      GpuFinished;
    r_drw2d_Platform_GpuCmdListCreate_t                 GpuCmdListCreate;
    r_drw2d_Platform_GpuCmdListGenerate_t               GpuCmdListGenerate;
    r_drw2d_Platform_GpuCmdListExec_t                   GpuCmdListExec;
    r_drw2d_Platform_GpuCmdListCopy_t                   GpuCmdListCopy;
    r_drw2d_Platform_GpuCmdListDelete_t                 GpuCmdListDelete;
    r_drw2d_Platform_PerfCountersAlloc_t                PerfCountersAlloc;
    r_drw2d_Platform_PerfCountersFree_t                 PerfCountersFree;
    r_drw2d_Platform_PerfValueGet_t                     PerfValueGet;
    r_drw2d_Platform_PerfValueReset_t                   PerfValueReset;
} r_drw2d_sys_FunctionTable_t;



/***************************************************************************
  Section: Global Variables
*/
extern const r_drw2d_sys_FunctionTable_t * const g_pfunction_table_dhd;
extern const r_drw2d_sys_FunctionTable_t * const g_pfunction_table_cpu;



#ifdef __cplusplus
}
#endif

#endif /* R_DRW2D_FUNC_TABLE_H_ */


