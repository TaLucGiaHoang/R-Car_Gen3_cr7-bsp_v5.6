#!/bin/bash


OUT_FS_DATA_NAME=fs_data.c
OUT_FLASH_DATA_SF_NAME=data_flash_sf_to_$1
OUT_FLASH_DATA_NAME=data_flash_to_$1

#cat /dev/null > $OUT_FLASH_DATA_SF_NAME # clear file
#cat /dev/null > $OUT_FLASH_DATA_NAME # clear file
#cat /dev/null > $OUT_FS_DATA_NAME # clear file

echo "#include \"r_typedefs.h\"" >> $OUT_FS_DATA_NAME
echo "#include \"romfs_api.h\"" >> $OUT_FS_DATA_NAME
echo "" >> $OUT_FS_DATA_NAME
echo "static const char _iROM_data[] __attribute__((aligned (256)));" >> $OUT_FS_DATA_NAME
echo "" >> $OUT_FS_DATA_NAME
echo "const R_ROMFS_Data_t RomFileSystemData[]={" >> $OUT_FS_DATA_NAME
