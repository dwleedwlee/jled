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

#ifndef __EVALUATOR_H__
#define __EVALUATOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

extern const uint8_t kFullBrightness;
extern const uint8_t kZeroBrightness;

/* Interface */
// a function f(t,period,param) that calculates the LEDs brightness for a given
// point in time and the given period. param is an optionally user provided
// parameter. t will always be in range [0..period-1].
// f(period-1,period,param) will be called last to calculate the final state of
// the LED.
typedef struct BrightnessEvaluator brightnessEvaluator_t;

struct BrightnessEvaluator {
  uint16_t (*period)(brightnessEvaluator_t *);
  uint8_t (*eval)(brightnessEvaluator_t *, uint32_t);
};

/* Concrete objects */
typedef struct ConstantBrightnessEvaluator {
  brightnessEvaluator_t brEval;
  uint8_t val;
} constantBrightnessEvaluator_t;

typedef struct BlinkBrightnessEvaluator {
  brightnessEvaluator_t brEval;
  uint16_t duration_on, duration_off;
} blinkBrightnessEvaluator_t;

typedef struct FadeOnBrightnessEvaluator {
  brightnessEvaluator_t brEval;
  uint16_t period;
} fadeOnBrightnessEvaluator_t;

typedef struct FadeOffBrightnessEvaluator {
  brightnessEvaluator_t brEval;
  uint16_t period;
} fadeOffBrightnessEvaluator_t;

typedef struct BreatheBrightnessEvaluator {
  brightnessEvaluator_t brEval;
  uint16_t period;
} breatheBrightnessEvaluator_t;

typedef struct AdvBreatheBrightnessEvaluator {
  brightnessEvaluator_t brEval;
  uint16_t duration_fade_on;
  uint16_t duration_on;
  uint16_t duration_fade_off;
} advBreatheBrightnessEvaluator_t;

typedef struct CandleBrightnessEvaluator {
  brightnessEvaluator_t brEval;
  uint16_t period;
  
  uint8_t speed;
  uint8_t jitter;
  uint8_t last;
  uint32_t last_t;
} candleBrightnessEvaluator_t;

typedef union {
  brightnessEvaluator_t brevIF;
  
  constantBrightnessEvaluator_t consBrev;
  blinkBrightnessEvaluator_t blinkBrev;
  fadeOnBrightnessEvaluator_t fadeonBrev;
  fadeOffBrightnessEvaluator_t fadeoffBrev;
  breatheBrightnessEvaluator_t breatheBrev;
  advBreatheBrightnessEvaluator_t advBreatheBrev;
  candleBrightnessEvaluator_t candleBrev;
} brightnessEvaluatorUnion_t;

void constantBrightnessInit(constantBrightnessEvaluator_t *pThis, uint8_t val);
void blinkBrightnessInit(blinkBrightnessEvaluator_t *pThis, uint16_t durOn, uint16_t durOff);
void fadeOnBrightnessInit(fadeOnBrightnessEvaluator_t *pThis, uint16_t period);
void fadeOffBrightnessInit(fadeOffBrightnessEvaluator_t *pThis, uint16_t period);
void breatheBrightnessInit(breatheBrightnessEvaluator_t *pThis, uint16_t period);
void advBreatheBrightnessInit(advBreatheBrightnessEvaluator_t *pThis, uint16_t duration_fade_on,
    uint16_t duration_on, uint16_t duration_fade_off);
void candleBrightnessInit(candleBrightnessEvaluator_t *pThis, uint8_t speed, uint8_t jitter, uint16_t period);

#ifdef __cplusplus
}
#endif

#endif /* __EVALUATOR_H__ */
