#include <compiler_defs.h>
#include <C8051F020_defs.h> // Include SFR declarations


unsigned int currfloor =0;
unsigned int elev_load = 0;
char seg_code[]={0x3f,0x06,0x5b,0x4f};
sbit updown = P1^0;
sbit relayenable = P1^1;
sbit IR1 = P1^6;
sbit IR2 = P1^7;

//P1MDIN |= 0xFF;

void DELAY_ms(unsigned int ms_Count)
{
    unsigned int i,j;
    for(i=0;i<ms_Count;i++)
    {
        for(j=0;j<100;j++);
    }
}


void init_intClock(void)
{
	OSCXCN = 0x00;
	// Bits 6-4 is 000 to disable it , else > don't care
	OSCICN = 0x14; // should be put as 0x14 to make it work	 but originally is it 0x04
	while (!(OSCICN & 0x10)); // Wait till IFRDY ( bit no 4 ) pin is set	, not working ,why if ^4 is 0
	/* Missing Clock Enable Bit(7) is 0 ( disabled) 
	  bits [ 1 : 0] are 10 for 2MHz
	  bit (2) is 1 to enable internal clock
	  bit (3) is 0 to disable external clock
	  */
}




// P1 digital o/p
void ext_int(void) interrupt 0 //\int0
{
while (currfloor > 0){ 
currfloor = currfloor -1;
//get the hex code from LUT and show on
relayenable = 1; // relay vdd
updown = 0; // going down relay 
DELAY_ms(1000);
P2 = seg_code[currfloor];
//show on 7 segment LUT
relayenable = 0;
}
} 


void ext_int3(void) interrupt 3 //t1
{
while (currfloor < 3)
{ 
currfloor = currfloor +1;
//show on 7 segment LUT
P2 = seg_code[currfloor];
relayenable = 0; // relay vdd
updown = 1; // going up relay
DELAY_ms(1000);
}
updown = 0;
} 

void ext_int1(void) interrupt 1
{
if (currfloor == 0){ 
currfloor = 1 ; 
relayenable = 0; // relay vdd
updown = 1 ;
DELAY_ms(1000);
P2 = seg_code[currfloor];
}
else { while (currfloor >1){
relayenable = 1; // relay vdd
updown = 0 ;
currfloor = currfloor -1 ;
DELAY_ms(1000); 
P2 = seg_code[currfloor];

}
}
currfloor= 1 ;
updown = 0;
relayenable =0;
}

void ext_int4(void) interrupt 5 //floor 2 timer2
{
if (currfloor == 3){ currfloor = 2; // relay vdd
updown = 0 ;
relayenable = 1;
DELAY_ms(1000); ;
P2 = seg_code[currfloor];  

}
else { while (currfloor <2){
currfloor = currfloor +1 ; 
relayenable=0; // relay vdd
updown=1 ;
DELAY_ms(1000); 
P2 = seg_code[currfloor];  
updown = 0;
}
}
relayenable = 0;
TF2=0;
}


void main (void)
{
TMOD = 0x66 ;//use timers as external interrupt

EA = 0; 
WDTCN = 0xde;
WDTCN = 0xad;
init_intClock();
EA = 1;
EX0=1; 

TL0 = 0xff;
TH0 = 0xff;
TL1 = 0xff;
TH1 = 0xff;

TR0= 1; // start count.
TR1= 1;
IT0 = 1;
IT1 = 1;
ET0 = 1;
ET1 = 1;

XBR2 = 0x40;
XBR1 = 0x3E; // enable T0, IT0, T1, IT1   
XBR0 = 0x04; // Tx and Rx
P0 = 0xff;
P1 |= 0xff;
P2MDOUT = 0xff;
//Timer 2
T2CON = 0x02;
RCAP2 = 0xffff;
TL2 = 0xff;
TH2 = 0xff;
T2CON = 0x06;
ET2 = 1;
P1MDOUT = 0x20;
//P2 = 0xff;
while(1){}
}
