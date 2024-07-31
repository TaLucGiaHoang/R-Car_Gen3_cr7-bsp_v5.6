/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#ifndef _APP_CL_CARMODEL_H_
#define _APP_CL_CARMODEL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
  Title: Car model interface
*/

#undef _EXTERN_
#ifndef _APP_CL_CARMODEL_GLOBALS_
#define _EXTERN_ extern 
#else
#define _EXTERN_
#endif

/*
  Section: Global Types
*/

/*
  Car parameter value type.

  Members:
  Rpm       - Rounds per minute
  Speed     - Speed in km/h
  Gear      - Gear
  Direction - Accelerate (1), Break (-1), Cruise (0)
  Left      - Turn signal left on/off
  Right     - Turn signal right on/off
*/

typedef struct {
    uint16_t Rpm;
    float   Speed;
    int8_t   Gear;
    int8_t   Direction;
    uint8_t  FuelLevel;
    int8_t   CoolingTemp;
    uint8_t  GearSelect;
    int      Left:1;
    int      Right:1;
    int      HighLight:1;
    int      LowLight:1;
    int      Fan:1;
    int      Oil:1;
    int      Battery:1;
    int      Airbag:1;
    int      Belt:1;
    int      Break:1;
    int      Windshield:1;
    int      Wiping:1;
} CarData_t;


/*
  Section: Global Variables
*/

/*
  Variable: CarData

  Current car parameter values.
*/

_EXTERN_ CarData_t CarData;


/*
  Section: Global Functions
*/

/*
  Initialize the car model.
*/
void InitCarModel(void);

/*
  Deinitialize the car model.
*/
void DeInitCarModel(void);

/*
  Get current speed.

  Returns: Speed in km/h
*/
float CarModelSpeedGet(void);

/*
  Get current rpm.
  
  Returns: rpm
*/
uint16_t CarModelRpmGet(void);

uint8_t CarModelLeftGet(void);

uint8_t CarModelRightGet(void);

uint8_t CarModelHighLightGet(void);

uint8_t CarModelLowLightGet(void);

uint8_t CarModelFanGet(void);

uint8_t CarModelOilGet(void);

uint8_t CarModelBatteryGet(void);

uint8_t CarModelAirbagGet(void);

uint8_t CarModelBeltGet(void);

uint8_t CarModelBreakGet(void);

uint8_t CarModelWindshieldGet(void);

uint8_t CarModelWipingGet(void);

uint8_t CarModelFuelEmptyGet(void);

uint8_t CarModelCoolingHotGet(void);

/*
  Get fuel level.

  Returns: Fuel level
*/
uint8_t CarModelFuelLevelGet(void);

/*
  Get cooling liquid temperature.

  Returns: Temperature
*/
int8_t CarModelCoolingTempGet(void);

/*
  Get status of gear indicator.
*/
uint8_t CarModelGearPGet(void);

/*
  Get status of gear indicator.
*/
uint8_t CarModelGearRGet(void);

/*
  Get status of gear indicator.
*/
uint8_t CarModelGearNGet(void);

/*
  Get status of gear indicator.
*/
uint8_t CarModelGearDGet(void);

/*
  Get status of gear indicator.
*/
uint8_t CarModelGearSGet(void);

/*
  Trigger the car model thread.
*/
void CarModelTriggerThread(void);


void CarModelUpdate(void);

#ifdef __cplusplus
}
#endif

#endif /* _APP_CL_CARMODEL_H_ */
