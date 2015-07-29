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
      uint32_t factor = uart_getc();
      factor = ( (factor<<8) | uart_getc() );
      factor = ( (factor<<8) | uart_getc() );
      factor = ( (factor<<8) | uart_getc() );
      ad9850_set(factor);
      uart_putc(0x00);
    }
      break;

    // SET & GET
    case 0x03: {
      uint32_t factor = uart_getc();
      factor = ( (factor<<8) | uart_getc() );
      factor = ( (factor<<8) | uart_getc() );
      factor = ( (factor<<8) | uart_getc() );
      ad9850_set(factor);
      uint8_t v = 0;
      for (uint8_t i=0; i<32; i++) { v += adc_get(); }
      uart_putc(0x00); uart_putc(v>>8); uart_putc(v);
    }
      break;

    // ERROR
    default:
      uart_putc(0xff);
      break;
    }
  }

  return 0;
}

