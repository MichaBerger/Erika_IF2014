// interface setup

#include "main.h"

uint8_t		BaudSetting		EEMEM;
uint8_t		ProtocolSetting	EEMEM;
uint8_t		CRfromHost		EEMEM;
uint8_t		CRtoHost		EEMEM;
uint8_t		LocalSetting	EEMEM;
uint8_t		SendSetting		EEMEM;
uint8_t		TransSetting	EEMEM;
uint8_t		LinesPerPage	EEMEM;

// local prototypes
uint8_t getChoice(char * s);
uint8_t getNumber(char * s);
uint8_t LPP();	// lines per page
void printCurrentSettings(uint8_t i);

void Setup(void)
{
uint8_t a;
	// first set Local echo off, ASCII
	stLocal = FALSE;
	LOCAL_LED_OFF;
	UART1_sendChar(LOCAL_OFF);
	RememberTrans = stTranslate;	// remember translation setting
	stTranslate = ASCII;

	printString("Firmware V");printStringCR(VERSION);

	printStringCR("Interface Setup");
	printStringCR("---------------");
	UART1_sendChar(ENEWLINE);
	printStringCR("Current Settings:");
	printString(" 1  Baud Rate       ");
	printCurrentSettings(1);
	printString(" 2  Handshake       ");
	printCurrentSettings(2);
	printString(" 3  Lines per Page  ");
	printCurrentSettings(3);
	printString(" 4  Character Set   ");
	printCurrentSettings(4);
	printString(" 5  CR from Host    ");
	printCurrentSettings(5);
	printString(" 6  CR to Host      ");
	printCurrentSettings(6);
	
	UART1_sendChar(ENEWLINE);
	printStringCR(" Press (1) to (6) to change one item, (a) to change all,");
	printString(" (x) to leave setup without changes: ");
	a = getChoice("123456ax");
	if (a != 'x') {
		if ((a == '1') || (a == 'a')) {
			printStringCR("Baud Rate:");
			printStringCR(" 1  1200");
			printStringCR(" 2  9600");
			printStringCR(" 3  115200");
			printString(" Press (1) to (3) to select: ");
			stBaud = getChoice("123") - '0';
			eeprom_update_byte(&BaudSetting,stBaud);
		}
		if ((a == '2') || (a == 'a')) {
			printStringCR("Handshake:");
			printStringCR(" 1  None");
			printStringCR(" 2  XON/XOFF");
			printStringCR(" 3  RTS/CTS");
			printString(" Press (1) to (3) to select: ");
			stProtocol = getChoice("123") - '0';
			eeprom_update_byte(&ProtocolSetting,stProtocol);
		}
		if ((a == '3') || (a == 'a')) {
			printStringCR("Lines per Page, before Stop:");
			printString(" Enter as 2 digit number, 00 for no Stop: ");
			MaxLines = (getNumber("0123456789") - '0') * 10;
			MaxLines += getNumber("0123456789") - '0';
			UART1_sendChar(ENEWLINE);
			UART1_sendChar(ENEWLINE);
			eeprom_update_byte(&LinesPerPage,MaxLines);
		}
		if ((a == '4') || (a == 'a')) {
			printStringCR("Character Set:");
			printStringCR(" 1  ASCII");
			printStringCR(" 2  raw");
			printString(" Press (1) or (2) to select: ");
			RememberTrans = getChoice("12") - '0';
			eeprom_update_byte(&TransSetting,RememberTrans);
		}
		if ((a == '5') || (a == 'a')) {
			printStringCR("CR from Host Translation:");
			printStringCR(" 1  CR");
			printStringCR(" 2  CR LF");
			printString(" Press (1) or (2) to select: ");
			stCRfromHost = getChoice("12") - '0';
			eeprom_update_byte(&CRfromHost,stCRfromHost);
		}
		if ((a == '6') || (a == 'a')) {
			printStringCR("CR to Host Translation:");
			printStringCR(" 1  CR");
			printStringCR(" 2  CR LF");
			printString(" Press (1) or (2) to select: ");
			stCRtoHost = getChoice("12") - '0';
			eeprom_update_byte(&CRtoHost,stCRtoHost);
		}
	}
	SetupToggle = FALSE;
	init();	// load settings

}	// end of Setup

uint8_t getChoice(char * s)
{
uint8_t a;
	a = getNumber(s);
	UART1_sendChar(ENEWLINE);
	UART1_sendChar(ENEWLINE);
	return a;
}

uint8_t getNumber(char * s)
{
uint8_t	a,c,ok;
char * here;
	ok = FALSE;
	do {
		here = s;
		while (!keyBufUsage) { }
		cli();
		c = keyBuf[keyOutPtr++];
		a = revtrans[c];		// translate to ASCII
		keyBufUsage--;
		sei();
		while (*here) {
			if (a == *here) ok = TRUE;
			here++;
		}
	} while (!ok); // wait for a valid key
	printChar(a);
	return a;
}

uint8_t LPP()
{
uint8_t a,c,nLines;
	BoldFlag = TRUE;
	printString("# of Lines per Sheet, before Stop:");
	BoldFlag = FALSE;
	UART1_sendChar(ENEWLINE);	
	printString("enter as 2 Digits Number, 00 for continuous printing: ");
// -- digit 1
	do {
		while (!keyBufUsage) { }
		cli();
		c = keyBuf[keyOutPtr++];
		a = revtrans[c];		// translate to ASCII
		keyBufUsage--;
		sei();
	} while ((a < '0') || (a > '9')); // wait for a valid key
	printChar(a);
	nLines = 10 * (a - '0');
// -- digit 2
	do {
		while (!keyBufUsage) { }
		cli();
		c = keyBuf[keyOutPtr++];
		a = revtrans[c];		// translate to ASCII
		keyBufUsage--;
		sei();
	} while ((a < '0') || (a > '9')); // wait for a valid key
	printChar(a);
	nLines += a - '0';
// --
	UART1_sendChar(ENEWLINE);
	return nLines;
}	// end of LPP

void testLocalKey()
{
static uint16_t count = 0;
static uint8_t scan = TRUE;
	if (!(PINC & (1<<LOCAL_BTN)) && scan) {
		count++;
		if (count == COUNTMAX) {
			count = 0;
			scan = FALSE;
			stLocal = ~stLocal;			// invert Local status
			if (stLocal) {
				LOCAL_LED_ON;
				UART1_sendChar(LOCAL_ON);
				eeprom_update_byte(&LocalSetting,TRUE);
			}
			else {
				LOCAL_LED_OFF;
				UART1_sendChar(LOCAL_OFF);
				eeprom_update_byte(&LocalSetting,FALSE);
			}
		}
	}
	else count = 0;
	if (PINC & (1<<LOCAL_BTN)) scan = TRUE;
}	// end of testLocalKey

void testSendKey()
{
static uint16_t count = 0;
static uint8_t scan = TRUE;
	if (!(PINC & (1<<SEND_BTN)) && scan) {
		count++;
		if (count == COUNTMAX) {
			count = 0;
			scan = FALSE;
			stSend = ~stSend;			// invert SEND status
			if (stSend) {
				SEND_LED_ON;
				eeprom_update_byte(&SendSetting,TRUE);
			}
			else {
				SEND_LED_OFF;
				eeprom_update_byte(&SendSetting,FALSE);
			}
		}
	}
	else count = 0;
	if (PINC & (1<<SEND_BTN)) scan = TRUE;
}	// end of testSendKey

void printCurrentSettings(uint8_t i)
{
	switch(i) {
	case 1:
		if (stBaud == 1) printString("1200");
		if (stBaud == 2) printString("9600");
		if (stBaud == 3) printString("115200");		
		break;
	case 2:
		if (stProtocol == 1) printString("None");
		if (stProtocol == 2) printString("XON/XOFF");
		if (stProtocol == 3) printString("RTS/CTS");
		break;
	case 3:
		printChar(MaxLines/10+'0');
		printChar(MaxLines%10+'0');
		break;
	case 4:
		if (RememberTrans == 1) printString("ASCII");
		if (RememberTrans == 2) printString("raw");
		break;
	case 5:
		if (stCRfromHost == 1) printString("--> CR");
		if (stCRfromHost == 2) printString("--> CR LF");
		break;
	case 6:
		if (stCRtoHost == 1) printString("--> CR");
		if (stCRtoHost == 2) printString("--> CR LF");
		break;
	}
	UART1_sendChar(ENEWLINE);
}	// end of printCurrentSettings
