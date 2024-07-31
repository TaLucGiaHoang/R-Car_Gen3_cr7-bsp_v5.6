/*
 Image file name is BrakeOn.tga
 Image width is 65 pixel.
 Image heigth is 55 pixel.
 Image colour depth is 32 bit (or 4 bytes per pixel).
 Image doesn't contain a colour palette (index).
 RLE image data start at byte 18 and end at byte 13882.
*/

/* File will be remapped to Dx4-RGL framebuffer format IMG_ARGB8888. */

/* Output mode is 'Binary'.                                      */
/* Image data is stored binary to separate file.                 */
/* Only C structure describing the image is stored in this file. */

#include "r_typedefs.h"
#include "img_format.h"

static uint8_t *locBrakeOnDataPtr = (uint8_t*)0;

const Img_t BrakeOn = {
    "cluster_BrakeOn.bin",
    65,
    55,
    IMG_ARGB8888,
    0,
    (0 | IMG_ATTRIBUTE_IS_RLE_COMPRESSED),
    0,
    0,
    0,
    &locBrakeOnDataPtr,
    13865
};

