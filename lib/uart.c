#include "uart.h"
#include "bitops.h"

#include <avr/io.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#define USART_EVEN_PARITY (1)
#define USART_ODD_PARITY (2)

#define ATMEGA32_UART_BUF_SIZE (50)

void uartInit(uint32_t baud, uint8_t dataBits, char parity, uint8_t stoppBits) {
    /* Set baud rate, see p 147 in data sheet */
    UBRRH = (F_CPU/(16 * baud)-1) / 256;
    UBRRL = (F_CPU/(16 * baud)-1) % 256;
  
    //UBRR = (F_CPU/(16 * baud)-1);

    /* Enable receiver and transmitter */
    UCSRB = (1<<RXEN)|(1<<TXEN);


    // stopBits
    uint8_t stoppBitsPattern; // USBS
    stoppBitsPattern = (stoppBits == 2)?1:0; 

    // parity
    uint8_t parityBitsPattern; // UPM1 ... UPM0
    switch (parity) {
        case 'N':
            parityBitsPattern = 0;
            break;
        case 'E':
            parityBitsPattern = 2;
            break;
        case 'O':
            parityBitsPattern = 3;
            break;
        default:
            parityBitsPattern = 0; // default: no parity
    }

    // data bits
    uint8_t dataBitsPattern; // UCSZ2 .. UCSZ0

    switch (dataBits)
    {
    case 5:
        // 000
        dataBitsPattern = 0;
        break;
    case 6:
        // 001
        dataBitsPattern = 1;
        break;
    case 7:
        // 010
        dataBitsPattern = 2;
        break;
    case 8:
        // 011
        dataBitsPattern = 3;
        break;
    case 9:
        // 111
        dataBitsPattern = 7;
        break;
    default:
        dataBitsPattern = 3; // 8 data bits as default
    }

    // Beispiel für die Maskierung von DataBitPattern.
    // --> das Patern ist 3 Bits breit, wir wollen nur die unteren 2 Bits haben
    // Beispiel:
    // DataBitsPattern == 0b00000111
    // Maske           == 0b00000011
    // Ergebnis        == 0b00000011 

    // put it all together!
    UCSRC = (1 << URSEL) | (parityBitsPattern << UPM0) | (stoppBitsPattern << USBS) | ((dataBitsPattern & 0x03) << UCSZ0);
    if (dataBitsPattern & 0x04) {
        setBit(UCSRB, UCSZ2);
    } else {
        clrBit(UCSRB, UCSZ2);
    }
}

void uartPutc(char c) {
    /* Wait for empty transmit buffer */
    while ( !( UCSRA & (1<<UDRE)) )
      ;
    /* Put data into buffer, sends the data */
    UDR = c;
}

void uartPuts(const char* c) {
  while (*c !=0) {
    uartPutc(*c);
    c++;
  }
}

char uartGetc() {
    /* Wait for data to be received */
    while ( !(UCSRA & (1<<RXC)) )
      ;
    /* Get and return received data */
    return UDR;
}

void uartPrintf(const char* format, ...) {
    va_list argptr;
    va_start(argptr, format);
    char buf[ATMEGA32_UART_BUF_SIZE];
    // print to buffer:
    vsnprintf(buf, sizeof(buf), format, argptr);
    // send the buffer to UART:
    uartPuts(buf); 
    va_end(argptr);
}
