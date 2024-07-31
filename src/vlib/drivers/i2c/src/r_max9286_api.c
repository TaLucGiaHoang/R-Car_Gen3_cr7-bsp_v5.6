/*************************************************************************************************************
* i2c_max9286_api_c
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/
#include "r_i2c_api.h"
#include "r_cpg_api.h"
#include "r_max9286_api.h"
#include "r_pinctrl_api.h"
#include "r_delay_api.h"
#include "r_print_api.h"

#define MAXIM_I2C_I2C_SPEED_837KHZ    (0x7 << 2) /* 837kbps */
#define MAXIM_I2C_I2C_SPEED_533KHZ    (0x6 << 2) /* 533kbps */
#define MAXIM_I2C_I2C_SPEED_339KHZ    (0x5 << 2) /* 339 kbps */
#define MAXIM_I2C_I2C_SPEED_173KHZ    (0x4 << 2) /* 174kbps */
#define MAXIM_I2C_I2C_SPEED_105KHZ    (0x3 << 2) /* 105 kbps */
#define MAXIM_I2C_I2C_SPEED_085KHZ    (0x2 << 2) /* 84.7 kbps */
#define MAXIM_I2C_I2C_SPEED_028KHZ    (0x1 << 2) /* 28.3 kbps */
#define MAXIM_I2C_I2C_SPEED        MAXIM_I2C_I2C_SPEED_339KHZ

#define DELAY_US_CHANNEL_POWER_CTRL 1000
#define DELAY_US_RE_WR              1000

#define REG8_NUM_RETRIES        3
#define BROADCAST               0x6f
#define TIMEOUT                 10
static int max9286_i2c_ch = R_I2C_IF4;
static uint8_t serializer_id;
static int links_mask;
static uint8_t max9271_addr_map[4] = {0xA0, 0xA2, 0xA4, 0xA6};
static uint8_t csi2_outord;

static char* get_serializer_name(int Id)
{
    switch(Id) {
    case MAX9271_ID:
        return "max9271";
    case MAX96705_ID:
        return "max96705";
    default:
        R_PRINT_Log("[get_serializer_name] : Wrong ID %d\r\n", Id);
        return 0;
    }
}



uint8_t MAX9271_MAP_ADDR(int port)
{
    return max9271_addr_map[port];
}

static void loc_I2cReset(r_i2c_Unit_t Unit)
{

    switch (Unit) {
    case R_I2C_IF0:
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT31, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT31, 0);
        break;
    case R_I2C_IF4:
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT27, 1);
        delay_us(R_DELAY_ONE_RCLK_CYCLE_IN_US); /* 1 RCLK cycle wait */
        R_PMA_CPG_SetResetStat(R_CPG_REGIDX_9, R_CPG_BIT27, 0);
        break;
    default:
        R_PRINT_Log("[loc_I2cReset : max9286] I2C Unit is invalid.\r\n");
        break;
    }
    return;
}

static void loc_I2cClkEnable(r_i2c_Unit_t Unit)
{
    switch (Unit) {
    case R_I2C_IF0:
        R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT31, false);
        break;
    case R_I2C_IF4:
        R_PMA_CPG_SetModuleStopStat(R_CPG_REGIDX_9, R_CPG_BIT27, false);
        break;
    default:
        R_PRINT_Log("[loc_I2cClkEnable : max9286] I2C Unit is invalid.\r\n");
        break;
    }
}

static uint32_t loc_RegWrite(r_i2c_Unit_t I2c, uint32_t SlaveAddr,
                uint8_t Addr, uint8_t Data)
{
    uint32_t read;
    int8_t   no_bytes = 2;
    uint8_t  byte[no_bytes];
    byte[0] = Addr;
    byte[1] = Data;
    delay_us(DELAY_US_RE_WR); /*This delay is necessary to ensure the stability of I2C communication*/

    loc_I2cReset(I2c);
    read = R_I2C_Write(I2c, (SlaveAddr >> 1), &byte[0], no_bytes);

    if(read == no_bytes) { /* OK */
        return 0;
    } else {
        R_PRINT_Log("[loc_RegWrite : max9286] I2C Unit is invalid.\r\n");
        return 1;
    }
}

static uint32_t loc_RegRead(r_i2c_Unit_t I2c, uint32_t SlaveAddr,
                uint8_t Addr, uint8_t *Data)
{
    uint32_t read = -1;
    uint8_t  byte = 0;
    loc_I2cReset(I2c);
    read =  R_I2C_ReadRegMap(I2c, (SlaveAddr >> 1), Addr, &byte, 1);
    if(read == 1) { /* OK */
        *Data = byte;
        return 0;
    } else {
        R_PRINT_Log("[loc_RegRead : max9286] R_I2C_ReadRegMap return value(%u) is invalid. :  Failed(1)\r\n",read);
        return 1;
    }
}

int reg8_write(uint8_t slAddr, uint8_t reg, uint8_t val)
{
    uint8_t retries;
    int ret;
    for (retries = REG8_NUM_RETRIES; retries; retries--) {
        ret = (int)loc_RegWrite(max9286_i2c_ch, slAddr, reg, val);;
        if (ret == 0)
            break;
        delay_us(DELAY_US_RE_WR); /* This delay is necessary to ensure the stability of I2C communication */
    }
#if 0
    if(ret !=0)
        R_PRINT_Log("reg8 write fail slAddr:%x reg:%x val:%x\r\n", slAddr, reg, val);
#endif
    return ret;
}

int reg8_read(uint8_t slAddr, uint8_t reg, uint8_t* val)
{
    uint8_t retries;
    int ret;
    for (retries = REG8_NUM_RETRIES; retries; retries--) {
        ret = (int)loc_RegRead(max9286_i2c_ch, slAddr, reg, val);
        if (ret == 0)
            break;
        delay_us(DELAY_US_RE_WR); /* This delay is necessary to ensure the stability of I2C communication */
    }
    return ret;
}
int max9286_reg8_write(uint8_t reg, uint8_t val)
{
    return reg8_write(MAX9286_I2C_ADDR, reg, val);
}

int max9286_reg8_read(uint8_t reg, uint8_t* val)
{
    return reg8_read(MAX9286_I2C_ADDR, reg, val);
}

int max9271_reg8_write(uint8_t reg, uint8_t val)
{
    return reg8_write(MAX9271_I2C_ADDR, reg, val);
}

int max9271_reg8_read(uint8_t reg, uint8_t* val)
{
    return reg8_read(MAX9271_I2C_ADDR, reg, val);
}

static uint8_t hweight8(uint8_t val)
{
    val = (val & 0x55555555) + ((val & 0xaaaaaaaa) >> 1);
    val = (val & 0x33333333) + ((val & 0xcccccccc) >> 2);
    val = (val & 0x0f0f0f0f) + ((val & 0xf0f0f0f0) >> 4);
    val = (val & 0x00ff00ff) + ((val & 0xff00ff00) >> 8);
    val = (val & 0x0000ffff) + ((val & 0xffff0000) >> 16);
    return val;
}

static void max9286_gmsl_link_setup(int idx)
{
    /* GMSL setup */
    /* MAX9271-CAMx I2C */
    max9271_reg8_write(0x0d, 0x22 | MAXIM_I2C_I2C_SPEED);  /* disable artificial ACK, I2C speed set */
    max9271_reg8_write(0x07, 0x84 ); /* RAW/YUV, PCLK edge, HS/VS encoding enabled */
    max9271_reg8_write(0x02, 0xff);  /* spread spectrum +-4%, pclk range automatic, Gbps automatic  */
    if (serializer_id == MAX96705_ID) {
        /* setup crossbar in DBL mode: reverse DVP bus */
        max9271_reg8_write(0x20, 0x07);
        max9271_reg8_write(0x21, 0x06);
        max9271_reg8_write(0x22, 0x05);
        max9271_reg8_write(0x23, 0x04);
        max9271_reg8_write(0x24, 0x03);
        max9271_reg8_write(0x25, 0x02);
        max9271_reg8_write(0x26, 0x01);
        max9271_reg8_write(0x27, 0x00);

        max9271_reg8_write(0x30, 0x17);
        max9271_reg8_write(0x31, 0x16);
        max9271_reg8_write(0x32, 0x15);
        max9271_reg8_write(0x33, 0x14);
        max9271_reg8_write(0x34, 0x13);
        max9271_reg8_write(0x35, 0x12);
        max9271_reg8_write(0x36, 0x11);
        max9271_reg8_write(0x37, 0x10);
    }

    /* MAX9286-CAMx I2C */
    max9286_reg8_write(0x34, 0x22 | MAXIM_I2C_I2C_SPEED);  /* disable artificial ACK, I2C speed set */
    /* I2C translator setup */
    /* MAX9271-CAMx I2C */
    max9271_reg8_write(0x0B, BROADCAST << 1);              /* broadcast I2C */
    max9271_reg8_write(0x0C, max9271_addr_map[idx]);       /* MAX9271-CAMx I2C new */
    /* I2C addresse change */
    max9271_reg8_write(0x01, MAX9286_I2C_ADDR);            /* MAX9286 I2C */
    max9271_reg8_write(0x00, max9271_addr_map[idx]);       /* MAX9271-CAM0 I2C new */
    /* delay_us(DELAY_US_CHANNEL_POWER_CTRL); */           /* wait 2ms */
    /* put MAX9271 in configuration link state  */
    reg8_write(max9271_addr_map[idx], 0x04, 0x43);         /* enable reverse_control/conf_link */
    /* delay_us(DELAY_US_CHANNEL_POWER_CTRL); */           /* wait 2ms */
}


static int max9286_reverse_channel_setup(int idx)
{

    uint8_t val = 0;
    int timeout = TIMEOUT;
    int ret = 0;
    max9286_reg8_write(0x34, 0xa2 | MAXIM_I2C_I2C_SPEED);    /* enable artificial ACKs, I2C speed set */
    max9286_reg8_write(0x00, 0xe0 | BIT(idx));   /* enable GMSL link for CAMx */
    max9286_reg8_write(0x0a, 0x11 << idx);       /* enable reverse control for CAMx */
    delay_us(DELAY_US_CHANNEL_POWER_CTRL);       /* wait 2ms after any change of reverse channel settings */
    for (;;) {
        max9286_reg8_write(0x3f, 0x4f);          /* enable custom reverse channel & first pulse length */
        max9286_reg8_write(0x3b, 0x1e);          /* first pulse length rise time changed from 300ns to 200ns,
                                                    amplitude 100mV */

        /* MAX9271-CAMx I2C */
        max9271_reg8_write(0x04, 0x43);          /* wake-up, enable reverse_control/conf_link */
        max9271_reg8_read(0x04, &val);           /* wake-up, enable reverse_control/conf_link */
        max9271_reg8_write(0x08, 0x01);          /* reverse channel receiver high threshold enable */
        max9271_reg8_write(0x97, 0x5f);          /* enable reverse control channel programming
                                                    (MAX96705-MAX96711 only) */
        delay_us(DELAY_US_CHANNEL_POWER_CTRL);
        /* MAX9286-CAMx I2C */
        max9286_reg8_write(0x3b, 0x19);          /* reverse channel increase amplitude 170mV to
                                                    compensate high threshold enabled */
        /* MAX9271-CAMx I2C */
        ret = max9271_reg8_read(0x1e, &val);     /* read max9271 ID */

        if (val == MAX9271_ID || val == MAX96705_ID || --timeout == 0) {
            serializer_id = val;
            break;
        }
        reg8_read(max9271_addr_map[idx], 0x1e, &val);
        if (val == MAX9271_ID || val == MAX96705_ID ) {
            serializer_id = val;
            reg8_write(max9271_addr_map[idx], 0x04, 0x43);
            R_PRINT_Log("link%d camera already serializer %s \r\n", idx, get_serializer_name(serializer_id));
            return 0;
        }
        if(timeout == TIMEOUT/2) {
            gpioSetOutput(4 ,13 ,0);
            delay_us(DELAY_US_CHANNEL_POWER_CTRL);
            gpioSetOutput(4 ,13 ,1);
        }

    }
    /* sensor reset/unreset */
    max9271_reg8_write(0xf, 0xfc);
    max9271_reg8_write(0xe, 0x42 | 0x2);

    if (!timeout) {
        ret = -1;
        R_PRINT_Log("link%d camera 0x%x not found timeout\r\n",idx, max9271_addr_map[idx]/2);
        return ret;
    }
    R_PRINT_Log("link%d %s at 0x%x\r\n", idx, get_serializer_name(serializer_id), max9271_addr_map[idx]/2);
    links_mask |= BIT(idx);
    csi2_outord &= ~(0x3 << (idx * 2));
    csi2_outord |= ((hweight8(links_mask) - 1) << (idx * 2));
    return ret;
}

static void max9286_postinit(void)
{
    int idx;
    for (idx =0; idx < CAMERA_LINKS; idx++) {
        max9286_reg8_write(0x00, 0xe0 | BIT(idx));
        max9286_reg8_write(0x0a, 0x11 << idx);
        delay_us(DELAY_US_CHANNEL_POWER_CTRL);
        reg8_write(max9271_addr_map[idx], 0xf, 0xfe);
        reg8_write(max9271_addr_map[idx], 0xe, 0x42);
    }
    max9286_reg8_write(0x0a, 0x00);              /* disable reverse control for all cams */
    max9286_reg8_write(0x00, 0xe0 | links_mask); /* enable GMSL link for CAMs */
    max9286_reg8_write(0x0b, csi2_outord);       /* CSI2 output order */
    max9286_reg8_write(0x15, 0x9b);              /* enable CSI output, VC is set accordingly to Link number,
                                                    BIT7 magic must be set */
    max9286_reg8_write(0x1b, links_mask);        /* enable equalizer for CAMs */
    max9286_reg8_write(0x34, 0x22| MAXIM_I2C_I2C_SPEED);
    max9286_reg8_write(0x01, 0x02);
}

static int max9286_registered_async(int idx)
{
    max9286_reg8_write(0x0a, 0x11 << idx);           /* enable reverse/forward control for CAMx */
    reg8_write(max9271_addr_map[idx], 0x04, 0x83);   /* enable serial_link */
    max9286_reg8_write(0x0a, (links_mask << 4) | links_mask); /* enable reverse/forward control for all CAMs */
    return 0;
}


void R_MAX9286_Init()
{
    uint8_t id = 0;
    uint8_t links = CAMERA_LINKS;
    uint8_t idx = 0;
    csi2_outord = 0xff;
    int ret = 0;
    loc_I2cClkEnable(max9286_i2c_ch);

    /*Set POC Pin*/
    pfcSetGPIO(4, 13 ,1);
    gpioSetOutput(4 ,13 ,1);

    max9286_reg8_read(0x1e, &id);
    if(id != MAX9286_ID)
        return;
    /*
     * Ensure that both the forward and reverse channel are disabled on the
     * mux, and that the channel ID is invalidated to ensure we reconfigure
     * on the next select call.
     */

    max9286_reg8_write(0x0a, 0x00);        /* disable reverse control for all cams */
    max9286_reg8_write(0x00, 0x00);        /* disable all GMSL links [0:3] */
    delay_us(DELAY_US_CHANNEL_POWER_CTRL); /* wait 2ms after any change of reverse channel settings */
    max9286_reg8_write(0x1b,0);
    max9286_reg8_write(0x1c,0x04);         /* high-immunity or legacy mode */

    max9286_reg8_write(0x15, 0x13);        /* disable CSI output, VC is set accordingly to Link number */
    max9286_reg8_write(0x69, 0x0f);        /* mask CSI forwarding from all links */
#if 0
    switch(links) {
    case 1:
        max9286_reg8_write(0x12, 0x33);    /* enable CSI-2 Lanes D0, DBL mode, YUV422 8-bit*/
        break;
    case 2:
        max9286_reg8_write(0x12, 0x73);    /* enable CSI-2 Lanes D0-D1, DBL mode, YUV422 8-bit*/
        break;
    case 3:
        max9286_reg8_write(0x12, 0xd3);    /* enable CSI-2 Lanes D0-D2, DBL mode, YUV422 8-bit*/
        break;
    case 4:
        max9286_reg8_write(0x12, 0xf3);    /* enable CSI-2 Lanes D0-D3, DBL mode, YUV422 8-bit*/
        break;
    default:
        return;
    }
#else
    max9286_reg8_write(0x12, 0xf3);
#endif
    max9286_reg8_write(0x01, 0xc0);
    max9286_reg8_write(0x06, 0x0);
    max9286_reg8_write(0x07, 0xd4);
    max9286_reg8_write(0x08, 0x30);

    max9286_reg8_write(0x63, 0);             /* disable overlap window */
    max9286_reg8_write(0x64, 0);
    max9286_reg8_write(0x0c, 0x99);
    max9286_reg8_write(0x19, 0xc);
    for (idx = 0; idx < links; idx++) {
        ret = max9286_reverse_channel_setup(idx);
        if (ret)
            continue;
        max9286_gmsl_link_setup(idx);
    }
    max9286_postinit();
    return;
}


void R_MAX9286_Start(void)
{
    int idx = 0;
    for(idx = 0; idx < CAMERA_LINKS; idx++)
    {
        if(BIT(idx) & links_mask)
            max9286_registered_async(idx);
    }
    return;
}

void R_MAX9286_PowerOn(void)
{
    max9286_reg8_write(0x69, links_mask ^ 0x0f);
}

void R_MAX9286_PowerOff(void)
{
    max9286_reg8_write(0x69, 0x0f);
}

void R_MAX9286_Stop(void)
{
    max9286_reg8_write(0x69, 0x0f);
    return;
}

int R_GET_MAX9286_Ch(void)
{
    return max9286_i2c_ch;
}

