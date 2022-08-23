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

#include <Arduino.h>
#include "hal_pwm.h"

static const uint16_t kBrightnessShiftFactor = 0;

static uint32_t analogWriteAdapter(uint16_t pin, uint16_t duty) {
  analogWrite(pin, duty);
  return 0;
}

static const _HalPwm_t tableLedHal[HAL_PWM_RES_MAX] = {
  {analogWriteAdapter, 6},
  {analogWriteAdapter, 9},
  {analogWriteAdapter, 10},
  {analogWriteAdapter, 11}  
};

uint32_t HAL_Ticks(void) {
  return millis();
}

const _HalPwm_t *const HalPwm_Init(halPwmRes_t idx) {
  const _HalPwm_t *hal = &(tableLedHal[idx]);
  pinMode(hal->pin, OUTPUT);
  return hal;
}

const _HalPwm_t *const HalPwm_GetStaticFromIndex(halPwmRes_t idx) {
  return &(tableLedHal[idx]);
}

void HalPwm_AnalogWrite(const _HalPwm_t *const hal, uint8_t val) {
  hal->pwmWrite(hal->pin, val << kBrightnessShiftFactor);
}
