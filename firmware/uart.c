#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define UART_BUFFER_SIZE 32
#define UART_BUFFER_MASK (UART_BUFFER_SIZE-1)
#define min(a,b)         ( (a<b) ? a : b )

volatile uint8_t uart_rx_buffer[UART_BUFFER_SIZE];
volatile uint8_t uart_rx_head;
volatile uint8_t uart_rx_tail;

void
uart_init(uint16_t baud_prescale) {
  // Enable UART
  PRR    &= ~(1<<PRUSART0);
  // Enable receiver and transmitter & RX interrupt
  UCSR0B |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
  //UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
  // Set frame: 8data, 1 stop
  UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
  // Set buadrate
  UBRR0H  = (uint8_t) (baud_prescale>>8);
  UBRR0L  = (uint8_t) (baud_prescale);
  // Init ring buffer
  uart_rx_head = uart_rx_tail = 0;
}

uint8_t
uart_available() {
  if (uart_rx_head>=uart_rx_tail) { return (uart_rx_head-uart_rx_tail); }
  return UART_BUFFER_SIZE + uart_rx_tail - uart_rx_head ;
}

uint8_t
uart_getc() {
  while (uart_rx_head == uart_rx_tail) { }
  uint8_t c = uart_rx_buffer[uart_rx_tail];
  uart_rx_tail = (uart_rx_tail+1) & UART_BUFFER_MASK;
  return c;
}

void
uart_putc(uint8_t c) {
  // Do nothing until UDR is ready for more data to be written to it
  while (!(UCSR0A & (1<<UDRE0))) {};
  UDR0 = c;
}

uint8_t
uart_write(uint8_t *buffer, uint8_t len) {
  for (uint8_t i=0; i<len; i++) {
    uart_putc(buffer[i]);
  }
  return len;
}

uint8_t
uart_read(uint8_t *buffer, uint8_t len) {
  uint8_t n = min(len, uart_available());
  for (uint8_t i=0; i<n; i++) {
    buffer[i] = uart_getc();
  }
  return n;
}

ISR(USART_RX_vect)
{
  uint8_t c = UDR0;
  uint8_t new_head = ((uart_rx_head + 1) & UART_BUFFER_MASK);
  // check for overflow
  if (new_head == uart_rx_tail) { return; }
  // Get & store byte
  uart_rx_buffer[uart_rx_head] = c;
  uart_rx_head = new_head;
}
