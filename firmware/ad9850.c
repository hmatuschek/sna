#include "ad9850.h"
#include <avr/io.h>
#include <util/delay.h>

void ad9850_send_bit(uint8_t bit) {
  if (bit) { PORTC |= (1<<DDC1); }
  else { PORTC &= ~(1<<DDC1); }
  PORTC |= (1<<DDC3);
  _delay_us(1);
  PORTC &= ~(1<<DDC3);
  _delay_us(1);
}

void ad9850_update_frequency() {
  PORTC |= (1<<DDC2);
  _delay_us(1);
  PORTC &= ~(1<<DDC2);
  _delay_us(1);
}

void ad9850_init() {
  // PC1 -> DATA
  // PC2 -> FQ_UP
  // PC3 -> CLK
  DDRC  |= (1 << DDC1) | (1 << DDC2) | (1 << DDC3);
  PORTC &= ~( (1 << DDC1) | (1 << DDC2) | (1 << DDC3) );

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
