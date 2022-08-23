// JLed demo.
#include "jled_seq.h"

void setup() {
  // breathe LED for 5 times
  JLED_DelayAfter(JLED_Repeat(JLED_Breathe(JLED_Init(JLED_GetStaticJledFromIndex(JLED_RES_LED0), HalPwm_Init(HAL_PWM_RES_LED0)), 2000), 5), 2000);
  JLED_DelayAfter(JLED_Repeat(JLED_Blink(JLED_Init(JLED_GetStaticJledFromIndex(JLED_RES_LED1), HalPwm_Init(HAL_PWM_RES_LED1)), 1000, 1000), 5), 2000);
  JLED_DelayAfter(JLED_Forever(JLED_Candle(JLED_Init(JLED_GetStaticJledFromIndex(JLED_RES_LED2), HalPwm_Init(HAL_PWM_RES_LED2)), 6, 15, 2000)), 2000);
  JLED_Forever(JLED_Blink(JLED_Init(JLED_GetStaticJledFromIndex(JLED_RES_LED3), HalPwm_Init(HAL_PWM_RES_LED3)), 500, 1500)), 2000;

  JLED_SequenceInit(JLED_GetStaticJledSeqFromIndex(JLED_SEQ_RES_LED), JLED_SEQ_PARALLEL, JLED_GetStaticJledFromIndex(JLED_RES_LED0), JLED_RES_MAX);
}

void loop() {
  //JLED_Update(JLED_GetStaticJledFromIndex(JLED_RES_LED0));

  JLED_SeqUpdate(JLED_GetStaticJledSeqFromIndex(JLED_SEQ_RES_LED));
}
