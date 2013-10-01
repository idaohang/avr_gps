#include "../include/kl_gps.h"

/*

  GPS DEMO CODE - READ AND PROCESS SOME DATA 
  Interrupt driven UART ring buffer for atmega164p
  Keith Legg 8,8,2013


  // SAMPLE DATA LOOKS LIKE THIS :
  // $GPRMC,054123.000,A,4402.4926,N,12257.6825,W,0.00,152.38,080813,,,A*73

*/


volatile uint8_t bufferReady   = FALSE;
volatile uint8_t bufIndex      =     0;
volatile uint8_t uartRXBuf[RX_BUF_LEN];

void wait_begin(){
   uint8_t x =0;

   if (bufIndex==0){
       while(x!='\n'){
	  x = USART_receive_1(); 
       }
       bufferReady = FALSE;
       UCSR1B |= (1 << RXCIE1);  

    }

}

int main()
{
    // Set LED PB0-2 as output
    DDRB |= (1 << 0)|(1 << 1)|(1 << 2); 

    //turn on second interrupt - tx LED on
    UCSR0B |= (1 << TXCIE0);
    //turn on third interrupt - tx LED off
    UCSR0B |= (1 << RXCIE0);

    //activate UART hardware/enable interrupts
    USART_init_0(UBBRVAL);    
    USART_init_1(UBBRVAL2);
 
    //turn on global interrupts
    sei(); 
    
    uint8_t y =0;
  
   ////////////////////
   //activate interrupts when a new line begins

    wait_begin();

    while (TRUE)
    {

        if(bufferReady) {
           
            USART_transmit_0( 0x40 );
            send_string_0_2(uartRXBuf,64);
            
            //reset the buffer state 
            bufIndex    = 0;
            bufferReady = FALSE;

            wait_begin();
        }

        //beat LED heart  
        PORTB ^= (1 << 0); 
        _delay_ms(1000);

    }

    return 1;
}

///////////////
void send_string_0(char s[])
{
   int i =0;
   
   while (s[i] != 0x00)
   {
      USART_transmit_0(s[i]);
      i++;
   }
} 

void send_string_0_2(char s[],int num)
{
   int i =0;
   
   for (i=0;i<num;i++)
   {
      if (s[i]!=0x00){
         USART_transmit_0(s[i]);
      }
   }
   USART_transmit_0('\n');

} 

////////////////

//TEST OF SECOND INTERRUPT
ISR(USART0_TX_vect)
{
    PORTB &= 0b00000101;      //turn off  LED2,ignore 1+3
    //PORTB |= 0b00000010;      //turn on  LED2,ignore 1+3
}


///////////////
ISR(USART1_RX_vect, ISR_BLOCK)
{
    uint8_t c = UDR1;    
 
    PORTB |= 0b00000010; //TURN ON LED2

    //when we hit the end of the line
    if(c == '\n') { 
	   bufferReady = TRUE;
           // Disable interrupts untill process the buffer.
           UCSR1B&=~(1 << RXCIE1); //DMV way 

    }else if(bufIndex == (RX_BUF_LEN - 1)) {
		bufIndex = 0;
		nackfull(); //buffer is full error
		return;
    }


    uartRXBuf[bufIndex++] = c;
    //USART_transmit_0(c);  //fer debuggin
 
}

///////////////
void USART_init_0(uint16_t ubrr)
{
    // Load upper and lower bytes of baud rate into the UBRR register
    UBRR0H = (uint8_t)(ubrr>>8); 
    UBRR0L = (uint8_t)ubrr;      

    // Enable transmitter and receiver.
    UCSR0B |= ((1 << RXEN0)|(1 << TXEN0));

    // Enable the USART RX interrupt.
    //UCSR1B |= (1 << RXCIE1); // Enable the USART Recieve Complete interrupt (USART_RXC)

}


static void USART_transmit_0(uint8_t data)
{
    while (!(UCSR0A & (1 << UDRE0))) {/*Busy wait.*/}
    UDR0 = data;
}

static uint8_t USART_receive_0(void)
{
    while (!(UCSR0A & (1 << RXC0))) {/*Busy wait.*/}
    return UDR0;
}

///////////////

static void USART_init_1(uint16_t ubrr)
{
    UBRR1H = (uint8_t)(ubrr>>8); 
    UBRR1L = (uint8_t)ubrr;     

    // Enable transmitter and receiver.
    UCSR1B |= ((1 << RXEN1)|(1 << TXEN1));
}


static void USART_transmit_1(uint8_t data)
{
    while (!(UCSR1A & (1 << UDRE1))) {/*Busy wait.*/}
    UDR1 = data;
}


static uint8_t USART_receive_1(void)
{
    while (!(UCSR1A & (1 << RXC1))) {/*Busy wait.*/}
    return UDR1;
}



/**
* Sends the serial command acknowledge code.
*/
void ack(void)
{
    USART_transmit_0('O');
    USART_transmit_0('K');
    USART_transmit_0('\r');
    USART_transmit_0('\n');
}


/**
* Sends the serial command not acknowledge code.
*/
void nack(void)
{
    USART_transmit_0('E');
    USART_transmit_0('R');
    USART_transmit_0('R');
    USART_transmit_0('O');
    USART_transmit_0('R');
    USART_transmit_0('\r');
    USART_transmit_0('\n');
}

void nackfull(void)
{
    USART_transmit_0('B');
    USART_transmit_0('U');
    USART_transmit_0('F');
    USART_transmit_0('F');
    USART_transmit_0('R');
    USART_transmit_0('_');
    USART_transmit_0('F');
    USART_transmit_0('U');
    USART_transmit_0('L');
    USART_transmit_0('L');
    USART_transmit_0('\r');
    USART_transmit_0('\n');
}



