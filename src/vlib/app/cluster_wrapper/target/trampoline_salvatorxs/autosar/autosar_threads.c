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
void start_thread(void *a_Arg);

DeclareTask(maintask);
DeclareAlarm(maintask_sleep_alarm);
TASK(maintask) {
    start_thread((void*)0);
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
}

ISR(vin_ch4)
{
    R_OSAL_Sys_IrqHandler(206);
}

FUNC(void, OS_CODE) VIN_CH4_ClearFlag(void)
{
}

ISR(vin_ch5)
{
    R_OSAL_Sys_IrqHandler(207);
}

FUNC(void, OS_CODE) VIN_CH5_ClearFlag(void)
{
}

ISR(vin_ch0)
{
    R_OSAL_Sys_IrqHandler(220);
}

FUNC(void, OS_CODE) VIN_CH0_ClearFlag(void)
{
}

ISR(mfis_ariicr_ch0)
{
    R_OSAL_Sys_IrqHandler(256);
}

FUNC(void, OS_CODE) MFIS_ARIICR_CH0_ClearFlag(void)
{
}

ISR(vsp2_vspd_ch0)
{
    R_OSAL_Sys_IrqHandler(498);
}

FUNC(void, OS_CODE) VSP2_VSPD_CH0_ClearFlag(void)
{
}

ISR(vsp2_vspd_ch1)
{
    R_OSAL_Sys_IrqHandler(499);
}

FUNC(void, OS_CODE) VSP2_VSPD_CH1_ClearFlag(void)
{
}

ISR(vsp2_vspd_ch2)
{
    R_OSAL_Sys_IrqHandler(500);
}

FUNC(void, OS_CODE) VSP2_VSPD_CH2_ClearFlag(void)
{
}