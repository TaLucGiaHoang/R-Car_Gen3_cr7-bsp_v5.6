/*
 Image file name is 1920x720_Dashboard_BG.tga
 Image width is 1920 pixel.
 Image heigth is 720 pixel.
 Image colour depth is 32 bit (or 4 bytes per pixel).
 Image doesn't contain a colour palette (index).
 Raw image data start at byte 18 and end at byte 5529617.
*/

/* File will be remapped to Dx4-RGL framebuffer format IMG_ARGB8888. */

/* Output mode is 'Binary'.                                      */
/* Image data is stored binary to separate file.                 */
/* Only C structure describing the image is stored in this file. */

#include "r_typedefs.h"
#include "img_format.h"

static uint8_t *locClusterBackGroundDataPtr = (uint8_t*)0;

const Img_t ClusterBackGround = {
    "cluster_background.bin",
    1920,
    720,
    IMG_ARGB8888,
    0,
    (0),
    0,
    0,
    0,
    &locClusterBackGroundDataPtr,
    5529600
};

