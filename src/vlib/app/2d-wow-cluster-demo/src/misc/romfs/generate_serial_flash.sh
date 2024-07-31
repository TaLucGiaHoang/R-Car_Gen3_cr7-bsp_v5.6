#!/bin/bash

if [ $# != 1 ];then
echo "usage: $0 <start_address_in_srec>"
echo "Pipe the filenames into $0."
exit 1
fi

FILENAMES=$(cat )
START_ADDRESS=$1
OLD_START_ADDRESS=$1
OUT_FS_DATA_NAME=../fs_data.c
OUT_FLASH_DATA_NAME=../data_flash_sf_to_$1
OUT_FLASH_DATA_NAME_SREC=../data_flash_sf.rec
#ALIGN=128*1024 # 128k align
ALIGN=256 # 256byte align
#    #define ALIGN(arg,value) (((arg)+(value)-1)&~((value)-1))

FILECOUNT=$(echo $FILENAMES|wc -w)
printf "Flash => %3d Files\n" $FILECOUNT

if [ "$FILECOUNT" -eq "0" ];then
    exit
fi

START_ADDRESS=$[START_ADDRESS]
for i in $FILENAMES;do
    SIZE=$(wc -c <$i)
    ALIGNED_SIZE=$[ (SIZE+ALIGN-1)&~(ALIGN-1) ]
    DIFFERENCE=$[ ALIGNED_SIZE-SIZE ]
    echo $i size=$SIZE  aligned_size=$ALIGNED_SIZE difference=$DIFFERENCE
    QUOTED_NAME=\"$(cygpath -w $i| sed 's/\\/\\\\/')\"
    printf "    {%-30s,%10d, (char*)0x%08x},\n" $QUOTED_NAME $[SIZE] $[ START_ADDRESS ] >> $OUT_FS_DATA_NAME
    START_ADDRESS=$[START_ADDRESS+ALIGNED_SIZE]
    echo hallo | gawk -vsize=$DIFFERENCE '{for(i=0;i<size;i++) printf"%c",0}' | cat $i - >> $OUT_FLASH_DATA_NAME
done

objcopy --srec-forceS3 --change-addresses $OLD_START_ADDRESS  -I binary -O srec $OUT_FLASH_DATA_NAME  $OUT_FLASH_DATA_NAME_SREC

printf  "Creating file %s start address is 0x%08x end address is 0x%08x size 0x%08x\n" \
    $OUT_FS_DATA_NAME $OLD_START_ADDRESS $START_ADDRESS $[START_ADDRESS-OLD_START_ADDRESS]

