# eMMC FAT Filesystem App
## Description
This demo app by default looks for a FAT 32 filesystem in eMMC block 0 partition 1, mounts the filesystem, then writes and reads a file.
## Setup
This app expects a FAT 32 filesystem to already be present in eMMC, therefore you will need to make sure to create and format the filesystem using something like Yocto Linux. Instructions are provided for Yocto Linux, but other OSes like Android, should be capable but you'll need to figure out how to do these steps:
1. Boot into Yocto and login into root.
2. Create a partition in eMMC. Here we create parition 1, but you can put it in any available partition.
```text
root@salvator-x:~# fdisk /dev/mmcblk0
    > n // new parittion
    > p // primary
    > 1 // partition 1
    > t // set type
    > b // set type to FAT32
    > w // write to memory
```
3. Format the parition we created for FAT 32 filesystem using [mkfs.fat](https://github.com/dosfstools/dosfstools/releases/download/v4.2/dosfstools-4.2.tar.gz) which can be compiled from source (or you can build Yocto Linux with `IMAGE_INSTALL_append = " dosfstools"` added to `local.conf`). Change `mmcblk0p1` to match the partition you made if necessary.
```text
root@salvator-x:~# mkfs.fat /dev/mmcblk0p1
```
4. fatfs_emmc app expects these files in the filesystem:
```text
root@salvator-x:~# mkdir /mnt/fatfs
root@salvator-x:~# mount /dev/mmcblk0p1 /mnt/fatfs
root@salvator-x:~# cd /mnt/fatfs
root@salvator-x:/mnt/fatfs# echo "Hello, Renesas!" > test.txt
root@salvator-x:/mnt/fatfs# dd if=/dev/zero of=file.bin bs=1024 count=10240
```
5. Reboot to ensure changes are written to eMMC
```text
root@salvator-x:~# reboot
```
6. If you used block 0 partition 1 (`mmcblk0p1`), then you are ready to run the fatfs app, just make sure to run it while keeping the CA in UBoot. Otherwise, if you used a different parition then you will need to select the correct partition for your system. In `fatfs_emmc_main.c`, you will need to replace all `0:/` to the following:
```text
mmcblk0p1 -> '0:/'
mmcblk0p2 -> '1:/'
mmcblk0p3 -> '2:/'
mmcblk0p4 -> '3:/'
```