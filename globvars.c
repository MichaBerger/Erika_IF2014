// global variable definitions

#include "main.h"

// status variables
uint8_t stBaud;
uint8_t stProtocol;
uint8_t stCRfromHost;
uint8_t stCRtoHost;
uint8_t stLocal;	
uint8_t stSend;
uint8_t stTranslate;
uint8_t RememberTrans;

uint8_t volatile LineCounter;
uint8_t MaxLines;

volatile uint8_t	XOFFsent;

uint8_t				printBuf[PRINTBUFSIZE];
volatile uint16_t	inPtr;
		 uint16_t	outPtr;
volatile uint16_t	printBufUsage;
uint8_t				keyBuf[KEYBUFSIZE];
volatile uint8_t	keyInPtr;
		 uint8_t	keyOutPtr;
volatile uint8_t	keyBufUsage;
volatile uint8_t	HexToggle;
volatile uint8_t	SetupToggle;
uint8_t				UnderlineFlag;
uint8_t				BoldFlag;


const uint8_t chartrans[] = {
 #include "CHARTRANS.TXT"	
};

const uint8_t revtrans[] = {
 #include "INVERS_CHAR.TXT"
};

