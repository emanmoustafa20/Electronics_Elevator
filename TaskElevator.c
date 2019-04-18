#include <compiler_defs.h>
#include <C8051F020_defs.h> // Include SFR declarations

char seven_segment[]={0x3f,0x06,0x5b,0x4f}; // lookup table for the seven segment from 0 to 4
unsigned int current_floor =0; // initialization of the floor number variable 
unsigned int elev_load = 0;

sbit enable1 = P1^0;
sbit enable2 = P1^1;
sbit IR1 = P1^6; // IR sensor 1 to check when going in first   /////////////
sbit IR2 = P1^7; // IR sensor 1 to check when going in thrn     //////////
sbit buzzer = P1^2;
unsigned int people = 0 ;





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
	OSCXCN = 0x00; //external clk special fn register
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
void ext_int(void) interrupt 0 //\int0 //p0.2// external interrupt
{
while (current_floor > 0){ 
current_floor = current_floor -1;
//get the hex code from LUT and show on
enable2 = 1; // relay vdd
enable1 = 0; // going down relay 
DELAY_ms(1000);
P2 = seven_segment[current_floor];
//show on 7 segment LUT
enable2 = 0; //  shut the motor
}
} 


void ext_int3(void) interrupt 3 //t1
{
while (current_floor < 3)
{ 
current_floor = current_floor +1;
//show on 7 segment LUT
P2 = seven_segment[current_floor];
enable2 = 0; // relay vdd
enable1 = 1; // going up relay
DELAY_ms(1000);
}
enable1 = 0;
} 

void ext_int1(void) interrupt 1 // timer 1
{
if (current_floor == 0){ 
current_floor = 1 ; 
enable2 = 0; // relay vdd
enable1 = 1 ;
DELAY_ms(1000);
P2 = seven_segment[current_floor];
}
else { while (current_floor >1){
enable2 = 1; // relay vdd
enable1 = 0 ;
current_floor = current_floor -1 ;
DELAY_ms(1000); 
P2 = seven_segment[current_floor];

}
}
current_floor= 1 ;
enable1 = 0;
enable2 =0;
// reset flag automatic 
}

void ext_int4(void) interrupt 5 //floor 2 timer2
{
if (current_floor == 3){current_floor= 2; // relay vdd
enable1= 0 ;
enable2 = 1;
DELAY_ms(1000); ;
P2 = seven_segment[current_floor];  

}
else { while (current_floor <2){
current_floor = current_floor+1 ; 
enable2=0; // relay vdd
enable1=1 ;
DELAY_ms(1000); 
P2 = seven_segment[current_floor];  
enable1= 0;
}
}
enable2= 0;
TF2=0; // reset flag of the timer2
}

void IR(void) interrupt 6 // using timer 4 
{
	buzzer=0;
	DELAY_ms(1000);
	buzzer=0;
	T4CON = 0x06;// set flag to zero
}


void main (void)
{
TMOD = 0x66 ;//use timers as external interrupt

EA = 0; 
WDTCN = 0xde;
WDTCN = 0xad;
init_intClock();
EA = 1; // enable all interrupts
EX0=1; //enable interupt 0

TL0 = 0xff;
TH0 = 0xff;
TL1 = 0xff;
TH1 = 0xff;
TL2 = 0xff;
TH2 = 0xff;
TL4 = 0xfc;
TH4 = 0xff;

TR0= 1; // start counting 
TR1= 1;
IT0 = 1; // edge triggered for interrupt 0
ET0 = 1; // enable timer 0 interrupt
ET1 = 1; // enable timer 1 interrupt
ET2 = 1; // enable timer 2 interrupt



XBR2 = 0x48;// enable crossbar globally
XBR1 = 0x3E; // enable T0, IT0, T1, "IT1" , T2  
XBR0 = 0x04; // Tx and Rx (control externa0)
P0 = 0xff;  // open drain digital input
P2MDOUT = 0xff;  // digital o/p for seven 

//Timer 2
T2CON = 0x02;  // bit 6 and 7 flags reset, bit 2 =0 , bit 0,4,5 from slides table , set bit 1 (counter fn)
RCAP2 = 0xffff; // capture register 

T2CON = 0x06; // to enable capture function set bit 3

P1MDOUT = 0xff; // digital o/p

IR1 = 1;
IR2 = 1;

P2 = seven_segment[0]; // keep 0 when beginning


	//Timer 4
T4CON = 0x02;
RCAP4 = 0xfffc; // 4 counts to autoreload
T4CON = 0x06;
EIE2 = 0x04;	

//while(1){
//		if (IR1 == 0)
//		{
//			//buzzer = 1;
//			DELAY_ms (200) ;
//			if (IR2 == 0)
//			{
//				people = people + 1; 
//				if (people > 4)
//				{ 
			//		 buzzer = 1; 
//				}

//			}
//			else 
//			{ 
			//	buzzer = 1;
//			} 
//		}

//		if (IR2 == 0)
//		{ 
//			DELAY_ms(200) ;
//			if (IR1 == 0) 
//			{
//				people = people - 1;
//			}
//		}
//	}
}
