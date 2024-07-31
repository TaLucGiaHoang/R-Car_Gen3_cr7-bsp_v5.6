=============================================
1. How to build and convert osal sample apps
=============================================
1) Install BSP
   Please see readme.txt in BSP package

2) Unzip OSAL released package.
   unzip RTM8RC0000ZSAL2S00JPF0E.zip
   cp -r RTM8RC0000ZSAL2S00JPF0E/vlib/* h3-cr7-bsp-freertos/vlib
   cd h3-cr7-bsp-freertos

[osal_sample_resource]
$ make -C ./vlib/app/osal_sample_resource/target/freertos_salvatorxs/ LSI=H3
$ cd ./vlib/app/osal_sample_resource/target/freertos_salvatorxs/
$ arm-none-eabi-objcopy --srec-forceS3 -O srec osal_sample_resource.out cr7-rtos.srec

[osal_sample_memory]
$ make -C ./vlib/app/osal_sample_memory/target/freertos_salvatorxs/ LSI=H3
$ cd ./vlib/app/osal_sample_memory/target/freertos_salvatorxs/
$ arm-none-eabi-objcopy --srec-forceS3 -O srec osal_sample_memory.out cr7-rtos.srec

[osal_sample_device]
$ make -C ./vlib/app/osal_sample_device/target/freertos_salvatorxs/ LSI=H3
$ cd ./vlib/app/osal_sample_device/target/freertos_salvatorxs/
$ arm-none-eabi-objcopy --srec-forceS3 -O srec osal_sample_device.out cr7-rtos.srec
