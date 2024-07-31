#!/bin/bash

VLIB_ROOT=$(pwd | sed '{s/vlib/vlib /;}' | awk '{print $1};')
OUT_FLASH_DATA_NAME=data_flash_to_$1
OUT_FS_DATA_NAME=fs_data.c

echo "    {0,0,0}" >> $OUT_FS_DATA_NAME
echo "};" >> $OUT_FS_DATA_NAME

echo "" >> $OUT_FS_DATA_NAME

# Only write iROM array if it holds any data.
if [ ! -f $OUT_FLASH_DATA_NAME ];then
    exit
fi

echo -n "static " >> $OUT_FS_DATA_NAME
$VLIB_ROOT/utils/bin2array/target/x86_gnu/bin2array.out -array _iROM_data -src $OUT_FLASH_DATA_NAME >> $OUT_FS_DATA_NAME