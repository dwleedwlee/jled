/*
  The MIT License (MIT)
  
  Copyright (c) 2022 Dongwook Lee (elight.lee@gmail.com)
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __HAL_PWM_H__
#define __HAL_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  HAL_PWM_RES_LED0 = 0,
  HAL_PWM_RES_LED1,
  HAL_PWM_RES_LED2,
  HAL_PWM_RES_LED3,
  HAL_PWM_RES_MAX
}halPwmRes_t;

typedef struct _HalPwm{
  uint32_t (*pwmWrite)(uint16_t pin, uint16_t duty);
  uint16_t pin;
}_HalPwm_t;

uint32_t HAL_Ticks(void);
const _HalPwm_t *const HalPwm_Init(halPwmRes_t idx);
const _HalPwm_t *const HalPwm_GetStaticFromIndex(halPwmRes_t idx);
void HalPwm_AnalogWrite(const _HalPwm_t *const hal, uint8_t val);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_PWM_H__ */
