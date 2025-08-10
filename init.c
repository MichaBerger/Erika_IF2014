// initialization functions

#include "main.h"

// EEPROM variables, defined in setup.c
extern uint8_t		BaudSetting		EEMEM;
extern uint8_t		ProtocolSetting	EEMEM;
extern uint8_t		CRfromHost		EEMEM;
extern uint8_t		CRtoHost		EEMEM;
extern uint8_t		LocalSetting	EEMEM;
extern uint8_t		SendSetting		EEMEM;
extern uint8_t		TransSetting	EEMEM;
extern uint8_t		LinesPerPage	EEMEM;

// header declarations
void initPorts(void);
void initUARTs(void);
void initStatus(void);
void initMargins(void);

void init(void)	// main init
{
	initPorts();
	initUARTs();
	initStatus();
	initMargins();
}	// end of init


void initPorts(void)
{
	DDRA = ALLPINSIN | (1<<PA7);
	DDRB = ALLPINSIN;
	DDRC = ALLPINSIN;
	DDRD = ALLPINSIN | (1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7);

	PORTA = 0xFF & ~(1<<PA7);	// internal pull ups active, outgoing /RTS low
	PORTB = 0xFF;
	PORTC = 0xFF;
	PORTD |= 0x0F;
		
}	// end of initPorts

void initUARTs(void)
{
uint8_t cbr;	// cuurent baud rate for host interface
	cbr = eeprom_read_byte(&BaudSetting);
	if ((cbr < 1) || (cbr > NBAUDRATES)) cbr = DEFAULTBAUD;
	// Set baud rates
	switch (cbr) {
	case 1:
		UBRR0H = (uint8_t) (UART0_UBRR_VALUE1>>8);
		UBRR0L = (uint8_t) UART0_UBRR_VALUE1;
		break;
	case 2:
		UBRR0H = (uint8_t) (UART0_UBRR_VALUE2>>8);
		UBRR0L = (uint8_t) UART0_UBRR_VALUE2;
		break;
	case 3:
		UBRR0H = (uint8_t) (UART0_UBRR_VALUE3>>8);
		UBRR0L = (uint8_t) UART0_UBRR_VALUE3;
		break;
	case 4:
		UBRR0H = (uint8_t) (UART0_UBRR_VALUE4>>8);
		UBRR0L = (uint8_t) UART0_UBRR_VALUE4;
		break;
	case 5:
		UBRR0H = (uint8_t) (UART0_UBRR_VALUE5>>8);
		UBRR0L = (uint8_t) UART0_UBRR_VALUE5;
		break;
	case 6:
		UBRR0H = (uint8_t) (UART0_UBRR_VALUE6>>8);
		UBRR0L = (uint8_t) UART0_UBRR_VALUE6;
		break;
	case 7:
		UBRR0H = (uint8_t) (UART0_UBRR_VALUE7>>8);
		UBRR0L = (uint8_t) UART0_UBRR_VALUE7;
		break;
	}
	UBRR1H = (uint8_t) (UART1_UBRR_VALUE>>8);
	UBRR1L = (uint8_t) UART1_UBRR_VALUE;


	// UART0 connected to Host: transmit, receive, interrupt
	UCSR0B = (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0);
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);	// 8N1 for interface 0
	// UART1 connected to Typewriter: transmit, receive, interrupt
	UCSR1B = (1<<TXEN1) | (1<<RXEN1) | (1<<RXCIE1);
	UCSR1C = (1<<UCSZ11) | (1<<UCSZ10); // 8N1 for interface 1

}	// end of initUARTs

void initStatus(void)
{
	stBaud = eeprom_read_byte(&BaudSetting);
	if ((stBaud < 1) || (stBaud > NBAUDRATES)) stBaud = 6;	// 9600 Baud as default
	stProtocol = eeprom_read_byte(&ProtocolSetting);
	if ((stProtocol < 1) || (stProtocol > 3)) stProtocol = 1;	// no flow control
	stTranslate = eeprom_read_byte(&TransSetting);	// ASCII or RAW
	if ((stTranslate < 1) || (stTranslate > 2)) stTranslate = 1;
	stCRfromHost = eeprom_read_byte(&CRfromHost);
	if ((stCRfromHost < 1) || (stCRfromHost > 2)) stCRfromHost = 2;
	stCRtoHost = eeprom_read_byte(&CRtoHost);
	if ((stCRtoHost < 1) || (stCRtoHost > 2)) stCRtoHost = 1;

	stLocal = eeprom_read_byte(&LocalSetting);	// allow direct typing
	if (stLocal != FALSE) stLocal = TRUE;
	stSend = eeprom_read_byte(&SendSetting);	// send keyboard input to host
	if (stSend != FALSE) stSend = TRUE;

	if (stProtocol == 2) UART0_sendChar(XON);	// send initial XON

	MaxLines = eeprom_read_byte(&LinesPerPage);	// lines before stop
	if (MaxLines > 99) MaxLines = 62;			// initial value
	LineCounter = 0;	// reset Line Counter
	inPtr = 0;
	outPtr = 0;
	printBufUsage = 0;
	keyInPtr = 0;
	keyOutPtr = 0;
	keyBufUsage = 0;

	XOFFsent = FALSE;
	HexToggle = FALSE;
	SetupToggle = FALSE;
	UnderlineFlag = FALSE;
	BoldFlag = FALSE;

	if (stLocal) LOCAL_LED_ON;
	else LOCAL_LED_OFF;
	if (stSend) SEND_LED_ON;
	else SEND_LED_OFF;
	CONT_LED_OFF;
	DATA_LED_ON;
	UART1_sendChar(ERESET);		// reset Erika
	if (stLocal) UART1_sendChar(LOCAL_ON);	// local keyboard echo on
	else UART1_sendChar(LOCAL_OFF);
}	// end of initStatus

void initMargins(void)	// set margins for typewriter
{
uint8_t i;
	UART1_sendChar(0x80);	// release margin
	for (i=0;i<9;i++) UART1_sendChar(0x72);	// back space
	UART1_sendChar(0x7E);	// set left margin
	for (i=0;i<74;i++) UART1_sendChar(0x71);	// space
	UART1_sendChar(0x80);	// release margin
	for (i=0;i<20;i++) UART1_sendChar(0x71);	// space
	UART1_sendChar(0x7F);	// set right margin
	UART1_sendChar(0x78);	// carriage return
}
