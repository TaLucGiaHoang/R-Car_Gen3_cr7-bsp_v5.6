/*************************************************************************************************************
* i2c_ak4613_h
* Copyright (c) [2020-2022] Renesas Electronics Corporation
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_AK4613_H_
#define R_AK4613_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint32_t ak4613_init(uint32_t mode);
void StartCodec(void);
void StopCodec(void);

#ifdef __cplusplus
}
#endif

#endif /* R_AK4613_H_ */