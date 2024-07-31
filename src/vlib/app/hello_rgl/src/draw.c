/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "hello_rgl.h"
#include "hello_rgl_intern.h"
#include "vout.h"
#include "draw.h"

#include "r_print_api.h"
#include "r_prr_api.h"

#undef TESTCASE_SURF_ALPHA

/*
   Update following definetion to enable R_PRINT_Log() in /vlib/drivers/print/src/r_print_api.c
   #define R_USE_LOG 0
*/

int DrawQuit = 0;

static const uint32_t colors[] = {
    0x000000FF, // Blue
    0x0000FF00, // Green
    0x00FF0000, // Red
    0x0000FFFF, // Cyan
    0x00FFFF00, // Yellow
    0x00FF00FF, // Magenta
};


static const osal_thread_id_t hellorgl_thread_id[MAX_THREAD] = {
    LOC_OSAL_THREAD_DRW2D_DRWTHREAD00,
    LOC_OSAL_THREAD_DRW2D_DRWTHREAD01,
    LOC_OSAL_THREAD_DRW2D_DRWTHREAD02,
    LOC_OSAL_THREAD_DRW2D_DRWTHREAD03,
    LOC_OSAL_THREAD_DRW2D_DRWTHREAD04,
    LOC_OSAL_THREAD_DRW2D_DRWTHREAD05
};
static osal_thread_handle_t hellorgl_thread_handle[MAX_THREAD] = {
    OSAL_THREAD_HANDLE_INVALID,
    OSAL_THREAD_HANDLE_INVALID,
    OSAL_THREAD_HANDLE_INVALID,
    OSAL_THREAD_HANDLE_INVALID,
    OSAL_THREAD_HANDLE_INVALID,
    OSAL_THREAD_HANDLE_INVALID
};
static int32_t hellorgl_thread_used[MAX_THREAD] = {
    0, 0, 0,
    0, 0, 0
};

static int32_t hellorgl_thread_count = 0;


#define IMAGE_SOLID     0x00000000  /* color=opaque red, set to 0 for ramp */
#define IMAGE_WIDTH     256
#define IMAGE_HEIGHT    64

static uint32_t                     *sg_image;
static osal_memory_buffer_handle_t  sg_imageMemHandle;

static void create_tex(void)
{
    uint32_t r, c;
    e_osal_return_t osal_ret;
    osal_memory_manager_handle_t memHandle;
    uint32_t *buff;
    size_t size;

    memHandle = VoutGetMemhandle();

    size = (IMAGE_HEIGHT * IMAGE_WIDTH) * sizeof(uint32_t);

    osal_ret = R_OSAL_MmngrAlloc( memHandle, size, ALLOC_ALIGN, &sg_imageMemHandle);
    if(OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("create_tex: R_OSAL_MmngrAlloc failer, ret = %d \n", osal_ret);
        return;
    }

    osal_ret = R_OSAL_MmngrGetCpuPtr( sg_imageMemHandle, (void**)&sg_image );
    if(OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("create_tex: R_OSAL_MmngrGetCpuPtr failer, ret = %d \n", osal_ret);
        return;
    }

    for (r = 0; r < IMAGE_HEIGHT; ++r) {
        buff = (uint32_t*)(sg_image + (r*IMAGE_WIDTH));
        for (c = 0; c < IMAGE_WIDTH; ++c) {
            if (0 != IMAGE_SOLID) {
                *buff++ = 0xFFFF0000;
            } else {
                float h = (float)c / IMAGE_WIDTH;
                int i = (int)(h * 6);
                float f = h * 6.0 - i;
                float q = 1 - f;
                uint32_t color = 0xFF000000;

                switch (i % 6) {
                case 0:
                    color = 0xFFFF0000 | ((uint32_t)(f * 255) << 8);
                    break;
                case 1:
                    color = 0xFF00FF00 | ((uint32_t)(q * 255) << 16);
                    break;
                case 2:
                    color = 0xFF00FF00 | ((uint32_t)(f * 255) << 0);
                    break;
                case 3:
                    color = 0xFF0000FF | ((uint32_t)(q * 255) << 8);
                    break;
                case 4:
                    color = 0xFF0000FF | ((uint32_t)(f * 255) << 16);
                    break;
                case 5:
                    color = 0xFFFF0000 | ((uint32_t)(q * 255) << 0);
                    break;
                }

                *buff++ = color;
            }
        }
    }


    /* flush our generated data so the DHD will be able to access */
    R_OSAL_MmngrFlush(sg_imageMemHandle, 0, size);
}

#define VIMAGE_SOLID     0x00000000  /* color=opaque red, set to 0 for ramp */
#define VIMAGE_WIDTH     64
#define VIMAGE_HEIGHT    256

static uint32_t                     *sg_vimage;
static osal_memory_buffer_handle_t  sg_vimageMemHandle;

static void create_vtex(void)
{
    uint32_t r, c;
    e_osal_return_t osal_ret;
    osal_memory_manager_handle_t memHandle;
    uint32_t *buff;
    size_t size;

    memHandle = VoutGetMemhandle();

    size = (VIMAGE_HEIGHT * VIMAGE_WIDTH) * sizeof(uint32_t);

    osal_ret = R_OSAL_MmngrAlloc( memHandle, size, ALLOC_ALIGN, &sg_vimageMemHandle);
    if(OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("create_vtex: R_OSAL_MmngrAlloc failer, ret = %d \n", osal_ret);
        return;
    }

    osal_ret = R_OSAL_MmngrGetCpuPtr( sg_vimageMemHandle, (void**)&sg_vimage );
    if(OSAL_RETURN_OK != osal_ret) {
        R_PRINT_Log("create_vtex: R_OSAL_MmngrGetCpuPtr failer, ret = %d \n", osal_ret);
        return;
    }

    for (r = 0; r < VIMAGE_HEIGHT; ++r) {
        buff = (uint32_t*)(sg_vimage + (r*VIMAGE_WIDTH));
        for (c = 0; c < VIMAGE_WIDTH; ++c) {
            *buff++ = 0xFFFFFFFF;
        }
    }
}

/* generate a delta to move distance between 5 and 25s */
static float delta_generator(uint32_t distance)
{
    return (distance) / (float)((rand() % 20 * 60) + (5 * 60));
}


void *DrawThreadFunc(void *Parameter)
{
    layer_t *pLayer = (layer_t *)Parameter;
    int x;
    uint32_t color;
    uint8_t VSyncMsg[4];
    r_drw2d_Texture_t vtex;
    r_drw2d_Rect_t vtex_rect;
    r_drw2d_Rect_t rect;
    r_drw2d_Texture_t tex;
    r_drw2d_Rect_t tex_rect;
    r_drw2d_Rect_t image_rect;
    r_drw2d_UVCoord_t uvs_def[4] = { { 0,               0               },
                                     { R_DRW2D_2X(256), 0               },
                                     { R_DRW2D_2X(256), R_DRW2D_2X(256) },
                                     { 0,               R_DRW2D_2X(256) } };

    float xpos = 0.;
    float ypos = 0.;
    float alpha = -1.;
    float delta_x, delta_y;
    float pos_x = 0., pos_y = 0.;
    uint8_t fb_index = 0;

    st_osal_mq_config_t mq_cfg;
    e_osal_return_t     osal_ret = OSAL_RETURN_OK;
    int shared_vspd_layer_offset=0;
    int corrected_z=0;
    r_prr_DevName_t dev    = R_PRR_GetDevice();

    // FIXME, workaround for VOUT2 / VOUT3 layers not mapped properly but start with z=3
    if (((R_PRR_RCARH3  == dev) && (pLayer->Parent->Disp == R_WM_DEV_VOUT3)) ||
	((R_PRR_RCARM3N == dev)	&& (pLayer->Parent->Disp == R_WM_DEV_VOUT2))
	) {
	shared_vspd_layer_offset=3;
    }


    // portTASK_USES_FLOATING_POINT();

    R_PRINT_Log("Thread started (display:%d, layer:%ld)\n", pLayer->Parent->Disp, pLayer->VoutSurface.PosZ);

    /* Must initialise rand seed for each task */
    srand((pLayer->Parent->Disp * 256) + pLayer->VoutSurface.PosZ);

    // FIXME for H3 and for M3N VGA uses a shared approach (VSP0 for LVDS & VGA)
    // Layer z value is the RPF number, but as VGA has reserved RPF3 & RPF4 we have to
    // add 3 to the z-value here (should be done in WM or in driver, but for testing
    // it is done here.
    corrected_z = pLayer->VoutSurface.PosZ;
    pLayer->VoutSurface.PosZ = corrected_z + shared_vspd_layer_offset;


    /* surface MUST be intialized in draw thread otherwise WM complains about future FB updates due to PID */
    x = VoutSurfaceInit(pLayer);
    if (x != 0) {
        R_PRINT_Log("Surface failed (display:%d, layer:%ld)\n", pLayer->Parent->Disp, pLayer->VoutSurface.PosZ);
        goto end;
    }

    // FIXME for H3 and for M3N -> 
    x = (pLayer->Parent->Disp * MAX_NR_LAYER) + corrected_z;

    osal_ret = R_OSAL_MqInitializeMqConfigSt(&mq_cfg);
    if(OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[DrawThreadFunc] R_OSAL_MqInitializeMqConfigSt[%d]: Err=%d\n", x, osal_ret);
        goto end;
    }
    mq_cfg.max_num_msg = 1;
    mq_cfg.msg_size    = 4;

    osal_ret = R_OSAL_MqCreate(&mq_cfg, (osal_mutex_id_t)(LOC_OSAL_MQ_SYNC_BASE+x), &pLayer->VSyncMq);
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[DrawThreadFunc] R_OSAL_MqCreate[%d]: Err=%d\n", x, osal_ret);
        goto end;
    }

    x = R_WM_DevEventRegister(pLayer->Parent->Disp, R_WM_EVENT_VBLANK, 0, (void *)&pLayer->VSyncMq);
    if (x != R_WM_ERR_SUCCESS)
    {
        R_PRINT_Log("[DrawThreadFunc] R_WM_DevEventRegister: Err=0x%08x\n", x);
        goto end;
    }

    tex.Handle = R_NULL;
    tex.Flags = (r_drw2d_TextureFlags_t)0;
    tex.Buffer.Size.Height = IMAGE_HEIGHT;
    tex.Buffer.Size.Width = IMAGE_WIDTH;
    tex.Buffer.Pitch = IMAGE_WIDTH;
    tex.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    tex.Buffer.Data = (void *)sg_image;

    tex_rect.Pos.X = R_DRW2D_2X(0);
    tex_rect.Pos.Y = R_DRW2D_2X(0);
    tex_rect.Size.Width = R_DRW2D_2X(IMAGE_WIDTH);
    tex_rect.Size.Height = R_DRW2D_2X(IMAGE_HEIGHT);

    image_rect.Pos.X = R_DRW2D_2X(0);
    image_rect.Pos.Y = R_DRW2D_2X(ypos);
    image_rect.Size.Width = R_DRW2D_2X(pLayer->VoutSurface.Width);
    image_rect.Size.Height = R_DRW2D_2X(pLayer->VoutSurface.Height * (corrected_z + 1) / 40);

    delta_x = delta_generator(pLayer->Parent->ScreenWidth - pLayer->VoutSurface.Width);
    delta_y = delta_generator(pLayer->Parent->ScreenHeight - pLayer->VoutSurface.Height);

    vtex.Handle = R_NULL;
    vtex.Flags = (r_drw2d_TextureFlags_t)0;
    vtex.Buffer.Size.Height = VIMAGE_HEIGHT;
    vtex.Buffer.Size.Width = VIMAGE_WIDTH;
    vtex.Buffer.Pitch = VIMAGE_WIDTH;
    vtex.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
    vtex.Buffer.Data = (void *)sg_vimage;

    vtex_rect.Pos.X = R_DRW2D_2X(0);
    vtex_rect.Pos.Y = R_DRW2D_2X(0);
    vtex_rect.Size.Width = R_DRW2D_2X(VIMAGE_WIDTH);
    vtex_rect.Size.Height = R_DRW2D_2X(VIMAGE_HEIGHT);

    rect.Pos.X = R_DRW2D_2X(xpos);
    rect.Pos.Y = R_DRW2D_2X(0);
    rect.Size.Width = R_DRW2D_2X(pLayer->VoutSurface.Width * (corrected_z + 1) / 40);
    rect.Size.Height = R_DRW2D_2X(pLayer->VoutSurface.Height);
    /* mix the display/layer colors up */
    color = colors[(pLayer->Parent->Disp + corrected_z) % (sizeof(colors) / sizeof(colors[0]))];

    osal_ret = R_OSAL_MqReceiveForTimePeriod((pLayer->VSyncMq), 1000u, &VSyncMsg, sizeof(VSyncMsg) );
    if (OSAL_RETURN_OK != osal_ret)
    {
        R_PRINT_Log("[DrawThreadFunc] R_OSAL_MqReceiveForTimePeriod[%d]: Err=%d\n", x, osal_ret);
    }

    R_PRINT_Log("Drawing start (display:%d, layer:%ld)\n", pLayer->Parent->Disp, corrected_z);

    while (!DrawQuit)
    {
        color &= 0x00FFFFFF;    // Constant Color

        {
            /* cycle the alpha up/down based on layer
               Layer 0:12s, 1:6s, 2:4s, 3:3s, 4:2.4s */
            alpha += 2. / (60. * 12. / ((float)corrected_z + 1.));
            if (alpha >= 1.0) {
                alpha = -1.0;
            }
            color |= ((int32_t)roundf(fabsf(alpha * 255.))) << 24;   // Variable Alpha
        }

        {
            /* move the surface on the display */
            pos_x += delta_x;
            if (((uint32_t)pos_x + pLayer->VoutSurface.Width) >=
                (pLayer->Parent->ScreenWidth)) {
                pos_x = (float)(pLayer->Parent->ScreenWidth - pLayer->VoutSurface.Width);
                delta_x = -delta_generator(pLayer->Parent->ScreenWidth - pLayer->VoutSurface.Width);
            }
            if (0.0 >= pos_x) {
                pos_x = 0.0;
                delta_x = delta_generator(pLayer->Parent->ScreenWidth - pLayer->VoutSurface.Width);
            }
            pLayer->VoutSurface.PosX = (uint32_t)pos_x;

            pos_y += delta_y;
            if (((uint32_t)pos_y + pLayer->VoutSurface.Height) >=
                (pLayer->Parent->ScreenHeight)) {
                pos_y = (float)(pLayer->Parent->ScreenHeight - pLayer->VoutSurface.Height);
                delta_y = -delta_generator(pLayer->Parent->ScreenHeight - pLayer->VoutSurface.Height);
            }
            if (0.0 >= pos_y) {
                pos_y = 0.0;
                delta_y = delta_generator(pLayer->Parent->ScreenHeight - pLayer->VoutSurface.Height);
            }
            pLayer->VoutSurface.PosY = (uint32_t)pos_y;
        }

        {
            /* select our buffer we will draw our surface on for this cycle */
            pLayer->Drw2DFb.Buffer.Data = (void *)pLayer->FBuf[fb_index];
            /* Update fb_index after executing VoutSetFB().
            fb_index++;
            fb_index %= (sizeof(pLayer->FBuf) / sizeof(pLayer->FBuf[0]));
            */

            x = R_DRW2D_FramebufferSet(pLayer->Drw2DDev, &pLayer->Drw2DFb);
            if (R_DRW2D_ERR_OK != x)
            {
                R_PRINT_Log("[DRW2D] Error: R_DRW2D_FramebufferSet(%d)\n",x);
                x = R_DRW2D_ERR_OK;
            }
        }

        {
            /* paint the surface with our current background*/
            x |= R_DRW2D_CtxBgColor(pLayer->Drw2DDev, color);
            if (R_DRW2D_ERR_OK != x)
            {
                R_PRINT_Log("[DRW2D] Error: R_DRW2D_CtxBgColor(%d)\n",x);
                x = R_DRW2D_ERR_OK;
            }
            x |= R_DRW2D_FramebufferClear(pLayer->Drw2DDev);
            if (R_DRW2D_ERR_OK != x)
            {
                R_PRINT_Log("[DRW2D] Error: R_DRW2D_FramebufferClear(%d)\n",x);
                x = R_DRW2D_ERR_OK;
            }
        }

#ifdef DRW2D_ON_DHD
        {
            /* draw a sliding vertical box the height of the surface */
            x |= R_DRW2D_CtxFillMode(pLayer->Drw2DDev, R_DRW2D_FILLMODE_SOLID);
            if (R_DRW2D_ERR_OK != x)
            {
                R_PRINT_Log("[DRW2D] Error: R_DRW2D_CtxFillMode(%d)\n",x);
                x = R_DRW2D_ERR_OK;
            }
            x |= R_DRW2D_CtxFgColor(pLayer->Drw2DDev, 0xffffffff);
            if (R_DRW2D_ERR_OK != x)
            {
                R_PRINT_Log("[DRW2D] Error: R_DRW2D_CtxFgColor(%d)\n",x);
                x = R_DRW2D_ERR_OK;
            }
            x |= R_DRW2D_DrawRect(pLayer->Drw2DDev, &rect);
            if (R_DRW2D_ERR_OK != x)
            {
                R_PRINT_Log("[DRW2D] Error: R_DRW2D_DrawRect(%d)\n",x);
                x = R_DRW2D_ERR_OK;
            }
#elif DRW2D_ON_CPU
        {
            /* draw a sliding vertical box the height of the surface */
            x |= R_DRW2D_CtxTextureSet(pLayer->Drw2DDev, 0, &vtex);
            if (R_DRW2D_ERR_OK != x)
            {
                R_PRINT_Log("[DRW2D] Error: R_DRW2D_CtxFillMode(%d)\n",x);
                x = R_DRW2D_ERR_OK;
            }
            x |= R_DRW2D_TextureBlit(pLayer->Drw2DDev, &vtex_rect, &rect);
            if (R_DRW2D_ERR_OK != x)
            {
                R_PRINT_Log("[DRW2D] Error: R_DRW2D_TextureBlit(%d)\n",x);
                x = R_DRW2D_ERR_OK;
            }
#endif
            /* slide  the rectangle at rate proportional to layer so timing can be checked
               Layer 0:16s, 1:8s, 2:4s, 3:2s, 4:1s */
            xpos += ((float)pLayer->VoutSurface.Width / (60. * 16. / ((float)(1 << corrected_z))));
            if (xpos >= (float)pLayer->VoutSurface.Width) {
                xpos = 0.;
            }
            rect.Pos.X = R_DRW2D_2X(xpos);
        }

        {
            /* copy a texture to the surface */
            x |= R_DRW2D_CtxTextureSet(pLayer->Drw2DDev, 0, &tex);
            if (R_DRW2D_ERR_OK != x)
            {
                R_PRINT_Log("[DRW2D] Error: R_DRW2D_CtxTextureSet(%d)\n",x);
                x = R_DRW2D_ERR_OK;
            }
#ifdef DRW2D_ON_DHD
            x |= R_DRW2D_CtxFillMode(pLayer->Drw2DDev, R_DRW2D_FILLMODE_TEXTURE);
            if (R_DRW2D_ERR_OK != x)
            {
                R_PRINT_Log("[DRW2D] Error: R_DRW2D_CtxFillMode(%d)\n",x);
                x = R_DRW2D_ERR_OK;
            }
            x |= R_DRW2D_CtxBlendMode(pLayer->Drw2DDev, R_DRW2D_BLENDMODE_CUSTOM);
            if (R_DRW2D_ERR_OK != x)
            {
                R_PRINT_Log("[DRW2D] Error: R_DRW2D_CtxBlendMode(%d)\n",x);
                x = R_DRW2D_ERR_OK;
            }
            x |= R_DRW2D_CtxBlendFactors(pLayer->Drw2DDev, R_DRW2D_BLEND_SRC_ALPHA,
                                         R_DRW2D_BLEND_ONE_MINUS_SRC_ALPHA,
                                         R_DRW2D_BLEND_ONE,
                                         R_DRW2D_BLEND_ONE_MINUS_SRC_ALPHA);
            if (R_DRW2D_ERR_OK != x)
            {
                R_PRINT_Log("[DRW2D] Error: R_DRW2D_CtxBlendFactors(%d)\n",x);
                x = R_DRW2D_ERR_OK;
            }
            x |= R_DRW2D_DrawRectUV(pLayer->Drw2DDev, &image_rect, uvs_def);
            if (R_DRW2D_ERR_OK != x)
            {
                R_PRINT_Log("[DRW2D] Error: R_DRW2D_DrawRectUV(%d)\n",x);
                x = R_DRW2D_ERR_OK;
            }
#elif DRW2D_ON_CPU
            x |= R_DRW2D_TextureBlit(pLayer->Drw2DDev, &tex_rect, &image_rect);
            if (R_DRW2D_ERR_OK != x)
            {
                R_PRINT_Log("[DRW2D] Error: R_DRW2D_TextureBlit(%d)\n",x);
                x = R_DRW2D_ERR_OK;
            }
#endif
            /* slide  the rectangle at rate proportional to layer so timing can be checked
               Layer 0:16s, 1:8s, 2:4s, 3:2s, 4:1s */

            ypos += ((float)pLayer->VoutSurface.Height / (60. * 16. / ((float)(1 << corrected_z))));
            if (ypos >= (float)pLayer->VoutSurface.Height) {
                ypos = 0.;
            }
            image_rect.Pos.Y = R_DRW2D_2X(ypos);
        }

#ifdef TESTCASE_SURF_ALPHA //Usecase to test R_WM_SurfacePropertySet
    r_wm_Surface_t *surf;
    if (corrected_z == 0)
    {
        pLayer->VoutSurface.Alpha = 0x40;
        surf = &pLayer->VoutSurface;
        x = R_WM_SurfacePropertySet(pLayer->Parent->Disp, R_WM_SURF_ALPHA_SET, surf);
    }

    if (corrected_z == 1)
    {
        pLayer->VoutSurface.Alpha = 0xFF;
        surf = &pLayer->VoutSurface;
        x = R_WM_SurfacePropertySet(pLayer->Parent->Disp, R_WM_SURF_ALPHA_SET, surf);
    }
#endif

        /* execute the display list */
        x |= R_DRW2D_GpuFinish(pLayer->Drw2DDev, R_DRW2D_FINISH_WAIT);
        if (R_DRW2D_ERR_OK != x)
        {
            R_PRINT_Log("[DRW2D] Error: R_DRW2D_GpuFinish(%d)\n",x);
            x = R_DRW2D_ERR_OK;
        }

        osal_ret = R_OSAL_MqReceiveForTimePeriod((pLayer->VSyncMq), 1000u, &VSyncMsg, sizeof(VSyncMsg) );
        if (OSAL_RETURN_OK != osal_ret) {
            R_PRINT_Log("[DrawThreadFunc] R_OSAL_MqReceiveForTimePeriod[%d]: Err=%d\n", x, osal_ret);
        }

        /* flush our surface changes to the VSPD */
        x |= VoutSetFB(pLayer, fb_index);

        /* Update fb_index */
        fb_index++;
        fb_index %= (sizeof(pLayer->FBuf) / sizeof(pLayer->FBuf[0]));
    }
end:
    return 0;
}

static uint32_t DrawInitOnce = 0;
int DrawInit(display_t *pDisplay)
{
    e_osal_return_t         osal_ret     = OSAL_RETURN_OK;
    st_osal_thread_config_t thread_config;

    int j, x = 0;
    char taskname[] = "DrawThread0:0";

    if (0 == DrawInitOnce) {
        DrawInitOnce = 1;
        DrawQuit = 0;
        x = R_DRW2D_Init();
        if (R_DRW2D_ERR_OK != x)
        {
            R_PRINT_Log("[DRW2D] Error: R_DRW2D_Init(%d)\n",x);
            x = R_DRW2D_ERR_OK;
        }
        create_tex();
        create_vtex();
    }
    else {
        DrawInitOnce++;
    }

    taskname[10] = '0' + (char)pDisplay->Disp;
    for (j = 0; j < (sizeof(pDisplay->Layer) / sizeof(pDisplay->Layer[0])); j++) {
//    { j=0;
        layer_t *pLayer = &pDisplay->Layer[j];
        pLayer->Drw2DFb.Flags = 0;
        pLayer->Drw2DFb.Handle = 0;
        pLayer->Drw2DFb.Buffer.Pitch = pLayer->VoutSurface.Width;
        pLayer->Drw2DFb.Buffer.Size.Width = pLayer->VoutSurface.Width;
        pLayer->Drw2DFb.Buffer.Size.Height = pLayer->VoutSurface.Height;
        // We're now assigning the drw2d pixel format in VoutInit
        //pLayer->Drw2DFb.Buffer.PixelFormat = R_DRW2D_PIXELFORMAT_ARGB8888;
        pLayer->Drw2DFb.Buffer.Data = (void *)pLayer->FBuf[0];

        x |= R_DRW2D_Open(0, 0, &pLayer->Drw2DGpuDev, &pLayer->Drw2DDev);
        R_PRINT_Log("R_DRW2D_Open[%d], ret= %d\n", j, x );
        taskname[12] = '0' + j;
        
        if( hellorgl_thread_count < MAX_THREAD )
        {
            thread_config.priority   = LOC_DRAW_PRIO;
            thread_config.task_name  = taskname;
            thread_config.func       = (p_osal_thread_func_t)DrawThreadFunc;
            thread_config.userarg    = (void*)pLayer;
            thread_config.stack_size = LOC_DRAW_STACKSIZE;

            osal_ret = R_OSAL_ThreadCreate( &thread_config,
                                      (osal_thread_id_t)hellorgl_thread_id[hellorgl_thread_count],
                                      &(hellorgl_thread_handle[hellorgl_thread_count]) );
            hellorgl_thread_used[hellorgl_thread_count] = 1;
            R_PRINT_Log("DrawInit: Create Thread[%s], ret = %d\n", thread_config.task_name, osal_ret);
            pLayer->DrawThread = hellorgl_thread_handle[hellorgl_thread_count];
            hellorgl_thread_count++;
        }
    }
    return x;
}

int DrawDeinit(display_t *pDisplay)
{
    int j;
    r_drw2d_Error_t x;

    for (j = 0; j < (sizeof(pDisplay->Layer) / sizeof(pDisplay->Layer[0])); j++) {
        layer_t *pLayer = &pDisplay->Layer[j];

        x = R_DRW2D_Close(pLayer->Drw2DDev);
        if (R_DRW2D_ERR_OK != x)
        {
            R_PRINT_Log("[DRW2D] Error: R_DRW2D_Close(%d)\n", j, x);
            x = R_DRW2D_ERR_OK;
        }
    }
    if (0 == --DrawInitOnce) {
        x = R_DRW2D_Exit();
        if (R_DRW2D_ERR_OK != x)
        {
            R_PRINT_Log("[DRW2D] Error: R_DRW2D_Exit(%d)\n", j, x);
            x = R_DRW2D_ERR_OK;
        }
    }
    return x;
}

int DrawLoop(void)
{

    while (!DrawQuit) {
        (void)R_OSAL_ThreadSleepForTimePeriod((osal_milli_sec_t)5);
    }
    return 0;
}


