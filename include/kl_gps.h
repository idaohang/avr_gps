#include <avr/io.h>
#define F_CPU   16000000UL
#include <util/delay.h>

//set baudrate for RPI
#define baud 9600
#define FOSC 16000000UL
#define UBBRVAL (((FOSC / (baud * 16UL))) - 1) 

//set baudrate2 for GPS
#define baud2 4800
#define UBBRVAL2 (((FOSC / (baud2 * 16UL))) - 1)

#include <avr/interrupt.h>


#define TRUE		                    1
#define FALSE		                    0
#define RX_BUF_LEN                         128



//setup uarts 0 and 1 for atmega164pv 
static void USART_init_0(uint16_t ubrr);
static void USART_init_1(uint16_t ubrr);

static void USART_transmit_0(uint8_t data);
static void USART_transmit_1(uint8_t data);

static uint8_t USART_receive_0(void);
static uint8_t USART_receive_1(void);

//error functions
__inline void ack(void);
__inline void nack(void);
__inline void nackfull(void);






