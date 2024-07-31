/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: Texture Module Interface

  An application can use these functions to setup textures
  in the GPU memory or release the memory used by a texture.
*/

/***********************************************************
  Section: Global Functions
*/

/***********************************************************
  Function: InitImg

  Prepare an image for drawing engine usage.

  If an image is located in the CPU memory, copy it to the 
  video memory. After the copy operation or if the image is
  already in the GPU memory (e.g. flash), adjust the pointer
  in the image structure.

  Parameters:
  Dev        - D2D device structure
  Img        - Image structure

  Returns:
  Original image data pointer
*/

void *Init2DImg(davehd_dev_t *dev, Img_t *Img);


/***********************************************************
  Function: Deinit2DImg

  Remove an image from the GPU memory.

  If an image is located in the GPU memory, the GPU memory
  will be released.

  Parameters:
  Img        - Image structure
  Original   - Pointer to the original image data (e.g. CPU flash)

  Returns:
  void
*/

void Deinit2DImg(davehd_dev_t *dev, Img_t *Img, void *Original);


#ifdef __cplusplus
}
#endif

#endif /* _TEXTURE_H_ */

