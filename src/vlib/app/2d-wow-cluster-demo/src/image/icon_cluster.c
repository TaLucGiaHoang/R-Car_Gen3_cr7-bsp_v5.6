/*
 Image file name is icon_cluster.tga
 Image width is 48 pixel.
 Image heigth is 46 pixel.
 Image colour depth is 24 bit (or 3 bytes per pixel).
 Image doesn't contain a colour palette (index).
 Raw image data start at byte 18 and end at byte 6641.
*/

/* File will be remapped to Dx4-RGL framebuffer format IMG_RGB565. */

/* Output mode is 'Binary'.                                      */
/* Image data is stored binary to separate file.                 */
/* Only C structure describing the image is stored in this file. */

#include "r_typedefs.h"
#include "img_format.h"

static uint8_t *locIconClusterDataPtr = (uint8_t*)0;

const Img_t IconCluster = {
    "icon_cluster.bin",
    48,
    46,
    IMG_RGB565,
    0,
    (0),
    0,
    0,
    0,
    &locIconClusterDataPtr,
    4416
};

