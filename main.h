#ifndef MAIN_H
#define MAIN_H

// constants
#define VERSION "1.2 1-Sep-2025"
#define F_CPU 18432000ul
//#define F_CPU     3686400ul

#define NBAUDRATES	7		// # of baudrate definitions

#define UART0_BAUD1 300ul
#define UART0_BAUD2 600ul
#define UART0_BAUD3 1200ul
#define UART0_BAUD4 2400ul
#define UART0_BAUD5 4800ul
#define UART0_BAUD6 9600ul
#define UART0_BAUD7 19200ul
#define DEFAULTBAUD 6

#define UART1_BAUD 1200ul

#define UART0_UBRR_VALUE1 ((F_CPU/(UART0_BAUD1<<4))-1)
#define UART0_UBRR_VALUE2 ((F_CPU/(UART0_BAUD2<<4))-1)
#define UART0_UBRR_VALUE3 ((F_CPU/(UART0_BAUD3<<4))-1)
#define UART0_UBRR_VALUE4 ((F_CPU/(UART0_BAUD4<<4))-1)
#define UART0_UBRR_VALUE5 ((F_CPU/(UART0_BAUD5<<4))-1)
#define UART0_UBRR_VALUE6 ((F_CPU/(UART0_BAUD6<<4))-1)
#define UART0_UBRR_VALUE7 ((F_CPU/(UART0_BAUD7<<4))-1)

#define UART1_UBRR_VALUE ((F_CPU/(UART1_BAUD<<4))-1)

#define ALLPINSOUT	0b11111111	// entire Atmel port to Output
#define ALLPINSIN	0b00000000	// entire Atmel port to Input
#define eRESET		0x95		// reset Erika
#define eNEWLINE	0x77		// Erika code for New Line
#define eLEFTMARGIN 0x7E		// Erika set left margin
#define CODE_Hex	0xD6		// Code + h on Erika keyboard
#define CODE_Setup	0xDD		// Code + i on Erika keyboard
#define PRINTBUFSIZE 14000		// size for print buffer
#define PRINTBUF_FULL PRINTBUFSIZE - 100
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
#define ASCII		1			// ASCII Char set + some Esc Sequences
#define RAW			2			// Typewriter Char Set
#define XON			17
#define XOFF		19

// libraries

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>


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
void FormFeed(uint8_t);

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
extern volatile uint8_t	HexTrigger;
extern volatile uint8_t	SetupTrigger;
extern volatile uint8_t DemoTrigger;

extern uint8_t	UnderlineFlag;
extern uint8_t	BoldFlag;

extern const uint8_t chartrans[];
extern const uint8_t revtrans[];

#endif	// MAIN_H
