/*
****************************************************************************
PROJECT : Drw2D driver
FILE    : $Id: r_drw2d_priv.h 11006 2016-11-07 16:32:44Z michael.golczewski $
============================================================================
DESCRIPTION
Driver for DRW2D
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2016 - 2022
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

/***********************************************************
  Title: DRW2D private functions (internal)
  
  DRW2D Main Module Internal Interface

  Only the driver modules shall include this header.
*/

#ifndef R_DRW2D_PRIV_H_
#define R_DRW2D_PRIV_H_

#ifdef __cplusplus
extern "C" {
#endif



/***********************************************************
  Section: Global Defines
*/

/***********************************************************
  Section: Global Types
*/



/***********************************************************
  Type: r_drw2d_UnitApi_t

  Private, per-unit data (API-side).

  Fields:
             Mutex - Used to synchronize access to this unit

  See also:
    <r_drw2d_Units> array variable.
*/
typedef struct r_drw2d_UnitApi_s
{
    r_drw2d_OS_Mutex_t       Mutex;
} r_drw2d_UnitApi_t;





/***********************************************************
  Section: Global Variables
*/


extern r_drw2d_UnitApi_t r_drw2d_UnitApis[R_DRW2D_NUM_UNITS];


/***********************************************************
  Section: Local Functions
*/

/***********************************************************
  Function: drw2d_ErrorHandler

  Handle DRW2D error by calling device context specific error
   handler (if set), or by spinning in an infinite loop.
*/

void drw2d_ErrorHandler (int32_t                  Unit,
                            r_drw2d_Error_t       DrwError,
                            r_drw2d_DeviceBase_t *DeviceContextHint
                           );



/***********************************************************
  Function: drw2d_IsValidPixelFormat_CPU

  Check whether the given pixel format is valid.

  (note) The pixel format may still be unsupported by the actual 
         graphics driver backend

  Parameters:
  PixelFormat - Pixel storage format

  Returns:
  see: <r_drw2d_Error_t>

*/
r_drw2d_Error_t drw2d_IsValidPixelFormat_CPU (r_drw2d_PixelFormat_t PixelFormat);



/***********************************************************
  Function: drw2d_IsValidBufferSize_CPU

  Check whether the given texture or framebuffer size is supported

  Parameters:
  Size - Buffer size

  Returns:
  see: <r_drw2d_Error_t>

*/
r_drw2d_Error_t drw2d_IsValidBufferSize_CPU (const r_drw2d_IntSize_t Size);



/***********************************************************
  Function: drw2d_UpdateEffectiveClipRect_CPU

  Clip user cliprect against framebuffer size and 
  store in DeviceBase->State.EffectiveClipRect.

  Parameters:
  DeviceBase - Address of device context base class

  Returns:
  none

*/
void drw2d_UpdateEffectiveClipRect_CPU (r_drw2d_DeviceBase_t *DeviceBase);



/***********************************************************
  Function: drw2d_CheckUpdateEffectiveClipRect

  Check <R_DRW2D_API_DIRTY_EFFCLIPRECT> flag and update cliprect if necessary.

  Parameters:
  DeviceBase  - Address of device context base class

  Returns:
  none

*/
R_INLINE void drw2d_CheckUpdateEffectiveClipRect (r_drw2d_DeviceBase_t *DeviceBase);

R_INLINE void drw2d_CheckUpdateEffectiveClipRect (r_drw2d_DeviceBase_t *DeviceBase) {
   if (0 != (DeviceBase->State.APIDirtyFlags & R_DRW2D_API_DIRTY_EFFCLIPRECT))
   {
      drw2d_UpdateEffectiveClipRect_CPU(DeviceBase);
      DeviceBase->State.APIDirtyFlags &= (uint16_t) ~R_DRW2D_API_DIRTY_EFFCLIPRECT;
   }
}



/***********************************************************
  Function: drw2d_VtxTransform_CPU

  Transform 4D vector(s) by 4x4 vertex matrix and project 
  onto screen if 3D transformation is enabled.

  Parameters:
   DeviceBase - Address of device context base class
          Vec - Pointer to vertices
  NumVertices - Number of vertices provided by "Vertices" parameter

  Returns:
  none

*/
void drw2d_VtxTransform_CPU (const r_drw2d_DeviceBase_t *DeviceBase,
                             r_drw2d_Vec4_t             *Vec,
                             uint32_t                    NumVertices
                             );



/***********************************************************
  Function: drw2d_GetTextureEnabled_CPU

  Get a enabled texture.

  Parameters:
  RenderCtx - Structure type for the render context

  Returns:
  none

*/
void drw2d_GetTextureEnabled_CPU (uint32_t *Status, r_drw2d_RenderContext_t *RenderCtx);

/***********************************************************
  Function: drw2d_GetTextureCount_CPU

  Get the count of textures.

  Parameters:
  RenderCtx - Structure type for the render context

  Returns:
  Count of textures

*/
uint32_t drw2d_GetTextureCount_CPU (r_drw2d_RenderContext_t *RenderCtx);



/***********************************************************
  Function: R_DRW2D_Platform_BlitTexture_CPU

  Blit a texture.

  Parameters:
  DeviceBase - Address of device context base class
    Vertices - Pointer to vertices
     SrcRect - Source rectangle

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_BlitTexture_CPU (r_drw2d_DeviceBase_t *DeviceBase,
                                                  const r_drw2d_Vec4_t *Vertices,
                                                  const r_drw2d_Rect_t *SrcRect
                                                  );



/***********************************************************
  Function: R_DRW2D_Platform_UpdateRenderstates_CPU

  Update render states according to dirty flags
  (see <r_drw2d_DeviceBase_t>:CtxDirtyFlags)
  and required flags.

  Parameters:
  DeviceBase - Address of device context base class
    ReqFlags - The Sys-side dirty flags

  Returns:
  See <r_drw2d_Error_t>.

*/
r_drw2d_Error_t R_DRW2D_Platform_UpdateRenderstates_CPU (r_drw2d_DeviceBase_t *DeviceBase,
                                                         uint32_t              ReqFlags
                                                         );



#ifdef __cplusplus
}
#endif

#endif /* R_DRW2D_PRIV_H_  */
