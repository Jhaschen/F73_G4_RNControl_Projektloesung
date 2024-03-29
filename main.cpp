#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "ATMega32_utility_bib.h"


//#define debug // Ausgabe der Botschaften

#define adress 0x02 // Eigene eindeutige Adresse des RNControl Boards

Button B;
uint8_t cnt=0;

CAN can(BITRATE_500_KBPS);
CAN_MSG sendmsg_Button;		// Message-Objekt auf dem Stack anlegen
CAN_MSG resvmsg;		// Message-Objekt auf dem Stack anlegen

USART UART(8,0,1,9600);	// USART init 8 Zeichenbits , keien Paritätsbits , 1 Stoppbit, 9600 Zeichen pro Sekunde
char buffer[150];		// Buffer zur Zwschischenspeicherung von Zeichenketten

int main ()
{
	DDRC = 0xFF;			// LED-Port: output
	PORTC = 0x00;			// LEDs ein
    _delay_ms(500);
    PORTC = 0xFF;            // LEDs aus
    
	
   	CAN_MSG sendmsg_LED;          // Message-Objekt auf dem Stack anlegen
	
        sendmsg_LED.id = 0x74;          // ID setzen, hier: dec
        sendmsg_LED.rtr = 0;      // Remote-Transmission-Request -> aus
        sendmsg_LED.dlc = 2;         // Länge der Nachricht: 2 Byte
	
	sprintf(buffer,"CAN Projekt LED Controller");	// Zeichenkette erzeugen und in dn Zwischenspeicher schreiben
	UART.UsartPuts(buffer);		   // Ausgeben
	UART.UsartPuts("\n\r");		   // Neue Zeile
	UART.UsartPuts("\n\r");		   // Neue Zeile
	_delay_ms(500);
	

	Timer T; 
	T.Timer_0_Compare_ISR_init(); // Timer ISR 1ms
	
	
	for (;;)
	{
		
		sprintf(buffer,"Gebe die Adresse des Empfänger Boards an 0x01 = Board[1] 0x2 = Board[2] !");	// Zeichenkette erzeugen und in dn Zwischenspeicher schreiben
		UART.UsartPuts(buffer);		   // Ausgeben
		UART.UsartPuts("\n\r");		   // Neue Zeile
		sendmsg_LED.data[0] = (UART.UsartGetc()-0x30); // ASCII in Dez
		// Prüfung auf Gültigkeit fehlt nur 1-4 oder so
		sprintf(buffer,"Gebe an welche LEDs angesteuert werden sollen 0 == LED1 an/aus !");	// Zeichenkette erzeugen und in dn Zwischenspeicher schreiben
		UART.UsartPuts(buffer);		   // Ausgeben
		UART.UsartPuts("\n\r");		   // Neue Zeile
		sendmsg_LED.data[1] = (UART.UsartGetc()-0x30);
		// Prüfung auf Gültigkeit fehlt nur 0-7
		can.CAN_Send(&sendmsg_LED);
		_delay_ms(100);
		
		
	}
	return 0;
}


// Compare " Vergleichsregister" Interrupt
//------------------------------------------------------------------------------
//  Interrupt Service Routinen
//------------------------------------------------------------------------------
// Interrupt-Service-Routine für den Interrupt bei Überlauf des Timer0
// ISR: Schlüsselwort für Compiler, dass dies eine ISR ist
// TIMER0_COMP_vect: Information an den Compiler, mit welchem Interrupt
//                  diese ISR verknüpft werden soll. Der Bezeichner "TIMER0_COM_vect"
//                  ist wie alle anderen ISR-Bezeichner in "avr/interrupt.h" definiert.
ISR(TIMER0_COMP_vect)
{
	cnt++;
	can.CAN_Rec(&resvmsg);
	if(resvmsg.data[0]== adress) // Ist die Botschaft für dieses Board ?
	{
		TGL_BIT(PORTC,resvmsg.data[1]);  // Byte 1 auf die LEDs ausgeben ( umgekehrte Logik)
	}
	
	
	
	
	if((cnt==250)) // 25 x 1ms = 250ms  (Alle 250ms wird die Button Botschaft gesendet
	{
	
	sendmsg_Button.id = 0x75;          // ID setzen, hier: dec
    sendmsg_Button.dlc = 2;         // Länge der Nachricht: 2 Byte
	sendmsg_Button.data[0]= adress;     // Datenbyte 0 füllen
	sendmsg_Button.data[1]=B.Button_read(); // Button Status in das Datenbyte 1 schreiben
	can.CAN_Send(&sendmsg_Button);
		
	
		}
	
 
 }

