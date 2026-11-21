#include "uart_init.h"

#define BUFFER_SIZE 4096

// Circular Ring Buffer Structures
typedef struct {
  uint8_t data[BUFFER_SIZE];
  volatile uint32_t head;
  volatile uint32_t tail;
} RingBuffer_t;

RingBuffer_t tx_buffer = {.head = 0, .tail = 0};

// Non-blocking transmit function
uint8_t USART1_Buffer(uint8_t data) {
  uint32_t next_head = (tx_buffer.head + 1) % BUFFER_SIZE;

  // Wait only if buffer is entirely full (rare safety fallback)
  if (next_head == tx_buffer.tail) {
    return 0;
  }

  // Place character into software buffer
  tx_buffer.data[tx_buffer.head] = data;
  tx_buffer.head = next_head;

  // CRITICAL: Enable the TXE interrupt.
  // The hardware will instantly see the TX register is empty and fire the IRQ.
  USART1->CR1 |= USART_CR1_TXEIE;
  return 1;
}

// Interrupt Service Routine for RX and TX
void USART1_IRQHandler(void) {
  // Check if RXNE is set
  if (USART1->SR & USART_SR_RXNE) {
    // Grab the incoming byte (clears RXNE flag)
    uint8_t received_data = (uint8_t)(USART1->DR & 0xFF);

    // Push it straight to the interrupt-driven transmitter
    USART1_Buffer(received_data);
  }

  // Check if TXE flag is set AND the TXE interrupt is actually enabled
  if ((USART1->SR & USART_SR_TXE) && (USART1->CR1 & USART_CR1_TXEIE)) {
    if (tx_buffer.tail != tx_buffer.head) {
      // Buffer has data! Write the next byte to the data register.
      // (Writing to DR automatically clears the TXE hardware flag)
      USART1->DR = tx_buffer.data[tx_buffer.tail];
      tx_buffer.tail = (tx_buffer.tail + 1) % BUFFER_SIZE;
    } else {
      // Buffer empty! Nothing left to send.
      // We MUST disable TXEIE, otherwise the CPU will lock up looping in this
      // ISR forever.
      USART1->CR1 &= ~USART_CR1_TXEIE;
    }
  }
}

int main(void) {
  USART1_Init_Interrupt();

  while (1) {
    __WFI();
  }
}