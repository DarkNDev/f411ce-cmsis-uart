#ifndef UART_INIT_H
#define UART_INIT_H

#include "stm32f411xe.h"

// Initialize USART1: PA9 (TX), PA10 (RX), 115200 8N1

static inline void USART1_Init(void) {
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  GPIOA->MODER &= ~(GPIO_MODER_MODE9 | GPIO_MODER_MODE10);
  GPIOA->MODER |= (2U << GPIO_MODER_MODE9_Pos) | (2U << GPIO_MODER_MODE10_Pos);

  GPIOA->OSPEEDR |=
      (3U << GPIO_OSPEEDR_OSPEED9_Pos) | (3U << GPIO_OSPEEDR_OSPEED10_Pos);

  // Pull-up for RX (PA10) to avoid floating line, No-pull for TX (PA9)
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD9 | GPIO_PUPDR_PUPD10);
  GPIOA->PUPDR |= (1U << GPIO_PUPDR_PUPD10_Pos);

  // Map PA9 and PA10 to AF7 in AFR[1] (AFRH handles pins 8-15)
  // Pin 9  -> Bits[7:4]
  // Pin 10 -> Bits[11:8]
  GPIOA->AFR[1] &= ~((0xFU << 4) | (0xFU << 8));
  GPIOA->AFR[1] |= (7U << 4) | (7U << 8);

  // Configure Baud Rate
  // Default HSI = 16 MHz. APB2 prescaler default = 1 (16 MHz)
  // 16 MHz / (16 * 115200) = 8.6875 -> Mantissa = 8, Fraction = 11 (0x0B)
  USART1->BRR = 0x008B;

  // Enable Transmitter (TE) and Receiver (RE)
  USART1->CR1 = USART_CR1_TE | USART_CR1_RE;

  // Enable USART peripheral (UE)
  USART1->CR1 |= USART_CR1_UE;
}

static inline void USART1_Init_Interrupt(void) {
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  GPIOA->MODER &= ~(GPIO_MODER_MODE9 | GPIO_MODER_MODE10);
  GPIOA->MODER |= (2U << GPIO_MODER_MODE9_Pos) | (2U << GPIO_MODER_MODE10_Pos);

  GPIOA->OSPEEDR |=
      (3U << GPIO_OSPEEDR_OSPEED9_Pos) | (3U << GPIO_OSPEEDR_OSPEED10_Pos);

  // Pull-up for RX (PA10) to avoid floating line, No-pull for TX (PA9)
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD9 | GPIO_PUPDR_PUPD10);
  GPIOA->PUPDR |= (1U << GPIO_PUPDR_PUPD10_Pos);

  // Map PA9 and PA10 to AF7 in AFR[1] (AFRH handles pins 8-15)
  // Pin 9  -> Bits[7:4]
  // Pin 10 -> Bits[11:8]
  GPIOA->AFR[1] &= ~((0xFU << 4) | (0xFU << 8));
  GPIOA->AFR[1] |= (7U << 4) | (7U << 8);

  // Configure Baud Rate
  // Default HSI = 16 MHz. APB2 prescaler default = 1 (16 MHz)
  // 16 MHz / (16 * 115200) = 8.6875 -> Mantissa = 8, Fraction = 11 (0x0B)
  USART1->BRR = 0x008B;

  // Enable Transmitter, Receiver, and RX Interrupt (RXNEIE)
  USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;

  // Enable USART1 interrupt in Cortex-M NVIC
  NVIC_SetPriority(USART1_IRQn, 2);
  NVIC_EnableIRQ(USART1_IRQn);

  // Enable USART peripheral (UE)
  USART1->CR1 |= USART_CR1_UE;
}

#endif /* UART_INIT_H */