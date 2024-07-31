/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "davehd_kernel_driver.h"
#include "davehd_kernel_bridge.h"
#include "davehd_driver.h"

#include "r_print_api.h"

#include "r_config_drw2d.h"
#include "r_drw2d_types.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_drw2d_ctx_dhd.h"
#include "demo.h"
#include "app_cluster.h"
#include "img_format_convert.h"


/* array of buffers that can be allocated by demos*/
#define _VIDMEMBUFFERS 50
static dhd_vidmem_t vidmembuffer[_VIDMEMBUFFERS];
static int vidmembuffer_alloced[_VIDMEMBUFFERS] = { 0 };

static r_drw2d_Framebuffer_t demo_cur_fb;

void demo_set_fb(r_drw2d_Framebuffer_t *fb)
{
    demo_cur_fb = *fb;
}

r_drw2d_Framebuffer_t *demo_get_fb(void)
{
    return &demo_cur_fb;
}

r_drw2d_Framebuffer_t demo_img2fb(davehd_dev_t *Dev, const Img_t *Img)
{
    r_drw2d_Framebuffer_t tmp_fb;
    void *pImageData;

    if (Img->Attributes & IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY) {
        pImageData = *Img->Data;
    } else {
        pImageData = demo_getvidmemptr(Dev, *Img->Data);
    }

    tmp_fb.Flags = 0;
    tmp_fb.Handle = 0;
    tmp_fb.Buffer.Pitch = Img->Width;
    tmp_fb.Buffer.Size.Width = Img->Width;
    tmp_fb.Buffer.Size.Height = Img->Height;
    tmp_fb.Buffer.PixelFormat = GetDrw2dFormat(Img->Colourformat);
    tmp_fb.Buffer.Data = pImageData;
    return tmp_fb;
}

r_drw2d_Texture_t demo_img2tex(davehd_dev_t *Dev, const Img_t *Img, r_drw2d_TextureFlags_t Flags)
{
    void *pImageData = 0;
    r_drw2d_Texture_t tmp_tex;

    if (Img->Attributes & IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY) {
        pImageData = *Img->Data;
    } else {
        pImageData = demo_getvidmemptr(Dev, *Img->Data);
    }
    tmp_tex.Flags = Flags;
    tmp_tex.Handle = 0;
    tmp_tex.Buffer.Data = pImageData;
    tmp_tex.Buffer.PixelFormat = GetDrw2dFormat(Img->Colourformat);
    tmp_tex.Buffer.Size.Height = Img->Height;
    tmp_tex.Buffer.Size.Width = Img->Width;
    tmp_tex.Buffer.Pitch = Img->Width;
    return tmp_tex;
}

r_drw2d_Texture_t demo_fb2tex(davehd_dev_t *Dev, r_drw2d_Framebuffer_t *Fb, r_drw2d_TextureFlags_t Flags)
{
    r_drw2d_Texture_t tmp_tex;

    tmp_tex.Flags = Flags;
    tmp_tex.Handle = 0;
    tmp_tex.Buffer = Fb->Buffer;
    return tmp_tex;
}

r_drw2d_Rect_t demo_int2rect(int PosX, int PosY, int Width, int Height)
{
    r_drw2d_Rect_t rect;

    rect.Pos.X = R_DRW2D_2X(PosX);
    rect.Pos.Y = R_DRW2D_2X(PosY);
    rect.Size.Width = R_DRW2D_2X(Width);
    rect.Size.Height = R_DRW2D_2X(Height);
    return rect;
}

int demo_init(davehd_dev_t *Dev)
{

    InitAppCluster(Dev);
    return 1;
}

int demo_render(davehd_dev_t *Dev, int first, int state)
{
    RenderAppCluster(Dev);

    return 1;
}

void demo_shutdown(davehd_dev_t *Dev)
{
/*     // cleanup here */
/*  d2_cliprect(handle, 1, 1,  DISPLAY_WIDTH-2, DISPLAY_HEIGHT-2); */
/*  d2_setblur(handle, _F(1.0)); */

    DeInitAppCluster(Dev);

}

void * demo_allocvidmem(davehd_dev_t *Dev, size_t size)
{
    int i, index = 0;

    for (i = 0; i < _VIDMEMBUFFERS; i++) {
        if (vidmembuffer_alloced[i] == 0) {
            index = i;
            vidmembuffer_alloced[i] = 1;
            break;
        }
    }
    if (i >= _VIDMEMBUFFERS) {
        return 0;
    }


    if ((dhd_alloc_vidmem(Dev->dhd, size, 0, E_DHD_MEMORY_TEXTURE, &vidmembuffer[index]) != E_DHD_OK)) {
        R_PRINT_Log("ERROR: failed to allocate vidmem\n");
        return 0;
    }
    return &vidmembuffer[index];
}

void demo_freevidmem(davehd_dev_t *Dev, void * ptr)
{
    int i;

    for (i = 0; i < _VIDMEMBUFFERS; i++) {
        if (ptr ==  &vidmembuffer[i]) {
            dhd_free_vidmem(Dev->dhd, ptr);
            vidmembuffer_alloced[i] = 0;
            break;
        }
    }
}

void demo_copytovidmem(davehd_dev_t *Dev, void * dst, void * src, size_t size)
{
    dhd_vidmem_t * ptr = (dhd_vidmem_t *)dst;

    dhd_upload_vidmem(Dev->dhd, src, ptr->m_gpu_address, size);
}

void * demo_getvidmemptr(davehd_dev_t *Dev, void * vidbuf)
{
    dhd_vidmem_t * ptr = (dhd_vidmem_t *)vidbuf;

    if (ptr == 0 || ptr->m_gpu_address == 0)
        while (1) {
        }
    return ptr->m_gpu_address;
}
