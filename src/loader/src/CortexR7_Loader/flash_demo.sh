#!/bin/bash
# Copyright Renesas 2019
# written by Sven Grundmann
#
DEVICE=/dev/ttyUSB0
#function write_ipl <filename> <program top address> <flash save address>
# example write_ipl bootparam_sa0.srec E6320000 000000
#board_name=draak
#board_name=hf
board_name=salvator-x
#board_name=
DELAY="sleep .1"
#DELAY=$DELAY
DEPLOY_DIR=.

echo "DEPLOY_DIR=" $DEPLOY_DIR

function finish {
  # Your cleanup code here
  stty -F $DEVICE 115200
}
trap finish EXIT

function write_ipl {
echo Flashing $1
echo -e >$DEVICE "\r"
echo -e >$DEVICE "xls2\r"
$DELAY
echo -e >$DEVICE "3\r"
$DELAY
echo -e >$DEVICE "Y"
$DELAY
echo -e >$DEVICE "Y"
$DELAY
if [ "$board_name" = "draak" ];then
echo -e >$DEVICE "Y"
$DELAY
fi
echo -e >$DEVICE "$2\r"
$DELAY
echo -e >$DEVICE "$3\r"
sleep 2
#cat >$DEVICE $1
pv >$DEVICE $1
sleep 1.2
echo -e >$DEVICE "y"
sleep 2
echo -e >$DEVICE "\r"
sleep .5
echo -e >$DEVICE "\r"
}
if [ "$1" = "bl2" ];then
flash_bl2="1"
shift
elif  [ "$1" = "u-boot" ];then
flash_u_boot="1"
shift
fi

stty -F $DEVICE 115200
#echo Writing Monitor program
echo Enable Scif Mode
echo Copying bootloader over Scif
pv AArch64_Gen3_H3_M3_Scif_MiniMon_V5.12.mot >$DEVICE

echo Minimonitor sent

sleep 2

#Increase Baudrate to max
echo -e >$DEVICE "\r"
echo -e >$DEVICE "SUP\r"
sleep .5
stty -F $DEVICE 921600
sleep .5


if [ "$1" = "all" ] ;then
echo Flashing all
#if true;then
    write_ipl $DEPLOY_DIR/bootparam_sa0.srec        E6320000 000000
    write_ipl $DEPLOY_DIR/cr7_loader.srec           E6304000 040000
    write_ipl $DEPLOY_DIR/cert_header_sa3.srec      E6320000 0C0000
#    write_ipl $DEPLOY_DIR/bin/M3/bl2-salvator-x.srec  E6304000 140000
#    write_ipl $DEPLOY_DIR/bin/M3/cert_header_sa6.srec      E6320000 180000
#    write_ipl $DEPLOY_DIR/bin/M3/bl31-salvator-x.srec 44000000 1C0000
#    write_ipl $DEPLOY_DIR/bin/M3/tee-salvator-x.srec       44100000 200000
sleep 4
#    write_ipl $DEPLOY_DIR/bin/M3/u-boot-elf-salvator-x.srec           50000000 640000
sleep 4
#    write_ipl $DEPLOY_DIR/bin/M3/cr7-rtos.srec              70000000 740000
sleep 64
stty -F $DEVICE 115200
else
    echo Flashing parts
#    write_ipl $DEPLOY_DIR/fromSven/IPL_files/bootparam_sa0.srec        E6320000 000000
#    write_ipl $DEPLOY_DIR/cr7_linaro/bootparam_sa0.srec        E6320000 000000
#    write_ipl $DEPLOY_DIR/cr7/testing/cr7_loader.srec           E6304000 040000

#    write_ipl $DEPLOY_DIR/cr7_linaro/cert_header_sa3.srec       E6320000 0C0000
#    write_ipl $DEPLOY_DIR/bl2-salvator-x-4x2g.srec   E6304000 140000
#    write_ipl $DEPLOY_DIR/cert_header_sa6.srec       E6320000 180000
#    write_ipl $DEPLOY_DIR/bl31-salvator-x-4x2g.srec 44000000 1C0000
#    write_ipl $DEPLOY_DIR/tee--salvator-x.srec      44100000 200000
#sleep 6
#    write_ipl $DEPLOY_DIR/u-boot-elf.srec           50000000 640000
#    write_ipl $DEPLOY_DIR/cluster_wrapper.srec      70000000 740000
#    write_ipl $DEPLOY_DIR/hello_rgl_wrapper.srec      70000000 740000
# For 2dwow not enough time to flash
##    write_ipl $DEPLOY_DIR/cr7_linaro/cr7-rtos.srec              70000000 740000
sleep 4
    write_ipl $DEPLOY_DIR/bin/M3/cluster_wrapper.srec              70000000 740000
#    write_ipl $DEPLOY_DIR/vlib/app/cluster_wrapper/target/freertos_salvatorxs/cluster_wrapper.srec              70000000 740000
#    write_ipl $DEPLOY_DIR/vlib/app/hello_rgl_wrapper/target/arm-gnu-freertos_cr7_r-carx3/hello_rgl_wrapper.srec              70000000 740000
#    write_ipl $DEPLOY_DIR/cr7/hello_rgl_on_cpu_wrapper.srec              70000000 740000
sleep 64
stty -F $DEVICE 115200
fi
