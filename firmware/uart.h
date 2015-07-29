/** @defgroup uart The UART interface. */
#ifndef __LIBAVR_UART_H__
#define __LIBAVR_UART_H__

#include <inttypes.h>

/** Helper macro to compute the baud-rate prescale. */
#define UART_BAUD_SELECT(baud, clk) (clk/(baud*16UL)-1)

// Pre-define some common rates
#define UART_BAUD_9600  UART_BAUD_SELECT(9600UL, F_CPU)

/** Initializes the USART of the device with the specified
 * baud-rate prescale. */
extern void uart_init(uint16_t baud_prescale);

/** Returns the number of bytes in the buffer. */
extern uint8_t uart_available();
/** Clears the receive buffer. */
extern void uart_flush();

/** Returns a single byte from the receive buffer. */
extern uint8_t uart_getc();
/** Reads upto @c len bytes from the receive buffer. */
extern uint8_t uart_read(uint8_t *buffer, uint8_t len);

/** Sends a byte (blocking). */
extern void uart_putc(uint8_t c);
/** Sends @c len bytes (blocking). */
extern uint8_t uart_write(uint8_t *buffer, uint8_t len);

#endif // __LIBAVR_UART_H__
