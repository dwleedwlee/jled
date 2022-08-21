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

#include "jled.h"

static const uint8_t kBitsBrightness = 5;
//static const uint8_t kBrightnessStep = 1 << (8 - 5);
static const uint16_t kRepeatForever = 65535;
static const uint32_t kTimeUndef = -1;

static jled_t tableJled[JLED_RES_MAX];

static uint8_t scale5(uint8_t val, uint8_t factor) {
  if (factor == 31)
    return val; // optimize for most common case (full brightness)
  return ((uint16_t)val * factor) >> 5;
}

static void JLED_Write(jled_t *led, uint8_t val) {
  val = scale5(val, led->maxBrightness);
  HalPwm_AnalogWrite(led->pHal, val);
}

static jled_t *JLED_SetBrightnessEval(jled_t *led, brightnessEvaluator_t *be);

jled_t *JLED_Init(jled_t *led, const _HalPwm_t *pHal) {
  led->brightnessEval = NULL;
  led->pHal = pHal;
  led->state = JLED_ST_RUNNING;
  led->maxBrightness = (1 << kBitsBrightness) - 1;
  led->numRepetitions = 1;
  led->lastUpdateTime = kTimeUndef;
  led->timeStart = kTimeUndef;
  led->delayBefore = 0;
  led->delayAfter = 0;
  
  return led;
}

jled_t *JLED_GetStaticJledFromIndex(jledRes_t idx) {
  return &(tableJled[idx]);
}

bool JLED_Update(jled_t *led) {
  return JLED_UpdateL(led, HAL_Ticks());
}

// Sets LED to given brightness
jled_t *JLED_Set(jled_t *led, uint8_t brightness) {
  constantBrightnessEvaluator_t *pBe = &(led->brightnessEvalBuf.consBrev);
  constantBrightnessInit(pBe, brightness);
  return JLED_SetBrightnessEval(led, (brightnessEvaluator_t *)pBe);
}

// turn LED on
jled_t *JLED_On(jled_t *led) {
  return JLED_Set(led, kFullBrightness);
}

// turn LED off
jled_t *JLED_Off(jled_t *led) {
  return JLED_Set(led, kZeroBrightness);
}

// Fade LED on
jled_t *JLED_FadeOn(jled_t *led, uint16_t duration) {
  fadeOnBrightnessEvaluator_t *pBe = &(led->brightnessEvalBuf.fadeonBrev);
  fadeOnBrightnessInit(pBe, duration);
  return JLED_SetBrightnessEval(led, (brightnessEvaluator_t *)pBe);
}

// Fade LED off - actually is just inverted version of FadeOn()
jled_t *JLED_FadeOff(jled_t *led, uint16_t duration) {
  fadeOffBrightnessEvaluator_t *pBe = &(led->brightnessEvalBuf.fadeoffBrev);
  fadeOffBrightnessInit(pBe, duration);
  return JLED_SetBrightnessEval(led, (brightnessEvaluator_t *)pBe);
} 

// Set effect to Breathe, with the given period time in ms.
jled_t *JLED_Breathe(jled_t *led, uint16_t period) {
  breatheBrightnessEvaluator_t *pBe = &(led->brightnessEvalBuf.breatheBrev);
  breatheBrightnessInit(pBe, period);
  return JLED_SetBrightnessEval(led, (brightnessEvaluator_t *)pBe);
}

// Set effect to Blink, with the given on- and off- duration values.
jled_t *JLED_Blink(jled_t *led, uint16_t durationOn, uint16_t durationOff) {
  blinkBrightnessEvaluator_t *pBe = &(led->brightnessEvalBuf.blinkBrev);
  blinkBrightnessInit(pBe, durationOn, durationOff);
  return JLED_SetBrightnessEval(led, (brightnessEvaluator_t *)pBe);
}

// Set effect to Candle light simulation
// Test value : speed = 6, jitter = 15, period = 0xffff
jled_t *JLED_Candle(jled_t *led, uint8_t speed, uint8_t jitter, uint16_t period) {
  candleBrightnessEvaluator_t *pBe = &(led->brightnessEvalBuf.candleBrev);
  candleBrightnessInit(pBe, speed, jitter, period);
  return JLED_SetBrightnessEval(led, (brightnessEvaluator_t *)pBe);
}

// Use a user provided brightness evaluator.
jled_t *JLED_UserFunc(jled_t *led, brightnessEvaluator_t *userEval) {
  return JLED_SetBrightnessEval(led, userEval);
}


// set number of repetitions for effect.
jled_t *JLED_Repeat(jled_t *led, uint16_t numRepetitions) {
  led->numRepetitions = numRepetitions;
  return led;
}

// repeat Forever
jled_t *JLED_Forever(jled_t *led) {
  return JLED_Repeat(led, kRepeatForever);
}

bool JLED_IsForever(jled_t *led) {
  return led->numRepetitions == kRepeatForever;
}

// Set amount of time to initially wait before effect starts. Time is
// relative to first call of Update() method and specified in ms.
jled_t *JLED_DelayBefore(jled_t *led, uint16_t delayBefore) {
  led->delayBefore = delayBefore;
  return led;
}

// Set amount of time to wait in ms after each iteration.
jled_t *JLED_DelayAfter(jled_t *led, uint16_t delayAfter) {
  led->delayAfter = delayAfter;
  return led;
}

// Stop current effect and turn LED immediately off. Further calls to
// Update() will have no effect.
jled_t *JLED_Stop(jled_t *led) {
  JLED_Write(led, kZeroBrightness);
  led->state = JLED_ST_STOPPED;
  return led;
}

bool IsRunning(jled_t *led) {
  return led->state != JLED_ST_STOPPED;
}

jled_t *JLED_Reset(jled_t *led) {
  led->timeStart = kTimeUndef;
  led->lastUpdateTime = kTimeUndef;
  led->state = JLED_ST_RUNNING;
  return led;
}

// Sets the maximum brightness level. 255 is full brightness, 0 turns the
// effect off. Currently, only upper 5 bits of the provided value are used
// and stored.
jled_t *JLED_MaxBrightness(jled_t *led, uint8_t level) {
  led->maxBrightness = level >> (8 - kBitsBrightness);
  return led;
}

// Returns current maximum brightness level. Since currently only upper 5
// bits are used, lower 3 bits will always be 0.
uint8_t JLED_GetMaxBrightness(jled_t *led) {
  return led->maxBrightness << (8 - kBitsBrightness);
}


// update brightness of LED using the given brightness evaluator
//  (brightness)                       ________________
// on 255 |                         ¸-'
//        |                      ¸-'
//        |                   ¸-'
// off 0  |________________¸-'
//        |<-delay before->|<--period-->|<-delay after-> (time)
//                         | func(t)    |
//                         |<- num_repetitions times  ->
bool JLED_UpdateL(jled_t *led, uint32_t now) {
  if(led->state == JLED_ST_STOPPED || !(led->brightnessEval)) return false;
  
  //no need to process updates twice during one time tick.
  if(led->lastUpdateTime == now) return true;
  
  if(led->lastUpdateTime == kTimeUndef) {
    led->timeStart = now + led->delayBefore;
  }
  led->lastUpdateTime = now;
  
  if (now < led->timeStart) return true;
  
  // t cycles in range [0..period+delay_after-1]
  const uint16_t period = led->brightnessEval->period(led->brightnessEval);
  const uint32_t t = (now - led->timeStart) % (period + led->delayAfter);
  
  if(t < period) {
    led->state = JLED_ST_RUNNING;
    JLED_Write(led, led->brightnessEval->eval(led->brightnessEval, t));
  } else {
    if(led->state == JLED_ST_RUNNING) {
      // when in delay after phase, just call Write()
      // once at the beginning.
      led->state = JLED_ST_IN_DELAY_AFTER_PHASE;
      JLED_Write(led, led->brightnessEval->eval(led->brightnessEval, period - 1));
    }
  }
  
  if(JLED_IsForever(led)) return true;
  
  const uint32_t timeEnd = led->timeStart + (uint32_t) (period + led->delayAfter) *
      led->numRepetitions - 1;
      
  if(now >= timeEnd) {
    // make sure final value of t = (period-1) is set
    led->state = JLED_ST_STOPPED;
    JLED_Write(led, led->brightnessEval->eval(led->brightnessEval, period - 1));
    return false;
  }
  return true;
}

static jled_t *JLED_SetBrightnessEval(jled_t *led, brightnessEvaluator_t *be) {
  led->brightnessEval = be;
  
  // start over after the brightness evaluator changed
  return JLED_Reset(led);
}
