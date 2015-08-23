#include "adc.h"
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


static volatile uint8_t  adc_finished = 0;
static volatile uint16_t adc_result = 0;

void adc_init() {
  // Disable digital input in ADC4
  DIDR0 |= (1<<4);
  // Enable ADC & set prescaler
  ADCSRA =
      // Enable ADC
      (1<<ADEN) |
      // Enable interrupt
      (1<<ADIE) |
      // Prescaler 128
      (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
  // Select ADC source
  ADMUX =
      // external reference (AREF=2.5V)
      (0 << REFS1) | (0 << REFS0) |
      // right adjust
      (0 << ADLAR) |
      // Select ADC4 as intput
      (0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (0 << MUX0);
}

uint16_t adc_get() {
  adc_finished = 0;
  // Enter noise reduction mode
  set_sleep_mode(SLEEP_MODE_ADC);
  sleep_mode();
  while (0 == adc_finished) { }
  return adc_result;
}

ISR(ADC_vect) {
  adc_result = ADC;
  adc_finished = 1;
}
