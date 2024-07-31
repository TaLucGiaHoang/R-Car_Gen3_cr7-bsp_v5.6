/*
****************************************************************************
PROJECT : Drw2D driver
FILE    : $Id: r_drw2d_ctx_cpu.h 11006 2016-11-07 16:32:44Z michael.golczewski $
============================================================================
DESCRIPTION
Driver for DRW2D
============================================================================
                            C O P Y R I G H T
============================================================================
                           Copyright (c) 2013 - 2022
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

#ifndef R_DRW2D_CTX_CPU_H_
#define R_DRW2D_CTX_CPU_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined (__ghs__)
#pragma ghs startnomisra
#endif /* __ghs__ */

#if defined (__ghs__)
#pragma ghs endnomisra
#endif /* __ghs__ */


/*******************************************************************************
  Title: DRW2D device context initialization

  Defines <r_drw2d_DeviceCPU_s> struct.

  An application using DRW2D must include this header file.
*/


/*******************************************************************************
  Define: DRW2D_CPU_VERTEX_BUFFER_SIZE

  Maximum number of vertices in the vertex buffer.

  Note that 8 is the maximum of vertices, that will occur when clipping a 
  convex quad against another quad (the clipping window).
*/
#define DRW2D_CPU_VERTEX_BUFFER_SIZE 8


/*******************************************************************************
  Define: DRW2D_CPU_POLY_VERTEX_COUNT

  Maximum number of vertices of a single polygon.
*/
#define DRW2D_CPU_POLY_VERTEX_COUNT  4



/*******************************************************************************
  Section: Types and enumerations
*/


/*******************************************************************************
  Type: IntRect_t
  
  Description:
    Describes position and dimension of an rectangle using integer values.

  Fields:
    X         - X position of the rectangle.
    Y         - Y position of the rectangle.
    Width     - Width of the rectangle.
    Height    - Height of the rectangle.
*/
typedef struct IntRect_s
{
    int32_t X;
    int32_t Y;
    int32_t Width;
    int32_t Height;
} IntRect_t;


/*******************************************************************************
  Type: vertex_t
  
  Description:
    Describes a vertex with additional texture coordinates.

  Fields:
    X   - X position of the vertex.
    Y   - Y position of the vertex.
    U   - U coordinate of the texture.
    V   - V coordinate of the texture.
*/
typedef struct vertex_s
{
    int32_t X;
    int32_t Y;
    float32_t U;
    float32_t V;
} vertex_t;


/*******************************************************************************
  Type: edge_t
  
  Description:
    Describes an edge of a polygon for the scan line rendering routines.
    Besides the edge's start and end point, the structure also contains
    information needed for edge scanning and texture mapping.

  Fields:
    SkipFirst   - If 1, the first scan line of the edge is skipped by the 
                  rendering routine.
    StartIdx    - Index of the start vertex in the vertex buffer (see 
                  <r_drw2d_DeviceCPU_t>).
    EndIdx      - Index of the end vertex in the vertex buffer (see
                  <r_drw2d_DeviceCPU_t>).
    Start       - Start vertex of the edge.
    End         - End vertex of the edge.
    my          - Slope of the edge, relating to the Y axis.
    mu          - Slope of the texture coordinate u, relating to the X axis.
    mv          - Slope of the texture coordinate v, relating to the X axis.
*/
typedef struct edge_s
{
    int32_t SkipFirst;
    int32_t StartIdx;
    int32_t EndIdx;
    vertex_t *Start;
    vertex_t *End;
    float32_t my;
    float32_t mu;
    float32_t mv;
} edge_t;


/*******************************************************************************
  Enum: RLEState_t
  
  Description:
    Specifiers for the states of the RLE decoder.

  Fields:
    RLE_CONTROL_BYTE        - A control byte is being read.
    RLE_RUN_LENGTH_PACKET   - A runlength packet is being extracted.
    RLE_RAW_PACKET          - A raw packet is being extracted.
*/
typedef enum {
    RLE_CONTROL_BYTE,
    RLE_RUN_LENGTH_PACKET,
    RLE_RAW_PACKET
} RLEState_t;

struct r_drw2d_DeviceCPU_s;


/*******************************************************************************
  Enum: r_drw2d_sys_cpu_PixelProcessing_t
  
  Description:
    Specifiers for determining how pixels are processed in the RenderBlit and 
    RenderScanline functions.

  Fields:
    R_DRW2D_SYS_CPU_PIXELPROCESSING_NONE        - No further processing.
    R_DRW2D_SYS_CPU_PIXELPROCESSING_CA          - Use Constant Alpha.
    R_DRW2D_SYS_CPU_PIXELPROCESSING_CA_PPA      - Use Constant Alpha, blending with Per-Pixel-Alpha.
    R_DRW2D_SYS_CPU_PIXELPROCESSING_MOD         - Use Modulate effect.
    R_DRW2D_SYS_CPU_PIXELPROCESSING_MOD_PPA     - Use modulate effect, blending with Per-Pixel-Alpha.
    R_DRW2D_SYS_CPU_PIXELPROCESSING_PPA         - Blend with Per-Pixel-Alpha.
*/
typedef enum
{
    R_DRW2D_SYS_CPU_PIXELPROCESSING_NONE        = 0,
    R_DRW2D_SYS_CPU_PIXELPROCESSING_CA          = 1,
    R_DRW2D_SYS_CPU_PIXELPROCESSING_CA_PPA      = 2,
    R_DRW2D_SYS_CPU_PIXELPROCESSING_MOD         = 3,
    R_DRW2D_SYS_CPU_PIXELPROCESSING_MOD_PPA     = 4,
    R_DRW2D_SYS_CPU_PIXELPROCESSING_PPA         = 5,

    R_DRW2D_SYS_CPU_NUM_PIXELPROCESSING,
    R_DRW2D_SYS_CPU_PIXELPROCESSING_NOT_SUPPORTED,
} r_drw2d_sys_cpu_PixelProcessing_t;

/*******************************************************************************
  Type: RenderScanLines
  
  Description:
    Function pointer to render routines.

    This type is being used to build an array of rendering functions that can
    be accessed by use of the needed rendering parameters (e.g. input format,
    output format, ...).
*/
typedef void (*RenderScanLines)(r_drw2d_DeviceBase_t *DeviceBase, int32_t StartY, int32_t EndY, r_drw2d_sys_cpu_PixelProcessing_t Processing);


/*******************************************************************************
  Type: r_drw2d_DeviceCPU_t
  
  Description:
    The Drw2D CPU context. The context holds some buffers for a polygon's
    vertices, as well as working variables for the rendering implementations.
    The context contains the whole state of the Drw2D CPU implementation.

  Fields:
    Base - Device base structure (see <r_drw2d_DeviceBase_t>)
    Vertices - Internal vertex buffer for the currently processed polygon.
    VertexCount - Number of vertices in the internal vertex buffer (see Vertices)
    Edges_l - Internal edge buffer for the scan line rendering functions.
    Edges_r - Internal edge buffer for the scan line rendering functions.
    EdgeCount_l - Number of edges in the edge buffer (see Edges_l).
    EdgeCount_r - Number of edges in the edge buffer (see Edges_r).
    
    The following fields are working variables of the scan line renderer:
    SRScanLine - Currently processed scan line.
    SREdgeIdxL - Index of the currently processed left edge in the left edge
                 buffer.
    SREdgeIdxR - Index of the currently processed right edge in the right edge
                 buffer.
    SREdge_x_L - Current X value at the left edge.
    SREdge_u_L - Current U value at the left edge.
    SREdge_v_L - Current V value at the left edge.
    SREdge_x_R - Current X value at the right edge.
    SREdge_u_R - Current U value at the right edge.
    SREdge_v_R - Current V value at the right edge.

    The following fields are variables for the simple blit renderer:
    BlitSrc - Source rectangle in the source texture. Note: The structure
              contains a 2D vertex. Height and Width are actually specifying
              the lower right vertex of the rectangle.
    BlitDst - Destination rectangle in the frame buffer. Note: The structure
              contains a 2D vertex. Height and Width are actually specifying
              the lower right vertex of the rectangle.

    The following fields are variables of the renderer:
    GfxModColor - The modulate color, set in the Drw2D effect stage.
    GfxConstAlpha - The constant alpha, set in the Drw2D effect stage.
*/
struct r_drw2d_DeviceCPU_s
{
    r_drw2d_DeviceBase_t Base;

    vertex_t Vertices[DRW2D_CPU_VERTEX_BUFFER_SIZE];
    int32_t VertexCount;

    edge_t Edges_l[DRW2D_CPU_VERTEX_BUFFER_SIZE - 1];
    edge_t Edges_r[DRW2D_CPU_VERTEX_BUFFER_SIZE - 1];
    int32_t EdgeCount_l;
    int32_t EdgeCount_r;

    /* Working variables of the scanline renderer */
    int32_t SRScanLine;
    int32_t SREdgeIdxL;
    int32_t SREdgeIdxR;
    float32_t SREdge_x_L;
    float32_t SREdge_u_L;
    float32_t SREdge_v_L;
    float32_t SREdge_x_R;
    float32_t SREdge_u_R;
    float32_t SREdge_v_R;

    /* Blit coordinates for fast render path.
       Note: The structures contain two 2D points. Width and height are
       actually the lower right vertex of the rectangle.
    */
    IntRect_t BlitSrc;
    IntRect_t BlitDst;

    uint32_t GfxModColor;
    uint32_t GfxConstAlpha;
};

typedef struct r_drw2d_DeviceCPU_s r_drw2d_DeviceCPU_t;


#endif /* R_DRW2D_CTX_CPU_H_ */
