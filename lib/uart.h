#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * F73 -- Helper API for RNControl Board with ATmega32
*/

/**
 * Init UART in async mode using 
 * baud symbols per second (see datasheet for supported baud rates),
 * dataBits data bits (5 ... 9),
 * parity 'N' (no), 'E' (even), 'O' (odd), and
 * stoppBits stop bits (1 or 2).
*/
void uartInit(uint32_t baud, uint8_t dataBits, char parity, uint8_t stoppBits);

/**
 * Write a single char c to UART.
*/
void uartPutc(char c);

/**
 * Write a null-terminated string to UART.
*/
void uartPuts(const char* c);

/**
 * printf to UART.
*/
void uartPrintf(const char* format, ...);




/**
 * Read a single char from UART.
*/
char uartGetc(void);

#ifdef __cplusplus
}
#endif
