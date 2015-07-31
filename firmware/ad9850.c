#include "ad9850.h"
#include <avr/io.h>
#include <util/delay.h>

void ad9850_send_bit(uint8_t bit) {
  if (bit) { PORTB |= (1<<DDB0); }
  else { PORTB &= ~(1<<DDB0); }
  PORTD |= (1<<DDD6);
  _delay_us(1);
  PORTD &= ~(1<<DDD6);
  _delay_us(1);
}

void ad9850_update_frequency() {
  PORTD |= (1<<DDD7);
  _delay_us(1);
  PORTD &= ~(1<<DDD7);
  _delay_us(1);
}

void ad9850_init() {
  // PB0 -> DATA
  // PD6 -> CLK
  // PD7 -> FQ_UP
  DDRB  |= (1 << DDB0);
  DDRD  |= (1 << DDD6) | (1 << DDD7);
  // set zero
  PORTB &= ~( (1<< DDB0) );
  PORTD &= ~( (1 << DDD6) | (1 << DDD7) );

  ad9850_send_bit(0);
  ad9850_send_bit(0);
  ad9850_set(0);
}

void ad9850_set(uint32_t fword) {
  // Send 32 freq. bits
  for (uint8_t i=0; i<32; i++) {
    ad9850_send_bit(fword & 0x01);
    fword = (fword >> 1);
  }

  // Send ctrl bits 0,0,0
  ad9850_send_bit(0);
  ad9850_send_bit(0);
  ad9850_send_bit(0);

  // Send 5 pahse bits (0,0,0,0,0)
  for (int i=0; i<5; i++) {
    ad9850_send_bit(0);
  }

  // Update freq:
  ad9850_update_frequency();
}
