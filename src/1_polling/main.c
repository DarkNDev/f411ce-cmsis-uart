#include "uart_init.h"

void USART1_WriteChar(char ch);
char USART1_ReadChar(void);
void USART1_WriteString(const char *str);

int main(void) {
  USART1_Init();

  while (1) {
    // Blocking read: wait for incoming character
    char c = USART1_ReadChar();

    // Echo back with newline conversion if Enter (CR or LF) is received
    if (c == '\r' || c == '\n') {
      USART1_WriteString("\r\n");
    } else {
      USART1_WriteChar(c);
    }
  }
}

// Transmit a single character (blocking polling)
void USART1_WriteChar(char ch) {
  // Wait until Transmit Data Register is Empty (TXE flag set)
  while (!(USART1->SR & USART_SR_TXE))
    ;

  // Writing to DR clears the TXE flag automatically
  USART1->DR = (uint8_t)ch;
}

// Transmit a null-terminated string
void USART1_WriteString(const char *str) {
  while (*str) {
    USART1_WriteChar(*str++);
  }
}

// Receive a single character (blocking polling)
char USART1_ReadChar(void) {
  // Wait until Read Data Register has received data (RXNE flag set)
  while (!(USART1->SR & USART_SR_RXNE))
    ;

  // Reading DR clears the RXNE flag automatically
  return (char)(USART1->DR & 0xFF);
}