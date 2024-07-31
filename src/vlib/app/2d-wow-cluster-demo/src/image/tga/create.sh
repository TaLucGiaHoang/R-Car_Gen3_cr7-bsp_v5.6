#!/bin/bash
VLIB_ROOT=$(pwd | sed '{s/vlib/vlib /;}' | awk '{print $1};')

$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i icon_cluster.tga         -s IconCluster                   -o ./src/icon_cluster.c             -b ./data/icon_cluster.bin
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i 800x480_Dashboard_BG.tga -s ClusterBackGround -c ARGB8888 -o ./src/cluster_background.c       -b ./data/cluster_background.bin      -r
#$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i Needle_red.tga           -s ClusterNeedle     -c ARGB8888 -o ./src/cluster_needle.c           -b ./data/cluster_needle.bin          -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i MetalNeedle.tga          -s ClusterNeedle     -c ARGB8888 -o ./src/cluster_needle.c           -b ./data/cluster_needle.bin          -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i Blinker_left.tga         -s TurnLeft          -c ARGB8888 -o ./src/cluster_turn_left.c        -b ./data/cluster_turn_left.bin       -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i Blinker_left_off.tga     -s TurnLeftOff       -c ARGB8888 -o ./src/cluster_turn_left_off.c    -b ./data/cluster_turn_left_off.bin   -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i Blinker_right.tga        -s TurnRight         -c ARGB8888 -o ./src/cluster_turn_right.c       -b ./data/cluster_turn_right.bin      -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i Blinker_right_off.tga    -s TurnRightOff      -c ARGB8888 -o ./src/cluster_turn_right_off.c   -b ./data/cluster_turn_right_off.bin  -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i Hazard_Icon_Normal.tga   -s HazardNormal      -c ARGB8888 -o ./src/cluster_hazard_normal.c    -b ./data/cluster_hazard_normal.bin   -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i Hazard_Icon_Selected.tga -s HazardSelected    -c ARGB8888 -o ./src/cluster_hazard_selected.c  -b ./data/cluster_hazard_selected.bin -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i BatteryOff.tga           -s BatteryOff        -c ARGB8888 -o ./src/cluster_BatteryOff.c       -b ./data/cluster_BatteryOff.bin      -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i BatteryOn.tga            -s BatteryOn         -c ARGB8888 -o ./src/cluster_BatteryOn.c        -b ./data/cluster_BatteryOn.bin       -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i FanOff.tga               -s FanOff            -c ARGB8888 -o ./src/cluster_FanOff.c           -b ./data/cluster_FanOff.bin          -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i FanOn.tga                -s FanOn             -c ARGB8888 -o ./src/cluster_FanOn.c            -b ./data/cluster_FanOn.bin           -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i HighLightOff.tga         -s HighLightOff      -c ARGB8888 -o ./src/cluster_HighLightOff.c     -b ./data/cluster_HighLightOff.bin    -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i HighLightOn.tga          -s HighLightOn       -c ARGB8888 -o ./src/cluster_HighLightOn.c      -b ./data/cluster_HighLightOn.bin     -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i LowLightOff.tga          -s LowLightOff       -c ARGB8888 -o ./src/cluster_LowLightOff.c      -b ./data/cluster_LowLightOff.bin     -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i LowLightOn.tga           -s LowLightOn        -c ARGB8888 -o ./src/cluster_LowLightOn.c       -b ./data/cluster_LowLightOn.bin      -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i OilOff.tga               -s OilOff            -c ARGB8888 -o ./src/cluster_OilOff.c           -b ./data/cluster_OilOff.bin          -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i OilOn.tga                -s OilOn             -c ARGB8888 -o ./src/cluster_OilOn.c            -b ./data/cluster_OilOn.bin           -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i AirbagOff.tga            -s AirbagOff         -c ARGB8888 -o ./src/cluster_AirbagOff.c        -b ./data/cluster_AirbagOff.bin       -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i AirbagOn.tga             -s AirbagOn          -c ARGB8888 -o ./src/cluster_AirbagOn.c         -b ./data/cluster_AirbagOn.bin        -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i BeltOff.tga              -s BeltOff           -c ARGB8888 -o ./src/cluster_BeltOff.c          -b ./data/cluster_BeltOff.bin         -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i BeltOn.tga               -s BeltOn            -c ARGB8888 -o ./src/cluster_BeltOn.c           -b ./data/cluster_BeltOn.bin          -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i BrakeOff.tga             -s BrakeOff          -c ARGB8888 -o ./src/cluster_BrakeOff.c         -b ./data/cluster_BrakeOff.bin        -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i BrakeOn.tga              -s BrakeOn           -c ARGB8888 -o ./src/cluster_BrakeOn.c          -b ./data/cluster_BrakeOn.bin         -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i WindshieldOff.tga        -s WindshieldOff     -c ARGB8888 -o ./src/cluster_WindshieldOff.c    -b ./data/cluster_WindshieldOff.bin   -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i WindshieldOn.tga         -s WindshieldOn      -c ARGB8888 -o ./src/cluster_WindshieldOn.c     -b ./data/cluster_WindshieldOn.bin    -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i WipingOff.tga            -s WipingOff         -c ARGB8888 -o ./src/cluster_WipingOff.c        -b ./data/cluster_WipingOff.bin       -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i WipingOn.tga             -s WipingOn          -c ARGB8888 -o ./src/cluster_WipingOn.c         -b ./data/cluster_WipingOn.bin        -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i CoolingLiquidOn.tga      -s CoolingLiquidOn   -c ARGB8888 -o ./src/cluster_CoolingLiquidOn.c  -b ./data/cluster_CoolingLiquidOn.bin -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i CoolingLiquidOff.tga     -s CoolingLiquidOff  -c ARGB8888 -o ./src/cluster_CoolingLiquidOff.c -b ./data/cluster_CoolingLiquidOff.bin    -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i FuelEmptyOn.tga          -s FuelEmptyOn       -c ARGB8888 -o ./src/cluster_FuelEmptyOn.c      -b ./data/cluster_FuelEmptyOn.bin     -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i FuelEmptyOff.tga         -s FuelEmptyOff      -c ARGB8888 -o ./src/cluster_FuelEmptyOff.c     -b ./data/cluster_FuelEmptyOff.bin    -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i gear_d_off.tga           -s GearDOff          -c ARGB8888 -o ./src/cluster_gear_d_off.c     -b ./data/cluster_gear_d_off.bin        -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i gear_d_on.tga            -s GearDOn           -c ARGB8888 -o ./src/cluster_gear_d_on.c      -b ./data/cluster_gear_d_on.bin         -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i gear_n_off.tga           -s GearNOff          -c ARGB8888 -o ./src/cluster_gear_n_off.c     -b ./data/cluster_gear_n_off.bin        -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i gear_n_on.tga            -s GearNOn           -c ARGB8888 -o ./src/cluster_gear_n_on.c      -b ./data/cluster_gear_n_on.bin         -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i gear_p_off.tga           -s GearPOff          -c ARGB8888 -o ./src/cluster_gear_p_off.c     -b ./data/cluster_gear_p_off.bin        -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i gear_p_on.tga            -s GearPOn           -c ARGB8888 -o ./src/cluster_gear_p_on.c      -b ./data/cluster_gear_p_on.bin         -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i gear_r_off.tga           -s GearROff          -c ARGB8888 -o ./src/cluster_gear_r_off.c     -b ./data/cluster_gear_r_off.bin        -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i gear_r_on.tga            -s GearROn           -c ARGB8888 -o ./src/cluster_gear_r_on.c      -b ./data/cluster_gear_r_on.bin         -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i gear_s_off.tga           -s GearSOff          -c ARGB8888 -o ./src/cluster_gear_s_off.c     -b ./data/cluster_gear_s_off.bin        -r
$VLIB_ROOT/app/util/tgadataconvert/target/x86_gnu/tgadataconvert.out -i gear_s_on.tga            -s GearSOn           -c ARGB8888 -o ./src/cluster_gear_s_on.c      -b ./data/cluster_gear_s_on.bin         -r

