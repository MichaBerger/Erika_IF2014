/*
  Robotron Erika S3004
  Firmware für Steuerplatine, Atmega 1284p
  M.Berger, Dezember 2013
*/

#include "main.h"

// local prototypes
void waitForContinueKey();

int main(void)
{
uint8_t	c;
	init();
	sei();						// allow interrupts
// *********************
//	testcode();
// *********************
	while (1)
	{
		if (keyBufUsage) sendKey();
		if (HexToggle) HexDumpMode();
		if (SetupToggle) Setup();
		if (printBufUsage) {
			cli();
			printBufUsage--;
			sei();
			c = printBuf[outPtr++];
			if (outPtr == PRINTBUFSIZE) outPtr = 0;
			printChar(c);
		}
		else {	// no data in buffer
			DATA_LED_OFF; 
			testLocalKey();
			testSendKey();
		}	
	}
} // end of main


// filter and print char
void printChar(uint8_t c)
{
	if (printBufUsage < PRINTBUF_LOW) {
		if (XOFFsent && (stProtocol == 2)) {	// XON/XOFF
			UART0_sendChar(XON);
			XOFFsent = FALSE;
		}
		if (stProtocol == 3) {		// RTS/CTS
			PORTA &= ~(1<<PA7);		// pull /RTS low
		}
	}

	if (stTranslate == RAW) {
		UART1_sendChar(c);	// raw mode: print char untranslated
	}
	else {	// translate from ASCII to Erika specific codes
		if (c > 31) {			// printable char, except DEL
			printTranslatedChar(c);
			if (BoldFlag) {
				UART1_sendChar(0xA5);
				UART1_sendChar(1);		// 1 MicroStep ->
				UART1_sendChar(114);	// BackSpace
				printTranslatedChar(c);	// print Char again
				UART1_sendChar(0xA5);
				UART1_sendChar(255); 	// 1 Microstep <-
			}
			if (UnderlineFlag) {
				UART1_sendChar(114);	// BackSpace
				UART1_sendChar(1);		// Underline
			}
		}
		else {	// perform actions for control chars
			if (c == CR) {
				if (stCRfromHost == 2) UART1_sendChar(ENEWLINE);// CR --> NewLine
				else UART1_sendChar(0x78);						// CR --> CR	
			}
			if (c == LF) UART1_sendChar(0x9F);	// Line Feed		
			if (c == 0x09) UART1_sendChar(121);	// TAB
			if (c == 0x08) UART1_sendChar(114);	// Backspace
			if (c == 27) EscActions();
			// all other chars masked out for now!
		}
		// stop at page end only in ASCII mode:
		if (c == CR) LineCounter++;
		if (c == FORMFEED) LineCounter = MaxLines;	// Form Feed
		if (MaxLines && (LineCounter == MaxLines)) {
			LineCounter = 0;
			waitForContinueKey();
		}
	}
}	// end of printChar

void printTranslatedChar(uint8_t c)
{
uint8_t i,x;
	x = chartrans[c-32];
	if (x != 255) {			// direct 1:1 translations
		UART1_sendChar(x);
	}
	else {					// complex substitutions
		switch(c) {
		case('^'):	// ^
			UART1_sendChar(25);	
			UART1_sendChar(113); // need to add space
			break;
		case('<'):
			for (i=0;i<5;i++) UART1_sendChar(0x81);	// down
			UART1_sendChar(41);	// ´
			for (i=0;i<3;i++) UART1_sendChar(0x81);
			UART1_sendChar(43);	// `
			for (i=0;i<8;i++) UART1_sendChar(0x82);	// up
			UART1_sendChar(113);					// space
			break;
		case('>'):
			for (i=0;i<5;i++) UART1_sendChar(0x81);	// down
			UART1_sendChar(43);	// `
			for (i=0;i<3;i++) UART1_sendChar(0x81);
			UART1_sendChar(41);	// ´
			for (i=0;i<8;i++) UART1_sendChar(0x82);	// up
			UART1_sendChar(113);					// space
			break;
		case('['):
			UART1_sendChar(23);		// '
			UART1_sendChar(0x72);	// Backspace
			for (i=0;i<6;i++) UART1_sendChar(0x81);	// down
			UART1_sendChar(23);		// '
			for (i=0;i<6;i++) UART1_sendChar(0x82);	// up
			UART1_sendChar(0x72);	// Backspace
			UART1_sendChar(0xA5);	// x microsteps horiz:
			UART1_sendChar(2);
			UART1_sendChar(99);		// .
			UART1_sendChar(0xA5);	// x microsteps hor.
			UART1_sendChar(-2);
			UART1_sendChar(0x76);	// schritt hoch
			UART1_sendChar(0x72);	// backspace
			UART1_sendChar(0xA5);	// x microsteps horiz:
			UART1_sendChar(2);
			UART1_sendChar(99);		// .
			UART1_sendChar(0xA5);	// x microsteps hor.
			UART1_sendChar(-2);
			UART1_sendChar(0x75);	// schritt runter
			break;
		case(']'):
			UART1_sendChar(23);		// '
			UART1_sendChar(0x72);	// Backspace
			for (i=0;i<6;i++) UART1_sendChar(0x81);	// down
			UART1_sendChar(23);		// '
			for (i=0;i<6;i++) UART1_sendChar(0x82);	// up
			UART1_sendChar(0x72);	// Backspace
			UART1_sendChar(0xA5);	// x microsteps horiz:
			UART1_sendChar(-2);
			UART1_sendChar(99);		// .
			UART1_sendChar(0xA5);	// x microsteps hor.
			UART1_sendChar(2);
			UART1_sendChar(0x76);	// schritt hoch
			UART1_sendChar(0x72);	// backspace
			UART1_sendChar(0xA5);	// x microsteps horiz:
			UART1_sendChar(-2);
			UART1_sendChar(99);		// .
			UART1_sendChar(0xA5);	// x microsteps hor.
			UART1_sendChar(2);
			UART1_sendChar(0x75);	// schritt runter
			break;
		case('{'):
			UART1_sendChar(29);		// (
			UART1_sendChar(0x72);	// backspace
			for (i=0;i<5;i++) UART1_sendChar(0x82);	// up
			UART1_sendChar(0xA5);	// x microsteps horiz:
			UART1_sendChar(-2);
			UART1_sendChar(99);		// .
			UART1_sendChar(0xA5);	// x microsteps horiz:
			UART1_sendChar(2);
			for (i=0;i<5;i++) UART1_sendChar(0x81);	// down
			break;
		case('}'):
			UART1_sendChar(31);		// )
			UART1_sendChar(0x72);	// backspace
			for (i=0;i<5;i++) UART1_sendChar(0x82);	// up
			UART1_sendChar(0xA5);	// x microsteps horiz:
			UART1_sendChar(2);
			UART1_sendChar(99);		// .
			UART1_sendChar(0xA5);	// x microsteps horiz:
			UART1_sendChar(-2);
			for (i=0;i<5;i++) UART1_sendChar(0x81);	// down
			break;
		case('~'):
			for (i=0;i<5;i++) UART1_sendChar(0x81);	// down
			UART1_sendChar(0xA5);	// x microsteps horiz:
			UART1_sendChar(-3);
			UART1_sendChar(41);		// ´
			UART1_sendChar(0xA5);	// x microsteps horiz:
			UART1_sendChar(5);
			UART1_sendChar(41);		// ´
			UART1_sendChar(0xA5);	// x microsteps horiz:
			UART1_sendChar(-2);		// balance to +- 0
			for (i=0;i<5;i++) UART1_sendChar(0x82);	// up
			UART1_sendChar(113);	// space
			break;
		case('\\'):
			UART1_sendChar(0xA5);	// x microsteps horiz:
			UART1_sendChar(3);
			UART1_sendChar(99);		// .
			for (i=1;i<4;i++) {
				UART1_sendChar(0x72);	// backspace
				UART1_sendChar(0xA5);	// x microsteps horiz:
				UART1_sendChar(-1);
				UART1_sendChar(0x82);	// up
				UART1_sendChar(0x82);	// up
				UART1_sendChar(0x82);	// up
				UART1_sendChar(99);		// .
			}
			for (i=0;i<9;i++) UART1_sendChar(0x81);	// adj. vert. msteps
			break;
		case('@'):
			UART1_sendChar(32);		// C
			UART1_sendChar(0x72);	// backspace
			for (i=0;i<3;i++) UART1_sendChar(0x82);	// up
			UART1_sendChar(97);		// a
			for (i=0;i<3;i++) UART1_sendChar(0x81);	// down
			break;
		}
	}
}	// end of printTranslatedChar

void EscActions(void)
{
char c;
	c = waitForHostChar();
	switch (c) {
	case 'N':
		UART1_sendChar(0x87); break;	// 10 chars per inch
	case 'C':
		UART1_sendChar(0x88); break;	// 12 chars per inch
	case '1':
		UART1_sendChar(0x84); break;	// line distance 1
	case '2':
		UART1_sendChar(0x85); break;	// line distance 1.5
	case '3':
		UART1_sendChar(0x86); break;	// line distance 2
	case 'H':
		UART1_sendChar(0x76); break;	// half line up
	case 'L':
		UART1_sendChar(0x75); break;	// half line down
	case 'U':
		c = waitForHostChar();
		if (c == '1') UnderlineFlag = TRUE;
		if (c == '0') UnderlineFlag = FALSE;
		break;
	case 'B':
		c = waitForHostChar();
		if (c == '1') BoldFlag = TRUE;
		if (c == '0') BoldFlag = FALSE;
		break;
	}
}

char waitForHostChar(void)
{
char c;
	while (!printBufUsage)	// wait for next incoming char
	{
	}
	cli();
	printBufUsage--;
	sei();
	c = printBuf[outPtr++];
	if (outPtr == PRINTBUFSIZE) outPtr = 0;
	return c;
}

void UART1_sendChar(uint8_t c)		// to typewriter
{
	while (PINA & (1<<PA0))	// wait for /RTS to go low
	{
	}
    while (!(UCSR1A & (1<<UDRE1))) 	// wait until send possible
	{
	}	                       
    UDR1 = c;                      	// send char
	while (!(PINA & (1<<PA0)))				// wait until Erika is ready
	{
	}
}	// end of UART1_sendChar

void sendKey(void)	// to host
{
uint8_t c,x;
	cli();
	c = keyBuf[keyOutPtr++];
	keyBufUsage--;
	sei();
	if (c == ENEWLINE) {
		UART0_sendChar(CR);
		if (stCRtoHost == 2) UART0_sendChar(LF);	// CR --> CR LF translation
	}
	else if (revtrans[c]) {
		x = revtrans[c]; 
		UART0_sendChar(x);
	}
	// printHexCode(c);		// M.B. TEST CODE!!!
}	// end of sendKey

void UART0_sendChar(uint8_t c)	// to host
{
    while (!(UCSR0A & (1<<UDRE0))) 	// wait until send possible
	{
	}	                       
    UDR0 = c;
}	// end of UART0_sendChar

void printHexCode(uint8_t c)	// Hex Code, followed by Space
{
uint8_t x;
	x = (c>>4) + '0';
	if (x>'9') x = x + 7;
	printChar(x);
	x = (c & 0x0F) + '0';
	if (x>'9') x = x + 7;
	printChar(x);
	printChar(' ');
}

void HexDumpMode(void)
{
char a[] = "................";
char c;
uint8_t i;
uint8_t cnt = 0;	// counter for chars per line
uint8_t leftover;
	HexToggle = FALSE;
	UART1_sendChar(ENEWLINE);
	do {
		if (printBufUsage) {
			cli();
			printBufUsage--;
			sei();
			c = printBuf[outPtr++];
			if (outPtr == PRINTBUFSIZE) outPtr = 0;
			if ((c > 31) && (c != 127)) a[cnt] = c;
			printHexCode(c);
			cnt++;
		}	
		if (cnt == 16) {
			cnt = 0;
			printChar(' ');
			for (i=0;i<16;i++) {
				printChar(a[i]);
				a[i] = '.';
			}
			UART1_sendChar(ENEWLINE);
		}
	} while (!HexToggle);
	HexToggle = FALSE;
	// print the remaining ASCII chars:
	if (cnt < 16) {
		leftover = 16 - cnt;
		for (i=0;i<leftover;i++) {
			printChar(' ');
			printChar(' ');
			printChar(' ');
		}
		printChar(' ');
		for (i=0;i<cnt;i++) {
			printChar(a[i]);
		}
	}
	UART1_sendChar(ENEWLINE);
}	// end of HexDumpMode

// *************************

void testcode(void)
{
uint8_t i,j;
	for (i=0;i<12;i++) {
		for (j=0;j<10;j++) {
			UART1_sendChar(0xA9);
			UART1_sendChar(i*10+j);
			UART1_sendChar(0x71);
			UART1_sendChar(0x71);
		}
		UART1_sendChar(ENEWLINE);
	}
} // end of testcode

void printString(char *s)
{
	while (*s) {
		printChar(*s++);
	} 
}

void printStringCR(char *s)
{
	printString(s);
	UART1_sendChar(ENEWLINE);
}

void waitForContinueKey()
{
	PORTD |= (1<<PD5);	// switch Cont LED on
	while (PINC & (1<<CONT_BTN))
	{ }					// wait for CONT Key to be pressed
	PORTD &= ~(1<<PD5);	// switch Cont LED off
}
