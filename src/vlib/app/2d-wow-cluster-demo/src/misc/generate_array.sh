#!/bin/bash
IMAGE_NAME="ImageData.c"

echo Generating header
cat <<EOF >$IMAGE_NAME
#ifndef USE_FLASH_IMAGE_DATA
#include "r_typedefs.h"
EOF

echo Generating array
bin2array -src data_flash_to_0xf0000000  -array   ClusterImageData -format uint8_t >>$IMAGE_NAME

echo Generating footer
cat <<EOF >>$IMAGE_NAME

const unsigned long sizeofClusterImageData = sizeof(ClusterImageData);  

#endif /*#ifndef USE_FLASH_IMAGE_DATA*/

EOF
echo Done