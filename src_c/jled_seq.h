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

#ifndef __JLED_SEQ_H__
#define __JLED_SEQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "jled.h"

typedef enum {
  JLED_SEQ_RES_LED = 0,
  JLED_SEQ_RES_MAX
}jledSeqRes_t;

typedef enum {
  JLED_SEQ_SEQUENCE = 0,
  JLED_SEQ_PARALLEL
}jledSeqMode_t;

typedef struct JledSequence {
  jledSeqMode_t mode;
  jled_t *leds;
  uint32_t cur;
  uint32_t n;
  uint16_t kRepeatForever;
  uint16_t numRepetitions;
  uint16_t iteration;
  bool isRunning;
}jledSequence_t;

jledSequence_t *JLED_SequenceInit(jledSequence_t *seq, jledSeqMode_t mode, jled_t *leds, uint32_t n);
jledSequence_t *JLED_GetStaticJledSeqFromIndex(jledSeqRes_t idx);
bool JLED_SeqUpdate(jledSequence_t *seq);
void JLED_SeqReset(jledSequence_t *seq);
void JLED_SeqStop(jledSequence_t *seq);
jledSequence_t *JLED_SeqRepeat(jledSequence_t *seq, uint16_t numRepetitions);
jledSequence_t *JLED_SeqForever(jledSequence_t *seq);
bool JLED_SeqIsForever(jledSequence_t *seq);

#ifdef __cplusplus
}
#endif

#endif /* __JLED_SEQ_H__ */
