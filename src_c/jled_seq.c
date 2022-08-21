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

#include "jled_seq.h"

static jledSequence_t tableJledSequence[JLED_SEQ_RES_MAX];

jledSequence_t *JLED_SequenceInit(jledSequence_t *seq, jledSeqMode_t mode, jled_t *leds, uint32_t n) {
  *seq = (jledSequence_t) {
    .mode = mode,
    .leds = leds,
    .cur = 0,
    .n = n,
    .kRepeatForever = 65535,
    .numRepetitions = 1,
    .iteration = 0,
    .isRunning = true
  };
  
  return seq;
}

jledSequence_t *JLED_GetStaticJledSeqFromIndex(jledSeqRes_t idx) {
  return &(tableJledSequence[idx]);
}

static bool JLED_SeqUpdateParallel(jledSequence_t *seq) {
  bool result = false;
  int i;
  for(i = 0u; i < seq->n; i++) {
    result |= JLED_Update(&(seq->leds[i]));
  }
  return result;
}

static bool JLED_SeqUpdateSequentially(jledSequence_t *seq) {
  if(seq->cur >= seq->n) {
    return false;
  }
  if(!JLED_Update(&(seq->leds[seq->cur]))) {
    return ++seq->cur < seq->n;
  }
  return true;
}

static void JLED_SeqResetLeds(jledSequence_t *seq) {
  int i;
  for(i = 0u; i < seq->n; i++) {
    JLED_Reset(&(seq->leds[i]));
  }
}

bool JLED_SeqUpdate(jledSequence_t *seq) {
  if(!seq->isRunning) {
    return false;
  }
  
  const bool ledRunning = (seq->mode == JLED_SEQ_PARALLEL) ? JLED_SeqUpdateParallel(seq)
      : JLED_SeqUpdateSequentially(seq);      
  if(ledRunning) {
    return true;
  }
  
  // start next iteration of sequence
  seq->cur = 0;
  JLED_SeqResetLeds(seq);
  
  seq->isRunning = ++seq->iteration < seq->numRepetitions ||
    seq->numRepetitions == seq->kRepeatForever;
    
  return seq->isRunning;
}

void JLED_SeqReset(jledSequence_t *seq) {
  JLED_SeqResetLeds(seq);
  seq->cur = 0;
  seq->iteration = 0;
  seq->isRunning = true;
}

void JLED_SeqStop(jledSequence_t *seq) {
  int i;
  for(i = 0u; i < seq->n; i++) {
    JLED_Stop(&(seq->leds[i]));
  }
}

jledSequence_t *JLED_SeqRepeat(jledSequence_t *seq, uint16_t numRepetitions) {
  seq->numRepetitions = numRepetitions;
  return seq;
}

jledSequence_t *JLED_SeqForever(jledSequence_t *seq) {
  return JLED_SeqRepeat(seq, seq->kRepeatForever);
}

bool JLED_SeqIsForever(jledSequence_t *seq) {
  return seq->numRepetitions == seq->kRepeatForever;
}
