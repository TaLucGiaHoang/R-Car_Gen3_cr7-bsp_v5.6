/*
 * Autosar OSAL glue code.
 *
 * Autosar only supports static creation of tasks, events, alarm, etc.
 * In order to work with systems that use dynamic tasks, we use a pool of tasks
 * with different priority levels.
 * In order to allow applications to make a task sleep, each Autosar task has an
 * event and an alarm.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "tpl_os.h"

#include "autosar_osal.h"

/* TODO these priorities may need tweaking depending on the app */
#define HIGH_PRI_TASK_THRESHOLD   10
#define MEDIUM_PRI_TASK_THRESHOLD 5

/* This function to run a thread inside an Autosar task is implemented in OSAL */
void R_OSAL_Sys_TaskRunner(void);

/* main() is called by the std C lib crt0.S from _start.
 * However, we need to call Trampoline functions from a task, not plain old main(),
 * so we use main() to call R_OSAL_Initialize() which starts the scheduler and
 * starts the one and only task that is set to AUTOSTART = TRUE, maintask.
 * maintask then calls what would have been the main() function, renamed here.
 */
int loc_MainThread(void* arg);

DeclareTask(maintask);
DeclareAlarm(maintask_sleep_alarm);
TASK(maintask) {
    loc_MainThread((void*)0);
}

DeclareTask(high1);
DeclareAlarm(task_high1_sleep_alarm);
TASK(high1) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(high2);
DeclareAlarm(task_high2_sleep_alarm);
TASK(high2) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(high3);
DeclareAlarm(task_high3_sleep_alarm);
TASK(high3) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(high4);
DeclareAlarm(task_high4_sleep_alarm);
TASK(high4) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(high5);
DeclareAlarm(task_high5_sleep_alarm);
TASK(high5) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(high6);
DeclareAlarm(task_high6_sleep_alarm);
TASK(high6) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(high7);
DeclareAlarm(task_high7_sleep_alarm);
TASK(high7) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(high8);
DeclareAlarm(task_high8_sleep_alarm);
TASK(high8) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(high9);
DeclareAlarm(task_high9_sleep_alarm);
TASK(high9) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(high10);
DeclareAlarm(task_high10_sleep_alarm);
TASK(high10) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(high11);
DeclareAlarm(task_high11_sleep_alarm);
TASK(high11) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(high12);
DeclareAlarm(task_high12_sleep_alarm);
TASK(high12) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(medium1);
DeclareAlarm(task_medium1_sleep_alarm);
TASK(medium1) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(medium2);
DeclareAlarm(task_medium2_sleep_alarm);
TASK(medium2) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(medium3);
DeclareAlarm(task_medium3_sleep_alarm);
TASK(medium3) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(medium4);
DeclareAlarm(task_medium4_sleep_alarm);
TASK(medium4) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(medium5);
DeclareAlarm(task_medium5_sleep_alarm);
TASK(medium5) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(medium6);
DeclareAlarm(task_medium6_sleep_alarm);
TASK(medium6) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(medium7);
DeclareAlarm(task_medium7_sleep_alarm);
TASK(medium7) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(medium8);
DeclareAlarm(task_medium8_sleep_alarm);
TASK(medium8) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low1);
DeclareAlarm(task_low1_sleep_alarm);
TASK(low1) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low2);
DeclareAlarm(task_low2_sleep_alarm);
TASK(low2) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low3);
DeclareAlarm(task_low3_sleep_alarm);
TASK(low3) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low4);
DeclareAlarm(task_low4_sleep_alarm);
TASK(low4) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low5);
DeclareAlarm(task_low5_sleep_alarm);
TASK(low5) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low6);
DeclareAlarm(task_low6_sleep_alarm);
TASK(low6) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low7);
DeclareAlarm(task_low7_sleep_alarm);
TASK(low7) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low8);
DeclareAlarm(task_low8_sleep_alarm);
TASK(low8) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low9);
DeclareAlarm(task_low9_sleep_alarm);
TASK(low9) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low10);
DeclareAlarm(task_low10_sleep_alarm);
TASK(low10) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low11);
DeclareAlarm(task_low11_sleep_alarm);
TASK(low11) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low12);
DeclareAlarm(task_low12_sleep_alarm);
TASK(low12) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low13);
DeclareAlarm(task_low13_sleep_alarm);
TASK(low13) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low14);
DeclareAlarm(task_low14_sleep_alarm);
TASK(low14) {
    R_OSAL_Sys_TaskRunner();
}

DeclareTask(low15);
DeclareAlarm(task_low15_sleep_alarm);
TASK(low15) {
    R_OSAL_Sys_TaskRunner();
}

/* Table must list tasks in order of priority */
osal_autosar_thread_t osal_autosar_thread_table[] = {
    /* maintask is a special task that is automatically started and used to run main() */
    { &maintask, &maintask_sleep_alarm, 255, "maintask", (void *)-1, NULL, NULL, NULL },
    { &high1, &task_high1_sleep_alarm, HIGH_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &high2, &task_high2_sleep_alarm, HIGH_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &high3, &task_high3_sleep_alarm, HIGH_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &high4, &task_high4_sleep_alarm, HIGH_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &high5, &task_high5_sleep_alarm, HIGH_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &high6, &task_high6_sleep_alarm, HIGH_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &high7, &task_high7_sleep_alarm, HIGH_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &high8, &task_high8_sleep_alarm, HIGH_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &high9, &task_high9_sleep_alarm, HIGH_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &high10, &task_high10_sleep_alarm, HIGH_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &high11, &task_high11_sleep_alarm, HIGH_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &high12, &task_high12_sleep_alarm, HIGH_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &medium1, &task_medium1_sleep_alarm, MEDIUM_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &medium2, &task_medium2_sleep_alarm, MEDIUM_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &medium3, &task_medium3_sleep_alarm, MEDIUM_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &medium4, &task_medium4_sleep_alarm, MEDIUM_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &medium5, &task_medium5_sleep_alarm, MEDIUM_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &medium6, &task_medium6_sleep_alarm, MEDIUM_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &medium7, &task_medium7_sleep_alarm, MEDIUM_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &medium8, &task_medium8_sleep_alarm, MEDIUM_PRI_TASK_THRESHOLD, "", NULL, NULL, NULL, NULL },
    { &low1, &task_low1_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
    { &low2, &task_low2_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
    { &low3, &task_low3_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
    { &low4, &task_low4_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
    { &low5, &task_low5_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
    { &low6, &task_low6_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
    { &low7, &task_low7_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
    { &low8, &task_low8_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
    { &low9, &task_low9_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
    { &low10, &task_low10_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
    { &low11, &task_low11_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
    { &low12, &task_low12_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
    { &low13, &task_low13_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
    { &low14, &task_low14_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
    { &low15, &task_low15_sleep_alarm, 0, "", NULL, NULL, NULL, NULL },
};

osal_autosar_thread_t *R_OSAL_Sys_GetThread(int index)
{
    return &osal_autosar_thread_table[index];
}

int R_OSAL_Sys_GetNrThreads(void)
{
    return sizeof(osal_autosar_thread_table) / sizeof(osal_autosar_thread_table[0]);
}


void R_OSAL_Sys_IrqHandler(int IntId);


ISR(dave_hd_sync)
{
    R_OSAL_Sys_IrqHandler(131);
}

FUNC(void, OS_CODE) DAVE_HD_SYNC_ClearFlag(void)
{
    Irq_Disable(131);
}

ISR(vin_ch4)
{
    R_OSAL_Sys_IrqHandler(206);
}

FUNC(void, OS_CODE) VIN_CH4_ClearFlag(void)
{
    Irq_Disable(206);
}

ISR(vin_ch5)
{
    R_OSAL_Sys_IrqHandler(207);
}

FUNC(void, OS_CODE) VIN_CH5_ClearFlag(void)
{
    Irq_Disable(207);
}

ISR(vin_ch0)
{
    R_OSAL_Sys_IrqHandler(220);
}

FUNC(void, OS_CODE) VIN_CH0_ClearFlag(void)
{
    Irq_Disable(220);
}

ISR(mfis_ariicr_ch0)
{
    R_OSAL_Sys_IrqHandler(256);
}

FUNC(void, OS_CODE) MFIS_ARIICR_CH0_ClearFlag(void)
{
    Irq_Disable(256);
}

ISR(vsp2_vspd_ch0)
{
    R_OSAL_Sys_IrqHandler(498);
}

FUNC(void, OS_CODE) VSP2_VSPD_CH0_ClearFlag(void)
{
    Irq_Disable(498);
}

ISR(vsp2_vspd_ch1)
{
    R_OSAL_Sys_IrqHandler(499);
}

FUNC(void, OS_CODE) VSP2_VSPD_CH1_ClearFlag(void)
{
    Irq_Disable(499);
}

ISR(vsp2_vspd_ch2)
{
    R_OSAL_Sys_IrqHandler(500);
}

FUNC(void, OS_CODE) VSP2_VSPD_CH2_ClearFlag(void)
{
    Irq_Disable(500);
}

ISR(ethernetavb_if_ch0)
{
    R_OSAL_Sys_IrqHandler(71);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH0_ClearFlag(void)
{
    Irq_Disable(71);
}

ISR(ethernetavb_if_ch1)
{
    R_OSAL_Sys_IrqHandler(72);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH1_ClearFlag(void)
{
    Irq_Disable(72);
}

ISR(ethernetavb_if_ch2)
{
    R_OSAL_Sys_IrqHandler(73);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH2_ClearFlag(void)
{
    Irq_Disable(73);
}

ISR(ethernetavb_if_ch3)
{
    R_OSAL_Sys_IrqHandler(74);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH3_ClearFlag(void)
{
    Irq_Disable(74);
}

ISR(ethernetavb_if_ch4)
{
    R_OSAL_Sys_IrqHandler(75);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH4_ClearFlag(void)
{
    Irq_Disable(75);
}

ISR(ethernetavb_if_ch5)
{
    R_OSAL_Sys_IrqHandler(76);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH5_ClearFlag(void)
{
    Irq_Disable(76);
}

ISR(ethernetavb_if_ch6)
{
    R_OSAL_Sys_IrqHandler(77);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH6_ClearFlag(void)
{
    Irq_Disable(77);
}

ISR(ethernetavb_if_ch7)
{
    R_OSAL_Sys_IrqHandler(78);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH7_ClearFlag(void)
{
    Irq_Disable(78);
}

ISR(ethernetavb_if_ch8)
{
    R_OSAL_Sys_IrqHandler(79);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH8_ClearFlag(void)
{
    Irq_Disable(79);
}

ISR(ethernetavb_if_ch9)
{
    R_OSAL_Sys_IrqHandler(80);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH9_ClearFlag(void)
{
    Irq_Disable(80);
}

ISR(ethernetavb_if_ch10)
{
    R_OSAL_Sys_IrqHandler(81);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH10_ClearFlag(void)
{
    Irq_Disable(81);
}

ISR(ethernetavb_if_ch11)
{
    R_OSAL_Sys_IrqHandler(82);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH11_ClearFlag(void)
{
    Irq_Disable(82);
}

ISR(ethernetavb_if_ch12)
{
    R_OSAL_Sys_IrqHandler(83);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH12_ClearFlag(void)
{
    Irq_Disable(83);
}

ISR(ethernetavb_if_ch13)
{
    R_OSAL_Sys_IrqHandler(84);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH13_ClearFlag(void)
{
    Irq_Disable(84);
}

ISR(ethernetavb_if_ch14)
{
    R_OSAL_Sys_IrqHandler(85);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH14_ClearFlag(void)
{
    Irq_Disable(85);
}

ISR(ethernetavb_if_ch15)
{
    R_OSAL_Sys_IrqHandler(86);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH15_ClearFlag(void)
{
    Irq_Disable(86);
}

ISR(ethernetavb_if_ch16)
{
    R_OSAL_Sys_IrqHandler(87);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH16_ClearFlag(void)
{
    Irq_Disable(87);
}

ISR(ethernetavb_if_ch17)
{
    R_OSAL_Sys_IrqHandler(88);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH17_ClearFlag(void)
{
    Irq_Disable(88);
}

ISR(ethernetavb_if_ch18)
{
    R_OSAL_Sys_IrqHandler(89);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH18_ClearFlag(void)
{
    Irq_Disable(89);
}

ISR(ethernetavb_if_ch19)
{
    R_OSAL_Sys_IrqHandler(90);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH19_ClearFlag(void)
{
    Irq_Disable(90);
}

ISR(ethernetavb_if_ch20)
{
    R_OSAL_Sys_IrqHandler(91);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH20_ClearFlag(void)
{
    Irq_Disable(91);
}

ISR(ethernetavb_if_ch21)
{
    R_OSAL_Sys_IrqHandler(92);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH21_ClearFlag(void)
{
    Irq_Disable(92);
}

ISR(ethernetavb_if_ch22)
{
    R_OSAL_Sys_IrqHandler(93);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH22_ClearFlag(void)
{
    Irq_Disable(93);
}

ISR(ethernetavb_if_ch23)
{
    R_OSAL_Sys_IrqHandler(94);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH23_ClearFlag(void)
{
    Irq_Disable(94);
}

ISR(ethernetavb_if_ch24)
{
    R_OSAL_Sys_IrqHandler(95);
}

FUNC(void, OS_CODE) ETHERNETAVB_IF_CH24_ClearFlag(void)
{
    Irq_Disable(95);
}
