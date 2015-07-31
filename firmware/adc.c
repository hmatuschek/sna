#include "adc.h"
#include <avr/io.h>


void adc_init() {
  ADCSRA =
      // Enable ADC
      (1<<ADEN) |
      // Prescaler 2
      (0<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);
  // Select ADC source
  ADMUX =
      // external reference (AREF=2.5V)
      (0 << REFS1) | (0 << REFS0) |
      // Select ADC4 as intput
      (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (0 << MUX0);
}

uint16_t adc_get() {
  // Start conversion
  ADCSRA |= (1 << ADSC);
  // Wait for ADC finish
  while (ADCSRA & (1<<ADSC)) { }
  return ADC;
}
