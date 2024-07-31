/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

/*
   Title: Cluster Application Module
 */

#define _APP_CL_CARMODEL_GLOBALS_
#include "r_typedefs.h"
#include "app_cl_carmodel.h"
#include "error.h"

/* Speed difference that is caused by one speed update tick. (in km/h) */
#define LOC_ACCELERATION_FACTOR    0.95

/* Maximum speed. */
#define LOC_CM_SPEED_MAX           240

/* Minimum speed. */
#define LOC_CM_SPEED_MIN           10

/* Medium speed. */
#define LOC_CM_SPEED_MID           50

/* Maximum rpm. */
#define LOC_CM_RPM_MAX             8000

/* Minimum rpm. */
#define LOC_CM_RPM_MIN             100

/* Shift down RPM. */
#define LOC_CM_GEARDOWN_RPM        2700

/* Shift down RPM. */
#define LOC_CM_GEARUP_RPM          5500

/* Max gear. */
#define LOC_CM_GEAR_MAX            5

/* Min gear. */
#define LOC_CM_GEAR_MIN            1


/* Translation factor speed to rpm. */
static const uint16_t locGearTrans[] = {
    200, 200, 150, 100, 50, 25
};


/* Update turn signal values. */
static void locUpdateTurnSignals(void)
{
    static uint32_t cc = 0;

    if (cc == 1) {
        CarData.Left = 1;
        CarData.Right = 0;
    } else if (cc == 500) {
        CarData.Left = 0;
        CarData.Right = 0;
    } else if (cc == 1500) {
        CarData.Left = 0;
        CarData.Right = 1;
    } else if (cc == 2000) {
        CarData.Left = 0;
        CarData.Right = 0;
    } else if (cc == 2500) {
        CarData.Left = 1;
        CarData.Right = 1;
    } else if (cc > 3000) {
        cc = 0;
    }
    cc++;
}

/* Update speed value. */
static void locUpdateSpeed(void)
{
    static int cc = 0;

    if (((CarData.Speed <= LOC_CM_SPEED_MIN)
         || ((CarData.Speed >= LOC_CM_SPEED_MID) && (CarData.Speed < LOC_CM_SPEED_MID + LOC_ACCELERATION_FACTOR))
         || (CarData.Speed >= LOC_CM_SPEED_MAX))
        && (cc < 100)) {
        cc++;
        if (cc < 100) {
            return;
        }
        cc = 0;
    }
    CarData.Speed += (LOC_ACCELERATION_FACTOR * (float)CarData.Direction);
    if (CarData.Speed > LOC_CM_SPEED_MAX) {
        CarData.Speed = LOC_CM_SPEED_MAX;
        CarData.Direction = -1;
    } else if (CarData.Speed <= LOC_CM_SPEED_MIN) {
        CarData.Speed = LOC_CM_SPEED_MIN;
        CarData.Direction = 1;
    }
}

/* Update RPM & Gear value. */
static void locUpdateRpm(void)
{
    do {
        CarData.Rpm = CarData.Speed * locGearTrans[CarData.Gear];
        if ((CarData.Rpm > LOC_CM_GEARUP_RPM) && (CarData.Direction > 0)) {
            if (CarData.Gear < LOC_CM_GEAR_MAX) {
                CarData.Gear++;
                CarData.Rpm = CarData.Speed * locGearTrans[CarData.Gear];
            }
        } else if ((CarData.Rpm < LOC_CM_GEARDOWN_RPM) && (CarData.Direction < 0)) {
            if (CarData.Gear > LOC_CM_GEAR_MIN) {
                CarData.Gear--;
                CarData.Rpm = CarData.Speed * locGearTrans[CarData.Gear];
            }
        }
    } while ((CarData.Rpm > LOC_CM_RPM_MAX) || (CarData.Rpm < LOC_CM_RPM_MIN));

}

/* Update low & high light */
static void locUpdateLight(void)
{
    static int cc = 0;

    if (cc < 100) {
        CarData.HighLight = 0;
        CarData.LowLight = 0;
    } else if (cc < 200) {
        CarData.HighLight = 0;
        CarData.LowLight = 1;
    } else if (cc < 250) {
        CarData.HighLight = 1;
        CarData.LowLight = 1;
    } else {
        cc = 0;
    }
    cc++;
}

/* Update fan */
static void locUpdateFan(void)
{
    static int cc = 0;

    if (cc < 200) {
        CarData.Fan = 0;
    } else if (cc < 300) {
        CarData.Fan = 1;
    } else {
        cc = 0;
    }
    cc++;
}

/* Update oil */
static void locUpdateOil(void)
{
    static int cc = 0;

    if (cc < 250) {
        CarData.Oil = 0;
    } else if (cc < 325) {
        CarData.Oil = 1;
    } else {
        cc = 0;
    }
    cc++;
}

/* Update battery */
static void locUpdateBattery(void)
{
    static int cc = 0;

    if (cc < 125) {
        CarData.Battery = 1;
    } else if (cc < 225) {
        CarData.Battery = 0;
    } else {
        cc = 0;
    }
    cc++;
}

/* Update airbag */
static void locUpdateAirbag(void)
{
    static int cc = 0;

    if (cc < 115) {
        CarData.Airbag = 1;
    } else if (cc < 1005) {
        CarData.Airbag = 0;
    } else {
        cc = 0;
    }
    cc++;
}

/* Update belt */
static void locUpdateBelt(void)
{
    static int cc = 0;

    if (cc < 415) {
        CarData.Belt = 1;
    } else if (cc < 705) {
        CarData.Belt = 0;
    } else {
        cc = 0;
    }
    cc++;
}

/* Update break */
static void locUpdateBreak(void)
{
    static int cc = 0;

    if (cc < 215) {
        CarData.Break = 1;
    } else if (cc < 305) {
        CarData.Break = 0;
    } else {
        cc = 0;
    }
    cc++;
}

/* Update windshield */
static void locUpdateWindshield(void)
{
    static int cc = 0;

    if (cc < 95) {
        CarData.Windshield = 1;
    } else if (cc < 285) {
        CarData.Windshield = 0;
    } else {
        cc = 0;
    }
    cc++;
}

/* Update wiping */
static void locUpdateWiping(void)
{
    static int cc = 0;

    if (cc < 625) {
        CarData.Wiping = 1;
    } else if (cc < 1305) {
        CarData.Wiping = 0;
    } else {
        cc = 0;
    }
    cc++;
}

/* Update cooling liquid  value. */
static void locUpdateCoolingLiquid(void)
{
    static int inc = 1;

    CarData.CoolingTemp += inc;
    if (CarData.CoolingTemp > 100) {
        inc = -1;
    } else if (CarData.CoolingTemp < 20) {
        inc = 1;
    }
}

/* Update fuel value. */
static void locUpdateFuel(void)
{
    static int cc = 0;

    cc++;
    if ((cc % 2) == 0) {
        if (CarData.FuelLevel == 0) {
            CarData.FuelLevel = 100;
        }
        CarData.FuelLevel--;
    }
}

/* Update gear selection. */
static void locUpdateGearSelect(void)
{
    static int cc = 0;

    cc++;
    if ((cc % 50) == 0) {
        CarData.GearSelect = CarData.GearSelect << 1;
        if (CarData.GearSelect > 16) {
            CarData.GearSelect = 1;
        }
    }
}

/*
   Car model thread

   The threads updates the car model data.
 */
void CarModelUpdate(void)
{
    locUpdateSpeed();
    locUpdateRpm();
    locUpdateTurnSignals();
    locUpdateLight();
    locUpdateFan();
    locUpdateOil();
    locUpdateBattery();
    locUpdateAirbag();
    locUpdateBelt();
    locUpdateBreak();
    locUpdateWindshield();
    locUpdateWiping();
    locUpdateCoolingLiquid();
    locUpdateFuel();
    locUpdateGearSelect();
}

/*
   Section: Global Functions

   See: <app_cl_carmodel.h>
 */

void InitCarModel(void)
{
    CarData.Speed = 30;
    CarData.Rpm = 25;
    CarData.Gear = 1;
    CarData.GearSelect = 1;
    CarData.Direction = 1;
}

void DeInitCarModel(void)
{
}

float CarModelSpeedGet(void)
{
    return CarData.Speed;
}

uint16_t CarModelRpmGet(void)
{
    return CarData.Rpm;
}

uint8_t CarModelLeftGet(void)
{
    return CarData.Left;
}

uint8_t CarModelRightGet(void)
{
    return CarData.Right;
}

uint8_t CarModelHighLightGet(void)
{
    return CarData.HighLight;
}

uint8_t CarModelLowLightGet(void)
{
    return CarData.LowLight;
}

uint8_t CarModelFanGet(void)
{
    return CarData.Fan;
}

uint8_t CarModelOilGet(void)
{
    return CarData.Oil;
}

uint8_t CarModelBatteryGet(void)
{
    return CarData.Battery;
}

uint8_t CarModelAirbagGet(void)
{
    return CarData.Airbag;
}

uint8_t CarModelBeltGet(void)
{
    return CarData.Belt;
}

uint8_t CarModelBreakGet(void)
{
    return CarData.Break;
}

uint8_t CarModelWindshieldGet(void)
{
    return CarData.Windshield;
}

uint8_t CarModelWipingGet(void)
{
    return CarData.Wiping;
}

uint8_t CarModelFuelEmptyGet(void)
{
    uint8_t x;

    if (CarData.FuelLevel > 10) {
        x = 0;
    } else {
        x = 1;
    }
    return x;
}

uint8_t CarModelCoolingHotGet(void)
{
    uint8_t x;

    if (CarData.CoolingTemp < 90) {
        x = 0;
    } else {
        x = 1;
    }
    return x;
}

uint8_t CarModelFuelLevelGet(void)
{
    return CarData.FuelLevel;
}

int8_t CarModelCoolingTempGet(void)
{
    return CarData.CoolingTemp;
}

uint8_t CarModelGearPGet(void)
{
    return CarData.GearSelect & 1;
}

uint8_t CarModelGearRGet(void)
{
    return CarData.GearSelect & 2;
}

uint8_t CarModelGearNGet(void)
{
    return CarData.GearSelect & 4;
}

uint8_t CarModelGearDGet(void)
{
    return CarData.GearSelect & 8;
}

uint8_t CarModelGearSGet(void)
{
    return CarData.GearSelect & 16;
}
