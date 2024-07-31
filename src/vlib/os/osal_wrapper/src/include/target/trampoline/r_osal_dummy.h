/* TODO: Add for iVDP1C dummy osal */

#ifndef OSAL_DUMMY_H
#define OSAL_DUMMY_H
#include "rcar-xos/osal/r_osal.h"

#ifdef __cplusplus
extern "C" {
#endif

void osal_dummy_power_on(void);
void osal_dummy_clock_enable(void);

#ifdef __cplusplus
}
#endif

#endif /* OSAL_DUMMY_H */

