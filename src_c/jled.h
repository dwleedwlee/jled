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

#ifndef __JLED_H__
#define __JLED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "hal_pwm.h"
#include "evaluator.h"

typedef enum {
  JLED_RES_LED0 = 0,
  JLED_RES_LED1,
  JLED_RES_LED2,
  JLED_RES_LED3,
  JLED_RES_MAX
}jledRes_t;

typedef enum {
  JLED_ST_STOPPED = 0,
  JLED_ST_RUNNING,
  JLED_ST_IN_DELAY_AFTER_PHASE
}jledStatus_t;

typedef struct Jled {
  brightnessEvaluator_t *brightnessEval;
  brightnessEvaluatorUnion_t brightnessEvalBuf;
  const _HalPwm_t *pHal;
  
  jledStatus_t state;
  uint8_t maxBrightness;
  uint16_t numRepetitions;
  uint32_t lastUpdateTime;
  uint32_t timeStart;
  uint16_t delayBefore;
  uint16_t delayAfter;
}jled_t;

jled_t *JLED_Init(jled_t *led, const _HalPwm_t *pHal);
jled_t *JLED_GetStaticJledFromIndex(jledRes_t idx);
bool JLED_Update(jled_t *led);

jled_t *JLED_Set(jled_t *led, uint8_t brightness);
jled_t *JLED_On(jled_t *led);
jled_t *JLED_Off(jled_t *led);
jled_t *JLED_FadeOn(jled_t *led, uint16_t duration);
jled_t *JLED_FadeOff(jled_t *led, uint16_t duration);
jled_t *JLED_Breathe(jled_t *led, uint16_t period);
jled_t *JLED_Blink(jled_t *led, uint16_t durationOn, uint16_t durationOff);
jled_t *JLED_Candle(jled_t *led, uint8_t speed, uint8_t jitter, uint16_t period);
jled_t *JLED_UserFunc(jled_t *led, brightnessEvaluator_t *userEval);
jled_t *JLED_Repeat(jled_t *led, uint16_t numRepetitions);
jled_t *JLED_Forever(jled_t *led);
bool JLED_IsForever(jled_t *led);
jled_t *JLED_DelayBefore(jled_t *led, uint16_t delayBefore);
jled_t *JLED_DelayAfter(jled_t *led, uint16_t delayAfter);
jled_t *JLED_Stop(jled_t *led);
bool IsRunning(jled_t *led);
jled_t *JLED_Reset(jled_t *led);
jled_t *JLED_MaxBrightness(jled_t *led, uint8_t level);
uint8_t JLED_GetMaxBrightness(jled_t *led);

bool JLED_UpdateL(jled_t *led, uint32_t now);

#ifdef __cplusplus
}
#endif

#endif
  
