// interrupt handling

#include "main.h"

ISR (USART0_RX_vect)	// receive from host
{
	printBuf[inPtr++] = UDR0;	// get char from host
	if (inPtr == PRINTBUFSIZE) inPtr = 0;
	printBufUsage++;
	DATA_LED_ON;
	if (printBufUsage == PRINTBUF_FULL) {
		if (stProtocol == 2) {	// XON/XOFF
			UART0_sendChar(XOFF);
			XOFFsent = TRUE;
		}
		if (stProtocol == 3) {	// RTS/CTS
			PORTA |= (1<<PA7);	// pull /RTS high
		}
	}
}

ISR (USART1_RX_vect)	// receive from typewriter
{
uint8_t c;
	c = UDR1;
	if (c == CODE_H) HexToggle = TRUE;
	if (c == CODE_I) SetupToggle = TRUE;
	// reset criteria for line counter:
	if (c == 0x83) LineCounter = 0;	// paper feed button pressed
	if (c == 0x75) LineCounter = 0; // half line down
	//
	if (stSend) {
		if (c < 0x7A) {	// filter out control chars > 78h
			keyBuf[keyInPtr++] = c;
			keyBufUsage++;
		}
	}
}
