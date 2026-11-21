#ifndef TIM5_INIT_H
#define TIM5_INIT_H

#include "stm32f411xe.h"
#include <stdint.h>

extern volatile uint32_t system_millis;

static inline void TIM5_Init(void) {
  RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

  // Assumes default 16 MHz HSI clock (APB1 timer clock = 16 MHz).
  // If running at max 100 MHz SYSCLK (APB1 = 50 MHz -> TIM5 = 100 MHz),
  // change PSC to: 100 - 1
  TIM5->PSC = 16 - 1;   // 16 MHz / 16 = 1 MHz counter (1 µs ticks)
  TIM5->ARR = 1000 - 1; // Overflow every 1000 µs = 1 ms

  TIM5->SR &= ~TIM_SR_UIF;
  TIM5->DIER |= TIM_DIER_UIE;

  NVIC_EnableIRQ(TIM5_IRQn);
  NVIC_SetPriority(TIM5_IRQn, 2);

  TIM5->CR1 |= TIM_CR1_CEN;
}

static inline uint32_t millis(void) { return system_millis; }

static inline void delay_ms(uint32_t ms) {
  uint32_t start = millis();
  while ((millis() - start) < ms)
    ;
}

/*

Define ISR and system_millis in main.c:

volatile uint32_t system_millis = 0;

void TIM5_IRQHandler(void) {
  if (TIM5->SR & TIM_SR_UIF) {
    TIM5->SR &= ~TIM_SR_UIF;

    // task
    system_millis++;
  }
}

*/

#endif /* TIM5_INIT_H */
