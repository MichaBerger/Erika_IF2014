#ifndef MAIN_H
#define MAIN_H

// constants
#define VERSION "1.0 28-Feb-2014"
#define F_CPU 18432000ul

#define NBAUDRATES	3		// # of baudrate definitions

#define UART0_BAUD1 1200ul
#define UART0_BAUD2 9600ul
#define UART0_BAUD3 115200ul
#define DEFAULTBAUD 2

#define UART1_BAUD 1200ul

#define UART0_UBRR_VALUE1 ((F_CPU/(UART0_BAUD1<<4))-1)
#define UART0_UBRR_VALUE2 ((F_CPU/(UART0_BAUD2<<4))-1)
#define UART0_UBRR_VALUE3 ((F_CPU/(UART0_BAUD3<<4))-1)

#define UART1_UBRR_VALUE ((F_CPU/(UART1_BAUD<<4))-1)

#define ALLPINSOUT	0b11111111	// entire Atmel port to Output
#define ALLPINSIN	0b00000000	// entire Atmel port to Input
#define ERESET		0x95		// reset Erika
#define ENEWLINE	0x77		// Erika code for New Line
#define CODE_H		0xD6		// Code + h on Erika keyboard
#define CODE_I		0xDD		// Code + i on Erika keyboard
#define PRINTBUFSIZE 14500		// size for print buffer
#define PRINTBUF_FULL PRINTBUFSIZE / 3 * 2
#define PRINTBUF_LOW 10
#define KEYBUFSIZE	256			// keyboard buffer size, must be 256!

#define LOCAL_LED	7	
#define SEND_LED	6
#define CONT_LED	5
#define DATA_LED	4

#define LOCAL_LED_ON	PORTD |=  (1<<LOCAL_LED)
#define LOCAL_LED_OFF	PORTD &= ~(1<<LOCAL_LED)
#define SEND_LED_ON		PORTD |=  (1<<SEND_LED)
#define SEND_LED_OFF	PORTD &= ~(1<<SEND_LED)
#define CONT_LED_ON		PORTD |=  (1<<CONT_LED)
#define CONT_LED_OFF	PORTD &= ~(1<<CONT_LED)
#define DATA_LED_ON		PORTD |=  (1<<DATA_LED)
#define DATA_LED_OFF	PORTD &= ~(1<<DATA_LED)


#define LOCAL_BTN	2
#define SEND_BTN	1
#define CONT_BTN	0

#define	COUNTMAX	800			// threshold value for key debouncing 
#define	LOCAL_OFF	0x91		// disable direct print
#define LOCAL_ON	0x92		// enable direct print
#define FALSE		0
#define TRUE		~FALSE
#define CR			13			// Carriage Return
#define LF			10			// Line Feed
#define FORMFEED	12			// Form Feed
#define ASCII		1			// ASCII vs. RAW charset
#define RAW			2		
#define XON			17
#define XOFF		19

// libraries

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>

// function prototypes
void init(void);
void printChar(uint8_t c);
void printTranslatedChar(uint8_t c);
void UART0_sendChar(uint8_t c);
void UART1_sendChar(uint8_t c);
void sendKey(void);
void testLocalKey(void);
void testSendKey(void);
void printHexCode(uint8_t c);
void HexDumpMode(void);
void EscActions(void);
char waitForHostChar(void);
void printString(char *s);
void printStringCR(char *s);
void Setup(void);

// status variables, these are being read from EEPROM after power on
extern uint8_t stBaud;
extern uint8_t stProtocol;
extern uint8_t stCRfromHost;
extern uint8_t stCRtoHost;
extern uint8_t stLocal;	
extern uint8_t stSend;
extern uint8_t stTranslate;
extern uint8_t RememberTrans;

extern volatile uint8_t LineCounter;
extern uint8_t MaxLines;

extern volatile uint8_t		XOFFsent;

extern 			uint8_t		printBuf[PRINTBUFSIZE];
extern volatile uint16_t	inPtr;
extern 			uint16_t	outPtr;
extern volatile uint16_t	printBufUsage;

extern uint8_t				keyBuf[KEYBUFSIZE];
extern volatile uint8_t	keyInPtr;
extern uint8_t	keyOutPtr;
extern volatile uint8_t	keyBufUsage;
extern volatile uint8_t	HexToggle;
extern volatile uint8_t	SetupToggle;

extern uint8_t	UnderlineFlag;
extern uint8_t	BoldFlag;

extern const uint8_t chartrans[];
extern const uint8_t revtrans[];

#endif	// MAIN_H
