


Cortex-R7 Loader (rev.2.0.9)
How to customize source code for FreeRTOS
                                                                      2022.04.25

The size of RTOS has been changed from the alpha version.
For details on the changes, see "3. Changes".
There is no change in the Loader version. 

1. Overview

This document describes how to customize Cortex-R7 Loader's source code for FreeRTOS.


2. File list

  bootparam_sa0.srec                              Loader (Boot parameter)
  cert_header_sa3.srec                            Loader (Certificate) / Updated for FreeRTOS
  cr7_loader.srec                                 Loader
  readme.txt                                      This file


3. Writing data

Refer to Application Note of Flash Writer or Manual of Minimonitor about how to write data.
Following information is required when writing data by Flash Writer or Minimonitor.

+----------------------+---------------------+--------------------+--------------------------------------------------+
| Filename             | Program Top Address | Flash Save Address | Description                                      |
|----------------------+---------------------+--------------------+--------------------------------------------------+
| bootparam_sa0.srec   | 0xE6320000          | 0x00000000         | Loader (Boot parameter)                          |
| cr7_loader.srec      | 0xE6304000          | 0x00040000         | Loader                                           |
| cert_header_sa3.srec | 0xE6320000          | 0x000C0000         | Loader (Certificate)                             |
| dummy_rtos.srec      | 0x70000000          | 0x00740000         | Dummy RTOS (instead of FreeRTOS)                 |
| bl2-salvator-x.srec  | 0xE6304000          | 0x00140000         | BL2                                              |
| cert_header_sa6.srec | 0xE6320000          | 0x00180000         | Loader (Certificate)                             |
| bl31-salvator-x.srec | 0x44000000          | 0x001C0000         | BL31                                             |
| tee-salvator-x.srec  | 0x44100000          | 0x00200000         | op-tee                                           |
| u-boot-elf.srec      | 0x50000000          | 0x00640000         | u-boot                                           |
+----------------------+---------------------+--------------------+--------------------------------------------------+



