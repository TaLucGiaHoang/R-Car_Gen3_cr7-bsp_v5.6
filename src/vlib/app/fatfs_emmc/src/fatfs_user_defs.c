#include "ff.h"
#include "diskio.h"
#include "emmc_def.h"
#include "emmc_config.h"
#include "string.h"

PARTITION VolToPart[] = {
	{1,1},  // -> "0:/"
	{1,2},  // -> "1:/"
	{1,3},  // -> "2:/"
	{1,4},  // -> "3:/"
};	/* Volume - Partition mapping table */

/* Get current time */
DWORD get_fattime (void) {
    return 0;
}

DSTATUS MMC_disk_initialize(void) {
    int status;
    status = rcar_emmc_init();
    if (EMMC_SUCCESS != status)
        return STA_NOINIT;
    rcar_emmc_memcard_power(EMMC_POWER_ON);
    status = rcar_emmc_mount();
    if (EMMC_SUCCESS != status)
        return STA_NOINIT;
    return 0;
}

DSTATUS MMC_disk_status(void) {
    return 0;
}

DRESULT MMC_disk_read(BYTE * buff, DWORD sector, BYTE count) {
    int status;
    uint32_t dma = 0;
    if ((((uintptr_t)buff) & 0x7f) == 0) // If buffer is 128-byte aligned, use DMA
        dma = LOADIMAGE_FLAGS_DMA_ENABLE;
    status = emmc_read_sector((uint32_t*)buff, sector, count, dma);
    if (EMMC_SUCCESS != status)
        return RES_ERROR;
    return RES_OK;
}

DRESULT MMC_disk_write(const BYTE *buff, DWORD sector, BYTE count) {
    int status;
    uint32_t dma = 0;
    if ((((uintptr_t)buff) & 0x7f) == 0) // If buffer is 128-byte aligned, use DMA
        dma = LOADIMAGE_FLAGS_DMA_ENABLE;
    status = emmc_write_sector((uint32_t*)buff, sector, count, dma);
    if (EMMC_SUCCESS != status)
        return RES_ERROR;
    return RES_OK;
}

DRESULT MMC_disk_ioctl(BYTE cmd, void *buff) {
    DRESULT result = RES_OK;
    int status;

    switch (cmd) {
    case MMC_GET_TYPE:
        *(BYTE*)buff = 1;
        break;

    case CTRL_SYNC:
        // Do Nothing
        break;

    case GET_BLOCK_SIZE:
        *(WORD*)buff = (WORD)EMMC_CSD_ERASE_GRP_SIZE()+1;
        break;

    case GET_SECTOR_SIZE:
        *(WORD*)buff = (WORD)EMMC_SECTOR_SIZE;
        break;

    case GET_SECTOR_COUNT:
        // Per SD Spec V9.0: BLOCK_NR = (C_SIZE+1)*(2^(C_SIZE_MULT+2))
        *(DWORD*)buff = (EMMC_CSD_C_SIZE()+1) * (4 << EMMC_CSD_C_SIZE_MULT());
        break;

    case CTRL_TRIM:
        // Do Nothing
        break;

    case MMC_GET_SDSTAT:
        /* CMD13 */
        emmc_make_trans_cmd(CMD13_SEND_STATUS, EMMC_RCA << 16,
                (uint32_t *) (buff), EMMC_SD_STATUS_SIZE,
                HAL_MEMCARD_READ, HAL_MEMCARD_NOT_DMA);
        status = emmc_exec_cmd(EMMC_R1_ERROR_MASK_WITHOUT_CRC, mmc_drv_obj.response);
        if (status != EMMC_SUCCESS)
            result = RES_ERROR;
        break;

    case MMC_GET_CID:
        memcpy(buff, mmc_drv_obj.cid_data, EMMC_MAX_CID_LENGTH);
        break;

    case MMC_GET_CSD:
        memcpy(buff, mmc_drv_obj.csd_data, EMMC_MAX_CSD_LENGTH);
        break;

    case MMC_GET_OCR:
        *(DWORD*)buff = mmc_drv_obj.r3_ocr;
        break;

    default:
        result = RES_ERROR;
        break;
    }
    return result;
}


DSTATUS RAM_disk_initialize(void) { return 0; }
DSTATUS RAM_disk_status(void) { return 0; }
DRESULT RAM_disk_read(BYTE *buff, DWORD sector, BYTE count) { return 0; }
DRESULT RAM_disk_write(const BYTE *buff, DWORD sector, BYTE count) { return 0; }
DRESULT RAM_disk_ioctl(BYTE cmd, void *buff) { return 0; }

DSTATUS USB_disk_initialize(void) { return 0; }
DSTATUS USB_disk_status(void) { return 0; }
DRESULT USB_disk_read(BYTE *buff, DWORD sector, BYTE count) { return 0; }
DRESULT USB_disk_write(const BYTE *buff, DWORD sector, BYTE count) { return 0; }
DRESULT USB_disk_ioctl(BYTE cmd, void *buff) { return 0; }
