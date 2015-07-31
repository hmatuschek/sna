#include "uart.h"
#include "ad9850.h"
#include "adc.h"

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


int main(void) {
  // Allow hardware to power-up
  _delay_ms(100);
  // Init UART
  uart_init(UART_BAUD_9600);
  // Init adc
  adc_init();
  // Init oscillator
  ad9850_init();

  sei();

  while (1) {
    // Wait for command
    uint8_t cmd = uart_getc();
    // Dispatch by cmd byte
    switch( cmd ) {
    // GET_VALUE
    case 0x01: {
      uint8_t v = 0;
      for (uint8_t i=0; i<32; i++) { v += adc_get(); }
      uart_putc(0x00); uart_putc(v>>8); uart_putc(v);
    }
      break;

    // SET_FREQUENCY
    case 0x02: {
      // Receive 32 bit uint
      uint32_t factor = uart_getc();
      factor = ( (factor<<8) | uart_getc() );
      factor = ( (factor<<8) | uart_getc() );
      factor = ( (factor<<8) | uart_getc() );
      // Set factor
      ad9850_set(factor);
      // ok.
      uart_putc(0x00);
    }
      break;

    // SET & GET
    case 0x03: {
      // Receive 32 bit uint
      uint32_t factor = uart_getc();
      factor = ( (factor<<8) | uart_getc() );
      factor = ( (factor<<8) | uart_getc() );
      factor = ( (factor<<8) | uart_getc() );
      // set factor
      ad9850_set(factor);
      // Measure
      uint8_t v = 0;
      // adc_get() returns a 10bit uint.
      // summing 32 10bit values will result into a 16bit uint
      for (uint8_t i=0; i<32; i++) { v += adc_get(); }
      // Send result
      uart_putc(0x00); uart_putc(v>>8); uart_putc(v);
    }
      break;

    // else ERROR
    default:
      uart_putc(0xff);
      break;
    }
  }

  return 0;
}

