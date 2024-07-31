/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/


/***********************************************************
  Title: File System Main Module 
*/

/***********************************************************
  Section: Includes
*/

#include "r_typedefs.h"
#include "romfs_api.h"
#include <string.h>
//#include <stdio.h>

//#define USE_FLASH_IMAGE_DATA
#ifdef USE_FLASH_IMAGE_DATA
  #ifdef WIN32
    //#include <stdlib.h>
  #else
//    #include <sys/alt_flash.h>
//    #define WOW_CLUSTER_DATA_L 1281652
  #endif
#endif
/***********************************************************
  Section: Local Defines
*/

/* Limits */
#define LOC_ROMFS_MAXOPEN        2

/* define "GENERATE_CPP_IMAGE_DATA" to generate a source file with the image data.*/
#ifdef USE_FLASH_IMAGE_DATA
/*#define GENERATE_CPP_IMAGE_DATA*/
#endif
/***********************************************************
  Section: Local Variables
*/


static eeRomFs_Data_t const *locRoot;
static eeRomFs_File_t locHandle[LOC_ROMFS_MAXOPEN];

#ifdef USE_FLASH_IMAGE_DATA
static void *pBinImageDataBuffer;
#else
extern const uint8_t ClusterImageData[];
extern const unsigned long sizeofClusterImageData ;
#endif

/***********************************************************
  Section: Global Functions
*/

/***********************************************************
  Function: eeRomFs_FOpen

  See: <romfs_api.h>
*/

eeRomFs_File_t *eeRomFs_FOpen(const char *Name, char *Mode) {
    int i;
    int j;

    /* Check if FS is initialized */
    if (locRoot == 0) {
        /* No valid pointer */
        return 0;
    }

    /* Search free handle */
    i = 0;
    while (i < LOC_ROMFS_MAXOPEN) {
        if (locHandle[i].InUse == 0) {
            break;
        }
        i++;
    }
    if (i >= LOC_ROMFS_MAXOPEN) {
        /* No free entry */
        return 0;
    }

    /* Find file */
    j = 0;
    while (locRoot[j].Address) {
        if (strcmp(Name, locRoot[j].Name) == 0) {
            /* File found */
            locHandle[i].InUse   = 1;
            locHandle[i].Pos     = 0;
            locHandle[i].Size    = locRoot[j].Size;
#ifdef USE_FLASH_IMAGE_DATA    
            locHandle[i].Address = (char *)(locRoot[j].Address  - locRoot[0].Address + (uint32_t)pBinImageDataBuffer);
#else
            locHandle[i].Address = (char *)(locRoot[j].Address  - locRoot[0].Address + (uint32_t)ClusterImageData);
#endif
            return &locHandle[i];
        }
        j++;
    }
    /* File not found */
    return 0;
}

/***********************************************************
  Function: eeRomFs_FClose

  See: <romfs_api.h>
*/

int eeRomFs_FClose(eeRomFs_File_t *Fp) {
    if (Fp == 0) {
        return -1;
    }
    Fp->InUse = 0;
    return 0;
}


/***********************************************************
  Function: eeRomFs_FRead

  See: <romfs_api.h>
*/

uint32_t eeRomFs_FRead(void *Buffer, uint32_t Size, uint32_t Count, eeRomFs_File_t *Fp) {
    uint32_t n;

    if (Fp == 0) {
        return 0;
    }
    if (Buffer == 0) {
        return 0;
    }
    n = Size * Count;
    if ((Fp->Pos + n) >= Fp->Size) {
        n = Fp->Size - Fp->Pos;
    }
    memcpy(Buffer, Fp->Address + Fp->Pos, n);
    Fp->Pos += n;
    n /= Size;
    return n;
}


/***********************************************************
  Function: eeRomFs_MMap

	See: <romfs_api.h>
*/

void *eeRomFs_MMap(void *Addr, uint32_t Size, int32_t Prot,
                    int32_t Flags, eeRomFs_File_t *Fp, uint32_t Offset) {
    return (Fp->Address + Offset);
}


/***********************************************************
  Function: eeRomFs_MUnmap

	See: <romfs_api.h>
*/

int32_t eeRomFs_MUnmap(void *addr, uint32_t Size) {
    return 0;
}


/***********************************************************
  Function: eeRomFs_Init

  See: <romfs_api.h>
*/

int  eeRomFs_Init(const eeRomFs_Data_t *PData) {

#ifdef USE_FLASH_IMAGE_DATA
#ifdef WIN32
    FILE *pImagesF = 0;
    size_t redSize = 0;
    size_t fileLen = 0;

#ifdef GENERATE_CPP_IMAGE_DATA
    FILE *pImagesFCpp = 0;
    unsigned long iIndex = 0;
#endif
#endif


#ifdef WIN32
     /* search for the file imageDataFlash with all the image data needed */
    pImagesF = fopen("../../src_demos/2dWowCluster/misc/imageDataFlash","rb");
    if (pImagesF == 0) {

      pImagesF = fopen("../../../../../code/2dWowCluster/misc/imageDataFlash","rb");
      if (pImagesF == 0) {
      
        pImagesF = fopen("S:\\demos\\dave2_demos\\src_demos\\2dWowCluster\\misc\\imageDataFlash","rb");

        if (pImagesF == 0) {
          return 1; /* error*/
        }
      }
    }
    
    fseek(pImagesF, 0, SEEK_END);
    fileLen = ftell(pImagesF);
    rewind(pImagesF);

    pBinImageDataBuffer = malloc((size_t)(fileLen+1));
    
    redSize = fread(pBinImageDataBuffer, 1, fileLen, pImagesF);

#ifdef GENERATE_CPP_IMAGE_DATA
    /* create an empty file*/
    pImagesFCpp = fopen("S:\\demos\\dave2_demos\\src_demos\\2dWowCluster\\misc\\ImageData.cpp","w");

    if (0 != pImagesFCpp)
    {  
      for (iIndex = 0; iIndex < fileLen+1; iIndex++)
      {
        fprintf(pImagesFCpp, "%3d, ", ((uint8_t *)pBinImageDataBuffer)[iIndex]);
        if (iIndex%30 == 29)
        {
          fputc('\n', pImagesFCpp);    
        }
      }
      fclose(pImagesFCpp);
    }
#endif


    if (redSize != fileLen) {
      return 1; /* error*/
    }
    
    fclose(pImagesF);  

#else   /* #ifdef WIN32*/

#endif  /* #else #ifdef WIN32*/
#else /* #ifdef USE_FLASH_IMAGE_DATA*/
    locRoot = PData;
#endif  /* #ifdef USE_FLASH_IMAGE_DATA*/
    return 0;
}

int  eeRomFs_DeInit() {

#ifdef USE_FLASH_IMAGE_DATA
#ifdef WIN32    
    free(pBinImageDataBuffer);
    pBinImageDataBuffer = 0;
#else
    free(pBinImageDataBuffer);
    pBinImageDataBuffer = 0;
#endif
#endif
    return 0;
}
