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

#include "evaluator.h"

const uint8_t kFullBrightness = 255;
const uint8_t kZeroBrightness = 0;

static uint8_t kFadeOnTable[] = {0, 3, 13, 33, 68, 118, 179, 232, 255};

// https://www.wolframalpha.com/input/?i=plot+(exp(sin((x-100%2F2.)*PI%2F100))-0.36787944)*108.0++x%3D0+to+100
// The fade-on func is an approximation of
//   y(x) = exp(sin((t-period/2.) * PI / period)) - 0.36787944) * 108.)
static uint8_t fadeon_func(uint32_t t, uint16_t period) {
  if(t + 1 >= period) return kFullBrightness;
  
  //approximate by linear interpolation.
  //scale t according to period to 0..255
  t = ((t << 8) / period) & 0xff;
  const uint32_t i = (t >> 5);  // -> i will be in range 0 .. 7
  const uint32_t y0 = kFadeOnTable[i];
  const uint32_t y1 = kFadeOnTable[i + 1];
  const uint32_t x0 = i << 5;   // *32
  
  // y(t) = mt+b, with m = dy/dx = (y1-y0)/32 = (y1-y0) >> 5
  return (uint8_t)((((t - x0) * (y1 - y0) >> 5) + y0));
}

static uint32_t rand_ = 0;

static void rand_seed(uint32_t seed) { rand_ = seed; }

static uint8_t rand8() {
  if (rand_ & 1) {
    rand_ = (rand_ >> 1);
  } else {
    rand_ = (rand_ >> 1) ^ 0x7FFFF159;
  }
  
  return (uint8_t)rand_;
}

static uint16_t constantBrightnessPeriod(constantBrightnessEvaluator_t *pThis) {
  return 1;
}

static uint8_t constantBrightnessEval(constantBrightnessEvaluator_t *pThis, uint32_t t) {
  return pThis->val;
}

void constantBrightnessInit(constantBrightnessEvaluator_t *pThis, uint8_t val) {
  pThis->brEval.period = (uint16_t (*)(brightnessEvaluator_t *))constantBrightnessPeriod;
  pThis->brEval.eval = (uint8_t (*) (brightnessEvaluator_t *, uint32_t))constantBrightnessEval;
  
  pThis->val = val;
}


static uint16_t blinkBrightnessPeriod(blinkBrightnessEvaluator_t *pThis) {
  return pThis->duration_on + pThis->duration_off;
}

static uint8_t blinkBrightnessEval(blinkBrightnessEvaluator_t *pThis, uint32_t t) {
  return (t < (pThis->duration_on)) ? kFullBrightness : kZeroBrightness;
}

void blinkBrightnessInit(blinkBrightnessEvaluator_t *pThis, uint16_t durOn, uint16_t durOff) {
  pThis->brEval.period = (uint16_t (*)(brightnessEvaluator_t *))blinkBrightnessPeriod;
  pThis->brEval.eval = (uint8_t (*) (brightnessEvaluator_t *, uint32_t))blinkBrightnessEval;
  
  pThis->duration_on = durOn;
  pThis->duration_off = durOff;
}


static uint16_t fadeOnBrightnessPeriod(fadeOnBrightnessEvaluator_t *pThis) {
  return pThis->period;
}

static uint8_t fadeOnBrightnessEval(fadeOnBrightnessEvaluator_t *pThis, uint32_t t) {
  return fadeon_func(t, pThis->period);
}

void fadeOnBrightnessInit(fadeOnBrightnessEvaluator_t *pThis, uint16_t period) {
  pThis->brEval.period = (uint16_t (*)(brightnessEvaluator_t *))fadeOnBrightnessPeriod;
  pThis->brEval.eval = (uint8_t (*) (brightnessEvaluator_t *, uint32_t))fadeOnBrightnessEval;
  
  pThis->period = period;
}


static uint16_t fadeOffBrightnessPeriod(fadeOffBrightnessEvaluator_t *pThis) {
  return pThis->period;
}

static uint8_t fadeOffBrightnessEval(fadeOffBrightnessEvaluator_t *pThis, uint32_t t) {
  return fadeon_func(pThis->period - t, pThis->period);
}

void fadeOffBrightnessInit(fadeOffBrightnessEvaluator_t *pThis, uint16_t period) {
  pThis->brEval.period = (uint16_t (*)(brightnessEvaluator_t *))fadeOffBrightnessPeriod;
  pThis->brEval.eval = (uint8_t (*) (brightnessEvaluator_t *, uint32_t))fadeOffBrightnessEval;
  
  pThis->period = period;
}


static uint16_t breatheBrightnessPeriod(breatheBrightnessEvaluator_t *pThis) {
  return pThis->period;
}

static uint8_t breatheBrightnessEval(breatheBrightnessEvaluator_t *pThis, uint32_t t) {
  if ((t + 1) >= (pThis->period)) return kZeroBrightness;
  const uint16_t periodh = (pThis->period) >> 1;
  return (t < periodh) ? fadeon_func(t, periodh) : fadeon_func(pThis->period - t, periodh);
}

void breatheBrightnessInit(breatheBrightnessEvaluator_t *pThis, uint16_t period) {
  pThis->brEval.period = (uint16_t (*)(brightnessEvaluator_t *))breatheBrightnessPeriod;
  pThis->brEval.eval = (uint8_t (*) (brightnessEvaluator_t *, uint32_t))breatheBrightnessEval;
  
  pThis->period = period;
}


static uint16_t advBreatheBrightnessPeriod(advBreatheBrightnessEvaluator_t *pThis) {
  return pThis->duration_fade_on + pThis->duration_on + pThis->duration_fade_off;
}

static uint8_t advBreatheBrightnessEval(advBreatheBrightnessEvaluator_t *pThis, uint32_t t) {
  if (t < pThis->duration_fade_on)
    return fadeon_func(t, pThis->duration_fade_on);
  else if (t < pThis->duration_fade_on + pThis->duration_on)
    return kFullBrightness;
  else if (t < advBreatheBrightnessPeriod(pThis))
    return fadeon_func(advBreatheBrightnessPeriod(pThis) - t, pThis->duration_fade_off);
  else
    return kZeroBrightness;
}

void advBreatheBrightnessInit(advBreatheBrightnessEvaluator_t *pThis, uint16_t duration_fade_on,
    uint16_t duration_on, uint16_t duration_fade_off) {
  pThis->brEval.period = (uint16_t (*)(brightnessEvaluator_t *))advBreatheBrightnessPeriod;
  pThis->brEval.eval = (uint8_t (*) (brightnessEvaluator_t *, uint32_t))advBreatheBrightnessEval;
  
  pThis->duration_fade_on = duration_fade_on;
  pThis->duration_on = duration_on;
  pThis->duration_fade_off = duration_fade_off;
}


static uint16_t candleBrightnessPeriod(candleBrightnessEvaluator_t *pThis) {
  return pThis->period;
}

static uint8_t candleBrightnessEval(candleBrightnessEvaluator_t *pThis, uint32_t t) {
  // idea from
  // https://cpldcpu.wordpress.com/2013/12/08/hacking-a-candleflicker-led/
  // TODO(jd) finetune values
  static const uint8_t kCandleTable[] = {
  5, 10, 20, 30, 50, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 255};
  if ((t >> pThis->speed) == pThis->last_t) return pThis->last;
  pThis->last = (uint8_t)(t >> pThis->speed);
  const uint8_t rnd = rand8() & 255;
  pThis->last = (rnd >= pThis->jitter) ? 255 : (50 + kCandleTable[rnd & 0xf]);
  return pThis->last;
}

void candleBrightnessInit(candleBrightnessEvaluator_t *pThis, uint8_t speed, uint8_t jitter, uint16_t period) {
  pThis->brEval.period = (uint16_t (*)(brightnessEvaluator_t *))candleBrightnessPeriod;
  pThis->brEval.eval = (uint8_t (*) (brightnessEvaluator_t *, uint32_t))candleBrightnessEval;
  
  pThis->period = period;
  pThis->speed = speed;
  pThis->jitter = jitter;
  
  pThis->last = 5;
  pThis->last_t = 0;
}
