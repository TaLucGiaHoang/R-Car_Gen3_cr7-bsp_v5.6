/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/
/*--------------------------------------------------------------------------*/
/* Project: D/AVE*/
/* File:    demo.h (%version: 17 %)*/
/*          created Tue Feb 15 18:22:30 2005 by hh04027*/
/**/
/* Description:*/
/*  %date_modified: Tue Mar 20 10:33:53 2007 %  (%derived_by:  hh74026 %)*/
/**/
/* Changes:*/
/* 2005-12-22 CSe  adapted to variable screen size*/
/* 2006-03-14 MPa  added main menu and framework-struct*/
/* 2007-10-16 ASc  added new buttoncodes to be used for the 3C25 touchpad*/
/*--------------------------------------------------------------------------*/

#ifndef __1_demo_h_H
#define __1_demo_h_H
/*---------------------------------------------------------------------------*/
#define DHD_TEX_RLD_AVAILABLE 1

typedef struct {
    struct r_drw2d_DeviceDHD_s sys;
    r_drw2d_Device_t           drw2d;
    void                       *dhd;
} davehd_dev_t;

/*---------------------------------------------------------------------------*/
enum demo_buttoncodes
{
    demo_b1 = 1,           /* userdefined*/
    demo_b2 = 2,           /* userdefined*/
    demo_b3 = 4,           /* userdefined*/
    demo_b4 = 64,
    demo_hold_b1 = 8,      /* userdefined*/
    demo_hold_b2 = 16,     /* userdefined*/
    demo_hold_b3 = 32,     /* userdefined*/
    demo_hold_b4 = 128,    /* show menu*/
   
    demo_b5  = 256,        /* go back*/
    demo_b6  = 512         /* go forward*/
};


/*---------------------------------------------------------------------------*/
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#define _I(x) ((x)<<4)
#define _F(x) ((int)((x) * 16.0f))

extern unsigned int demo_screen_width;
extern unsigned int demo_screen_height;
extern unsigned int demo_screen_mode;

#define SCREENX demo_screen_width
#define SCREENY demo_screen_height
#define SCREENM demo_screen_mode


#define SCREENSIZE ((SCREENX)*(SCREENY))
#define FRAMEBUFFERSIZE ((SCREENX)*(SCREENY))

#define TRIGTAB_SIZE 4096
#define TRIG_ONE     1024

#ifdef WIN32
#pragma warning (disable:4761)
#endif

extern short SinTab[TRIGTAB_SIZE];
extern short CosTab[TRIGTAB_SIZE];


/*---------------------------------------------------------------------------*/

typedef int (*fp_demo_init)(davehd_dev_t *Dev );
typedef int (*fp_demo_render)(davehd_dev_t  *Dev, int first, int state );
typedef void (*fp_demo_shutdown)(davehd_dev_t *Dev );

typedef struct _demo_info
{
    struct _demo_info *next;
    int id;
    fp_demo_init init_function;
    fp_demo_render render_function;
    fp_demo_shutdown shutdown_function;
    char *name;
} demo_info;

struct frameworkInfo
{
  int showMenu;
  int clearFrameBuffer;
  int showFps;
  int frameSync;
  char fpsInfoString[80];
};

struct mainMenuState
{
  int x, y;
  int width, itemHeight;
  int nofItems, titleHeight;
  int menuselect, demoselect;
  int *automaticMode;
  int showFps;
  int frameSync;
};

/*---------------------------------------------------------------------------*/

#define DEMOID(x,y) \
static int demo_init(davehd_dev_t *Dev); \
static int demo_render(davehd_dev_t *Dev, int first, int state); \
static void demo_shutdown(davehd_dev_t *Dev); \
demo_info effect##x = { NULL, x, demo_init, demo_render, demo_shutdown, y }

/*---------------------------------------------------------------------------*/

extern unsigned int demo_random();
extern short GetSin(short sAngle);
extern short GetCos(short sAngle);

/*---------------------------------------------------------------------------*/

extern void drawString(davehd_dev_t *Dev, const char * s, int x, int y);

extern int initdemo(davehd_dev_t *Dev);

extern void renderframe(davehd_dev_t *Dev, int buttons, int frametime);

extern void shutdowndemo(davehd_dev_t *Dev);

/*---------------------------------------------------------------------------*/


/*extern void menuDemoGetAlphaInfo(struct AlphaInfo *ai);*/
extern struct frameworkInfo g_frameworkInfo;
extern struct mainMenuState g_mainMenuState;
/*---------------------------------------------------------------------------*/

extern void* demo_allocvidmem(davehd_dev_t *Dev, unsigned int size);
extern void  demo_freevidmem(davehd_dev_t *Dev, void* ptr);
extern void  demo_copytovidmem(davehd_dev_t *Dev, void* dst, void* src, unsigned int size);
extern void* demo_getvidmemptr(davehd_dev_t *Dev, void* vidbuf);
extern void  demo_getflashedtexture(davehd_dev_t *Dev, int index, void **ptr, unsigned int *size, int *width, int *height);

/*---------------------------------------------------------------------------*/

void demo_set_fb(r_drw2d_Framebuffer_t *fb);
r_drw2d_Framebuffer_t *demo_get_fb(void);
#include "img_format.h"
r_drw2d_Framebuffer_t demo_img2fb(davehd_dev_t *Dev, const Img_t *Img);
r_drw2d_Texture_t demo_img2tex(davehd_dev_t *Dev, const Img_t *Img, r_drw2d_TextureFlags_t Flags);
r_drw2d_Rect_t demo_int2rect(int PosX,int PoxY,int Width,int Height);
r_drw2d_Texture_t demo_fb2tex(davehd_dev_t *Dev, r_drw2d_Framebuffer_t *Fb, r_drw2d_TextureFlags_t Flags);

#endif
