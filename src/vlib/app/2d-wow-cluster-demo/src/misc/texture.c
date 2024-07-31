/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

/***********************************************************
  Title: Texture Module

  The module provides functions to setup textures in the GPU 
  memory or to release the memory used by a texture.
*/

/***********************************************************
  Section: Includes
*/

#include <stdint.h>

#include "davehd_driver.h"
#include "davehd_util.h"

#include "r_config_drw2d.h"
#include "r_drw2d_types.h"
#include "r_drw2d_os.h"
#include "r_drw2d_api.h"
#include "r_drw2d_ctx_dhd.h"

#include "img_format.h"
#include "error.h"
#include "romfs_api.h"


#include "demo.h"
#include "texture.h"

/***********************************************************
  Section: Local Functions
*/

static void locVMemWrite8(
    davehd_dev_t *dev,
    uint32_t     Address,
    uint8_t      Value)
{
    *((uint8_t*)Address) = Value;
}

static void locVMemWrite16(
    davehd_dev_t *dev,
    uint32_t     Address,
    uint16_t     Value)
{
    *((uint16_t*)Address) = Value;
}

static  void locVMemWrite32(
    davehd_dev_t *dev,
    uint32_t     Address,
    uint32_t     Value)
{
    *((uint32_t*)Address) = Value;
}

/* aus V:\vlib\app\dx4\gfx_appnote\src\platform\ab_v850e2_dr4_3d\platform_ab_v850e2_dr4_3d.c!!! */
static void loc_D2DWritePixel(
    davehd_dev_t *dev,
    uint32_t Dst,
    uint8_t *Src,
    uint8_t Bpp) 
{
    uint8_t  tmp;
    uint32_t value;

    switch (Bpp) {
        case 4:
            value = 0;
            for (tmp = 0; tmp < Bpp; tmp++) {
                value |= ((*Src) << (tmp << 3));
                Src++;
            }
            locVMemWrite32(dev, Dst, value);
            break;
        case 2:
            value = 0;
            for (tmp = 0; tmp < Bpp; tmp++) {
                value |= ((*Src) << (tmp << 3));
                Src++;
            }
            locVMemWrite16(dev, Dst, value);
            break;
        default:
            for (tmp = Bpp; tmp > 0; tmp--) {
                value = *Src;
                Src++;
                locVMemWrite8(dev, Dst, value);
                Dst++;
            }
            break;
    }
}


/***********************************************************
  Function: locDecompressRLE

  Unpack RLE data to destination address.

  Parameters:
  Dev        - D2D device structure
  Dst        - video memory destination address
  Src        - Pointer to source data
  Size       - Size of compressed data
  Bpp        - Number of bytes per pixel

  Returns:
  void
*/

static void locDecompressRLE( davehd_dev_t *dev, uint32_t Dst, uint8_t *Src, uint32_t Size, uint8_t Bpp) {
    uint8_t  pixelcounter;
    uint8_t  nextpixelrle;
    uint8_t  tmp;
    uint8_t* rlestart;

    while (Size > 0) {
        tmp = *Src;
        pixelcounter = (tmp & 0x7F) + 1;
        nextpixelrle = (tmp >> 7);
        Src++;
        Size -= 1;
        if (Size == 0) {
            break;
        }
        rlestart = Src;
        while (pixelcounter > 0) {
            if (nextpixelrle != 0) {
                /* if rle package repeat same pixel */
                Src = rlestart;
            }
            loc_D2DWritePixel(dev, Dst, Src, Bpp);
            Dst += Bpp;
            Src += Bpp;
            pixelcounter -= 1;
            if ((nextpixelrle == 0)
                || ((nextpixelrle != 0) && (pixelcounter == 1))) {
                if (Size >= Bpp) {
                    Size -= Bpp;
                } else {
                    Size = 0;
                }
            }
        }
    }    
}


/***********************************************************
  Section: Global Functions

  Description see: <texture.h>
*/

/***********************************************************
  Function: Init2DImg
*/

void *Init2DImg( davehd_dev_t *Dev, Img_t *Img) {
    void     *data;
    void     *old;
    uint32_t  size;
    uint8_t   bpp;
    eeRomFs_File_t *fp;
    uint32_t  mm_data_addr = 0;

    if (!Img) {
        return 0;
    }
    
    if (Img->Attributes & IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY) {
        /* don't copy image to video memory,
            directly use it from persistent memory (Flash) */        
        if (*Img->Data == 0) {  /* check, if ROMFS is to be used */
            /* use ROMFS */
            fp = eeRomFs_FOpen(Img->Name, "rb"); 
            if (!fp) { 
                return 0; 
            }
            mm_data_addr = (uint32_t)eeRomFs_MMap(0, 0, 0, 0, fp, 0);
            if (!mm_data_addr) {
                return 0;
            }          
            eeRomFs_MUnmap((void*)mm_data_addr, 0);
            eeRomFs_FClose(fp);
            /* simply return image data address (read from ROMFS) */
            old          = *Img->Data;
            *Img->Data   = (void*)mm_data_addr;
        } else {
            /* simply return image data address */
            old          = *Img->Data;
        }
    } else {
        /* copy image data to video memory */
    
       /* check colourformat and calc bpp */
       switch (Img->Colourformat) {
           case IMG_ARGB8888:
           case (IMG_ARGB8888 | IMG_MODE_RLE):
               bpp = 4;
               break;
           
           case IMG_RGB888:
               /* RGB888 is a 4 byte aligned */
               bpp = 4;
               break;

           case (IMG_RGB888 | IMG_MODE_RLE):
               /* RGB888 is a 3 byte format w/o 4byte alignment when RLE compressed */
               bpp = 3;
               break;
               
           case IMG_ARGB1555:
           case (IMG_ARGB1555 | IMG_MODE_RLE):
           case IMG_RGB565:
           case (IMG_RGB565 | IMG_MODE_RLE):
               bpp = 2;
               break;
               
           case IMG_ALPHA8:
           case (IMG_ALPHA8 | IMG_MODE_RLE):
               bpp = 1;
               break;
               
           default:
               ClusterError(ERR_TEXTURE_SETUP_FAILED);
               return *Img->Data;
       }
       
       /* calc image size (decompressed) */
       size = (Img->Height * Img->Width * bpp);
          
       data = demo_allocvidmem(Dev, size);
              
       if (!data)  {
           ClusterError(ERR_TEXTURE_SETUP_FAILED);
           return *Img->Data;
       }
       
       if ((Img->Attributes & IMG_ATTRIBUTE_IS_RLE_COMPRESSED)
           && (!(Img->Colourformat & IMG_MODE_RLE))) {
           /* rle decompress image by CPU, only if image is of rle fomat
              and IMG_MODE_RLE flag is not set (means the image should not be
              decompressed by the Drawing engine during runtime */
           if (*Img->Data == 0) {  /* check, if ROMFS is to be used */
               /* use ROMFS */
               fp = eeRomFs_FOpen(Img->Name, "rb"); 
               if (!fp) { 
                   demo_freevidmem(Dev, data);
                   return 0; 
               }
               mm_data_addr = (uint32_t)eeRomFs_MMap(0, 0, 0, 0, fp, 0);
               if (!mm_data_addr) {               
                   demo_freevidmem(Dev, data);
                   return 0;
               }               
               locDecompressRLE(Dev, (uint32_t)(demo_getvidmemptr(Dev,data)),
                                (uint8_t*)mm_data_addr, Img->DataLength, bpp);
               eeRomFs_MUnmap((void*)mm_data_addr, 0);
               eeRomFs_FClose(fp);
           } else {
               locDecompressRLE(Dev, (uint32_t)(demo_getvidmemptr(Dev,data)), *Img->Data, Img->DataLength, bpp);
           }
       } else {
           if (*Img->Data == 0) {  /* check, if ROMFS is to be used */
               /* use ROMFS */
               fp = eeRomFs_FOpen(Img->Name, "rb");
               if (!fp) { 
                   demo_freevidmem(Dev, data);
                   return 0; 
               }
               mm_data_addr = (uint32_t)eeRomFs_MMap(0, 0, 0, 0, fp, 0);
               if (!mm_data_addr) {
                   demo_freevidmem(Dev, data);
                   return 0;
               }
               demo_copytovidmem(Dev, data, (void*)mm_data_addr, Img->DataLength);
               eeRomFs_MUnmap((void*)mm_data_addr, 0);
               eeRomFs_FClose(fp);
           } else {
               if (!mm_data_addr) {
                   demo_copytovidmem(Dev, data, (void*)mm_data_addr, Img->DataLength);
               }
           }
       }
       old        = *Img->Data;
       *Img->Data = data;
    }
    return old;
}


/***********************************************************
  Function: Deinit2DImg
*/

void Deinit2DImg(davehd_dev_t *Dev, Img_t *Img, void *Original) {
    if (Img->Attributes & IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY) {
        /* don't copy image to video memory,
            directly use it from persistent memory (Flash) */
        /*    -> nothing to free */
    } else {
        /* copy image to video memory */
        /*    -> video memory can be freed now */
        demo_freevidmem(Dev, *(Img->Data));
    }
    *Img->Data = Original;
}

