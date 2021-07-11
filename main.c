#include <msp430.h>
#include "gpio.h"

//Define our pins
#define DATA BIT2 //DS (SER) - P2.2
#define CLOCK BIT0 // SH_CP (SCK) - P1.0
#define LATCH BIT1 // ST_CP (RCK) - P1.1

// The OE pin can be tied directly to ground, but controlling
// it from the MCU lets you turn off the entire array without
// zeroing the register

// Declare functions

void pulseClock ( void );
void shiftOut ( unsigned char );
void pinWrite ( unsigned int, unsigned char );




const int Seg[10] = {
                     0b11000000,//0
                       0b11111001,//1
                       0b10100100,//2
                       0b10110000,//3
                       0b10011001,//4
                       0b10010010,//5
                       0b10000011,//6
                       0b11111000,//7
                       0b10000000,//8
                       0b10010000,//9
};

int main( void )
{
    WDTCTL = WDTPW + WDTHOLD;
        PMM_unlockLPM5();
    static int vitri =0;

    P1DIR |=  CLOCK +LATCH +BIT2 ;  // Setup pins as outputs
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6+GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6+GPIO_PIN7);
//    P1DIR &= ~BIT6;
//    P1DIR &= ~BIT7;

    P2DIR |=DATA;

    while(1){
        P1OUT &=~LATCH;
      shiftOut(Seg[vitri]);
      P1OUT |= LATCH;

      P1OUT ^=BIT2;
      vitri++;
      if (vitri ==10){
          vitri=0;
      }
      __delay_cycles(500000);
    }


    //Enter LPM4 w/interrupt
    __bis_SR_register(LPM4_bits + GIE);

    //For debugger
    __no_operation();


}

void shiftOut(unsigned char data1) {
 volatile int i;
 char temp, temp2;
#ifdef MSBFIRST // reverse order of the bits
 for (i = 0; i < 8; i++) {
   temp = (0x01 & data);
   data = data >> 1;
   temp2 = (temp2 << 1) + temp;
 }
 data = temp2;
#endif
 for (i = 0; i < 8; i++) {         // 8 bits to a char
   char bittowrite = (0x01 & data1); // get bit
   data1 = data1 >> 1; // shift data left
   if (bittowrite == 1) {  // send bit
     P2OUT |= DATA;
   } else {

     P2OUT &= ~DATA;
   }
   // pulse clockpin
   P1DIR |= CLOCK;
   P1DIR &= ~CLOCK;
 }

}
