#!/bin/bash

if [ $# != 1 ];then
echo "usage: $0 <start_address_in_srec>"
echo "Pipe the filenames into $0."
exit 1
fi

FILENAMES=$(cat )
START_ADDRESS=$1
OLD_START_ADDRESS=$1
OUT_FS_DATA_NAME=../../misc/fs_data.c
OUT_FLASH_DATA_NAME=../../misc/data_flash_to_$1
OUT_FLASH_DATA_NAME_SREC=../../misc/data_flash.srec
#ALIGN=128*1024 # 128k align
ALIGN=1024 # 1kb align it does not really matter but it is safer for execution in place
#    #define ALIGN(arg,value) (((arg)+(value)-1)&~((value)-1))

cat /dev/null > $OUT_FLASH_DATA_NAME # clear file
cat /dev/null > $OUT_FS_DATA_NAME # clear file

echo "#include \"r_typedefs.h\"" >> $OUT_FS_DATA_NAME
echo "#include \"romfs_api.h\"" >> $OUT_FS_DATA_NAME
echo "const eeRomFs_Data_t RomFileSystemData[]={" >> $OUT_FS_DATA_NAME
for i in $FILENAMES;do
    SIZE=$(wc -c <$i)
    ALIGNED_SIZE=$[ (SIZE+ALIGN-1)&~(ALIGN-1) ]
    DIFFERENCE=$[ ALIGNED_SIZE-SIZE ]
    echo $i size=$SIZE  aligned_size=$ALIGNED_SIZE difference=$DIFFERENCE
    QUOTED_NAME=\"$(echo $i| sed 's/\\/\\\\/')\"
    printf "    {%-30s,%10d, (char*)0x%08x},\n" $QUOTED_NAME $[SIZE] $[ START_ADDRESS ] >> $OUT_FS_DATA_NAME
    START_ADDRESS=$[START_ADDRESS+ALIGNED_SIZE]
    echo hallo | gawk -vsize=$DIFFERENCE '{for(i=0;i<size;i++) printf"%c",0}' | cat $i - >> $OUT_FLASH_DATA_NAME
done
echo "    {\"\",0,0}," >> $OUT_FS_DATA_NAME
echo "};" >> $OUT_FS_DATA_NAME

objcopy --srec-forceS3 --change-addresses $OLD_START_ADDRESS  -I binary -O srec $OUT_FLASH_DATA_NAME  $OUT_FLASH_DATA_NAME_SREC

printf  "Creating file %s start address is 0x%08x end address is 0x%08x size 0x%08x\n" \
    $OUT_FS_DATA_NAME $OLD_START_ADDRESS $START_ADDRESS $[START_ADDRESS-OLD_START_ADDRESS]

