/* TODO: Add for iVDP1C dummy osal */

#include <inttypes.h>

#include "target/trampoline/r_osal_dummy.h"
#include "r_cpg_api.h"

/*********************
 * Common
 *********************/
 /* sysc */
#define OSAL_DMY_REG_SYSC_BASE			(0xe6180000u)
/* Power status register  9(A3VC) offset */
#define OSAL_DMY_REG_SYSC_PWRSR9		(0x00000380u)
/* Power resume control register  9(A3VC) offset */
#define OSAL_DMY_REG_SYSC_PWRONCR9		(0x0000038Cu)
/* Power status register 10(A2VC)	 offset */
#define OSAL_DMY_REG_SYSC_PWRSR10		(0x000003C0u)
/* Power resume control register 10(A2VC) offset */
#define OSAL_DMY_REG_SYSC_PWRONCR10		(0x000003CCu)
/* A3VC     */
#define OSAL_DMY_REG_SYSC_MASK_SR9		(0x00000010u)
#define OSAL_DMY_REG_SYSC_MASK_ONCR9	(0x00000001u)
/* A2VC0/A2VC1 */
#define OSAL_DMY_REG_SYSC_MASK_SR10		(0x00000008u)
#define OSAL_DMY_REG_SYSC_MASK_ONCR10	(0x00000002u)
/* Power on retry cnt */
#define OSAL_DMY_PWR_RETRY				(1000u)
 
 /* cpg */
#define OSAL_DMY_REG_CPG_BASE			(0xE6150000u)
#define OSAL_DMY_REG_CPG_WPCR        (0x0904) /* Write protect control register */
#define OSAL_DMY_REG_CPG_WPR         (0x0900) /* write protect register */

#define OSAL_DMY_REG_CPG_RMSTPCR0       (0x0110)
#define OSAL_DMY_REG_CPG_RMSTPCR1       (0x0114)
#define OSAL_DMY_REG_CPG_RMSTPCR2       (0x0118)
#define OSAL_DMY_REG_CPG_RMSTPCR3       (0x011C)
#define OSAL_DMY_REG_CPG_RMSTPCR4       (0x0120)
#define OSAL_DMY_REG_CPG_RMSTPCR5       (0x0124)
#define OSAL_DMY_REG_CPG_RMSTPCR6       (0x0128)
#define OSAL_DMY_REG_CPG_RMSTPCR7       (0x012C)
#define OSAL_DMY_REG_CPG_RMSTPCR8       (0x0980)
#define OSAL_DMY_REG_CPG_RMSTPCR9       (0x0984)
#define OSAL_DMY_REG_CPG_RMSTPCR10      (0x0988)
#define OSAL_DMY_REG_CPG_RMSTPCR11      (0x098C)

#define OSAL_DMY_VDP_CPG_REG			(1)
#define OSAL_DMY_VDP_CPG_BIT			(30)
#define OSAL_DMY_FCPC_CPG_REG			(6)
#define OSAL_DMY_FCPC_CPG_BIT			(19)
#define OSAL_DMY_IMR_CPG_REG			(8)
#define OSAL_DMY_IMR0_CPG_BIT			(23)
#define OSAL_DMY_IMR1_CPG_BIT			(22)
#define OSAL_DMY_IMR2_CPG_BIT			(21)
#define OSAL_DMY_IMR3_CPG_BIT			(20)

/* osal resource */
#define OSAL_DMY_MMNGR_NUM		4u

//static OSAL_DMY_DEVICE_IO_T sDevInfo[OSAL_DMY_DEVICE_NUM] = {
//  /* id,			type,		address,		range,			irq_number,			irq_channels */
//														 	 /* {vlc, ce,  cm} */
//	{"ivdp1c_00",	"ivdp1c",	0xFE8D0000U,	0x00001000U,	{412, 413, 255},	3	},
//	{"fcpc_00",		"fcpc",		0xFE90F000U,	0x00001000U,	{0},				0	}
//};

static const uint16_t loc_RmstpOffset[] = {
    OSAL_DMY_REG_CPG_RMSTPCR0,
    OSAL_DMY_REG_CPG_RMSTPCR1,
    OSAL_DMY_REG_CPG_RMSTPCR2,
    OSAL_DMY_REG_CPG_RMSTPCR3,
    OSAL_DMY_REG_CPG_RMSTPCR4,
    OSAL_DMY_REG_CPG_RMSTPCR5,
    OSAL_DMY_REG_CPG_RMSTPCR6,
    OSAL_DMY_REG_CPG_RMSTPCR7,
    OSAL_DMY_REG_CPG_RMSTPCR8,
    OSAL_DMY_REG_CPG_RMSTPCR9,
    OSAL_DMY_REG_CPG_RMSTPCR10,
    OSAL_DMY_REG_CPG_RMSTPCR11,
};

/************************************************************
 * Function
 ************************************************************/
static void osal_dummy_regwrite(uintptr_t Address, uint32_t Value)
{
    *((volatile uint32_t *)Address) = Value;
}

static uint32_t osal_dummy_regread(uintptr_t Address)
{
    return *((volatile uint32_t *)Address);
}

void osal_dummy_power_on(void)
{
	unsigned int	 lRegDat;
	unsigned int	 lCnt;

	/* A3VC power on setting */
	lRegDat = osal_dummy_regread(OSAL_DMY_REG_SYSC_BASE+OSAL_DMY_REG_SYSC_PWRSR9);
	if ((lRegDat & OSAL_DMY_REG_SYSC_MASK_SR9) != OSAL_DMY_REG_SYSC_MASK_SR9) {
		lRegDat = osal_dummy_regread(OSAL_DMY_REG_SYSC_BASE+OSAL_DMY_REG_SYSC_PWRONCR9);
		lRegDat |= OSAL_DMY_REG_SYSC_MASK_ONCR9;
		osal_dummy_regwrite(OSAL_DMY_REG_SYSC_BASE + OSAL_DMY_REG_SYSC_PWRONCR9, lRegDat);
		lCnt = 0u;
		do {
			lCnt++;
			lRegDat = osal_dummy_regread(OSAL_DMY_REG_SYSC_BASE+OSAL_DMY_REG_SYSC_PWRSR9);
		} while ((lCnt < OSAL_DMY_PWR_RETRY) &&
				 ((lRegDat & OSAL_DMY_REG_SYSC_MASK_SR9) != OSAL_DMY_REG_SYSC_MASK_SR9));
//		printf(" SYSC_PWRSR9[reg:0x%08x rval:0x%08x] - lCnt:%c \n", 
//				(OSAL_DMY_REG_SYSC_BASE + OSAL_DMY_REG_SYSC_PWRSR9), lRegDat, lCnt);
	} else {
//		printf(" SYSC_PWRSR9[reg:0x%08x rval:0x%08x] \n", 
//				(OSAL_DMY_REG_SYSC_BASE + OSAL_DMY_REG_SYSC_PWRSR9), lRegDat);
	}

	/* A2V0/1 power on setting */
	lRegDat = osal_dummy_regread(OSAL_DMY_REG_SYSC_BASE+OSAL_DMY_REG_SYSC_PWRSR10);
	if ((lRegDat & OSAL_DMY_REG_SYSC_MASK_SR10) != OSAL_DMY_REG_SYSC_MASK_SR10) {
		lRegDat = osal_dummy_regread(OSAL_DMY_REG_SYSC_BASE+OSAL_DMY_REG_SYSC_PWRONCR10);
		lRegDat |= OSAL_DMY_REG_SYSC_MASK_ONCR10;
		osal_dummy_regwrite(OSAL_DMY_REG_SYSC_BASE + OSAL_DMY_REG_SYSC_PWRONCR10, lRegDat);
		lCnt = 0u;
		do {
			lCnt++;
			lRegDat = osal_dummy_regread(OSAL_DMY_REG_SYSC_BASE+OSAL_DMY_REG_SYSC_PWRSR10);
		} while ((lCnt < OSAL_DMY_PWR_RETRY) &&
				 ((lRegDat & OSAL_DMY_REG_SYSC_MASK_SR10) !=
				 								OSAL_DMY_REG_SYSC_MASK_SR10));
//		printf(" SYSC_PWRSR10[reg:0x%08x rval:0x%08x] - lCnt:%d \n", 
//				(OSAL_DMY_REG_SYSC_BASE + OSAL_DMY_REG_SYSC_PWRSR10), lRegDat, lCnt);
	} else {
//		printf(" SYSC_PWRSR10[reg:0x%08x rval:0x%08x] \n",
//				(OSAL_DMY_REG_SYSC_BASE + OSAL_DMY_REG_SYSC_PWRSR10), lRegDat);
	}
}

static void loc_ProtectedRegClear(uintptr_t Addr, uint32_t Mask)
{
    uint32_t val;

    osal_dummy_regwrite(OSAL_DMY_REG_CPG_BASE + OSAL_DMY_REG_CPG_WPCR, 0xA5A50000); /* Clear register protection */

    val = osal_dummy_regread(Addr);            /* Generate value */
    val = val & Mask;
    osal_dummy_regwrite(OSAL_DMY_REG_CPG_BASE + OSAL_DMY_REG_CPG_WPR, ~val);       /* Unlock write */
    osal_dummy_regwrite(Addr, val);                /* Write value */

    if ((osal_dummy_regread(Addr) & ~Mask) != 0) {
        printf("CPG did not clear MSTP bits! (Addr 0x%x, Mask 0x%x, actual 0x%x\n", 
        	Addr, (unsigned int)Mask, (unsigned int)osal_dummy_regread(Addr));
    }
}
static void loc_ModuleEnable(int RegNr, uint32_t BitNr)
{
    loc_ProtectedRegClear(OSAL_DMY_REG_CPG_BASE + loc_RmstpOffset[RegNr], ~(1 << BitNr));
}

void osal_dummy_clock_enable(void)
{

	loc_ModuleEnable(OSAL_DMY_VDP_CPG_REG, OSAL_DMY_VDP_CPG_BIT);
	loc_ModuleEnable(OSAL_DMY_FCPC_CPG_REG, OSAL_DMY_FCPC_CPG_BIT);
	/* IMR CPG */
	loc_ModuleEnable(OSAL_DMY_IMR_CPG_REG, OSAL_DMY_IMR0_CPG_BIT);
	loc_ModuleEnable(OSAL_DMY_IMR_CPG_REG, OSAL_DMY_IMR1_CPG_BIT);
	loc_ModuleEnable(OSAL_DMY_IMR_CPG_REG, OSAL_DMY_IMR2_CPG_BIT);
	loc_ModuleEnable(OSAL_DMY_IMR_CPG_REG, OSAL_DMY_IMR3_CPG_BIT);
}


