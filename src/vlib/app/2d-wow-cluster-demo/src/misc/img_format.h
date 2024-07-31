/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#ifndef _IMG_FORMAT_H_
#define _IMG_FORMAT_H_


#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: 2D Image Format Definition
  
  This header describes the 2D image format used by the
  framework.
*/

/***********************************************************
  Section: Global Types
*/

/***********************************************************
  Enum: Img_ColourFormat_t

  Image color format type.

  Values:
  IMG_ALPHA8     - 8bit Alpha Format
  IMG_RGB565     - RGB565 Format
  IMG_ARGB8888   - ARGB8888 Format
  IMG_ARGB1555   - ARGB1555 Format
  IMG_RGB888     - RGB888 Format

  additional values used as flags can be ored together with previous modes:
  IMG_MODE_RLE   - Format is RLE decoded
*/

typedef enum {
    IMG_ALPHA8      = 0,
    IMG_RGB565      = 1,
    IMG_ARGB8888    = 2,
    IMG_ARGB1555    = 4,
    IMG_RGB888      = 5,
    IMG_MODE_RLE    = 16
} Img_ColourFormat_t;


/***********************************************************
  Enum: Img_Attributes_t

  Attribute flags of the image. (bit description of image_t->attributes variable)
  The attribute flags can be ored together.

  Values:
  IMG_ATTRIBUTE_IS_RLE_COMPRESSED           - bit 0 'isrlecompressed'
                                                ==0 texture is RAW,
                                                ==1 texture is RLE compressed
  IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY  - bit 1 'useFromPersistentMemory'
                                                ==0 allow copy data to video memory before usage
                                                ==1 use data directly from persitent memory
*/

typedef enum {
    IMG_ATTRIBUTE_IS_RLE_COMPRESSED             = (1 << 0),
    IMG_ATTRIBUTE_USE_FROM_PERSISTENT_MEMORY    = (1 << 1),
    IMG_ATTRIBUTE_BIT2_UNUSED                   = (1 << 2),
    /* ... */
    IMG_ATTRIBUTE_BIT31_UNUSED                  = (1 << 31)
} Img_Attributes_t;


/***********************************************************
  Type: Img_t

  The structure contains all data of a texture.

  Members:
  Name             - Name of the image or 0
  Width            - Width of the texture
  Height           - Height of the texture
  Colourformat     - Drawing engine color format
  IsIndexed        - !=0 => texture is indexed
  Attributes       - collection of image attributes (see below)
   bit 0:          - ==0 texture is RAW, ==1 texture is RLE compressed (bit 'isRleCompressed')
   bit 1:          - =!0 use data directly from persitent memory (bit 'useFromPersistentMemory')  
  Index            - Pointer to index table
  IndexLength      - Length of the index table in bytes
  IndexEntrys      - Number of entries in the table
  Data             - Pointer to image data / pointer to image in VRAM
  DataLength       - Image data length
*/

typedef struct {
    char                   *Name;
    uint32_t                Width;
    uint32_t                Height;
    int                     Colourformat;
    uint32_t                IsIndexed;
    uint32_t                Attributes;
    uint8_t               **Index;
    uint32_t                IndexLength;
    uint32_t                IndexEntrys;
    uint8_t               **Data;
    uint32_t                DataLength;
} Img_t;


#ifdef __cplusplus
}
#endif

#endif /* _IMG_FORMAT_H_ */

