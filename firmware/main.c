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

  // config LED pins (POWER, TX, RX)
  DDRC |= ((1<<DDC4) || (1<<DDC3) || (1<<DDC2));
  PORTC &= ~((1<<PORTC4) || (1<<PORTC3) || (1<<PORTC2));

  sei();

  // Signal power up
  PORTC |= (1<<PORTC4);

  while (1) {
    // Wait for command
    uint8_t cmd = uart_getc();

    // Dispatch by cmd byte
    switch( cmd ) {
    // GET_VALUE
    case 0x01: {
      PORTC |= (1<<PORTC2);
      uint16_t v = 0;
      // Short delay before starting measurement
      _delay_ms(10);
      for (uint8_t i=0; i<64; i++) { v += adc_get(); }
      uart_putc(0x00); uart_putc(v>>8); uart_putc(v&0xff);
      PORTC &= ~(1<<PORTC3);
    }
      break;

    // SET_FREQUENCY
    case 0x02: {
      PORTC |= (1<<PORTC3);
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
      PORTC |= ((1<<PORTC3) || (1<<PORTC2));
      // Receive 32 bit uint
      uint32_t factor = uart_getc();
      factor = ( (factor<<8) | uart_getc() );
      factor = ( (factor<<8) | uart_getc() );
      factor = ( (factor<<8) | uart_getc() );
      // set factor
      ad9850_set(factor);
      // Short delay before starting measurement
      _delay_ms(10);
      // Measure
      uint16_t v = 0;
      // adc_get() returns a 10bit uint.
      // -> summing 64 10bit values will result into a 16bit uint
      for (uint8_t i=0; i<64; i++) { v += adc_get(); }
      // ... done. Send result
      uart_putc(0x00); uart_putc(v>>8); uart_putc(v&0xff);
      PORTC &= ~(1<<PORTC2);
    }
      break;

    // SHUTDOWN
    case 0x04:
      PORTC &= ~(1<<PORTC3);
      ad9850_shutdown();
      uart_putc(0x00);
      break;

    // else ERROR
    default:
      uart_putc(0xff);
      break;
    }
  }

  return 0;
}

