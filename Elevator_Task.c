#include <compiler_defs.h>

#include <C8051F020_defs.h> // Include SFR declarations


unsigned int floor =0;
//unsigned int elev_load = 0;
char segCode[]={0x3f,0x06,0x5b,0x4f};
sbit up = P1^0;
sbit down = P1^1;
sbit buzzer = P1^2;
sbit IR1 = P1^6;
sbit IR2 = P1^7;
sbit led = P2^7;                         // try ir
unsigned int people = 0 ;

//P1MDIN |= 0xFF;

void delay(unsigned int ms_Count)
{
    unsigned int i,j;
    for(i=0;i<ms_Count;i++)
    {
        for(j=0;j<100;j++);
    }
}


void initClock(void)
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

void IR(void) interrupt 16
{
led = 1;
delay(500);
led = 0;
T4CON = 0x06;
}


// First Priority/INT0/P0.3
void zeroFloor(void) interrupt 0
{
	while (floor > 0)
	{ 
		floor = floor - 1;
		//get the hex code from LUT and show on seven segment
		down = 1; 
		up = 0;  
		delay(1000);
		P2 = segCode[floor];
		//show on 7 segment LUT
		down = 0;
	}
} 

// Second Priority/Timer0/P0.2
void firstFloor (void) interrupt 1
{
	if (floor == 0)
	{ 
		floor = 1 ; 
		down = 0; 
		up = 1 ;
		delay(1000);
		P2 = segCode[floor];
		up = 0;
	}
	else 
	{ 
		while (floor >1)
		{
			down = 1; 
			up = 0 ;
			floor = floor -1 ;
			delay(1000); 
			P2 = segCode[floor];
			down = 0;
		}
	}
}
//ACTUAL THIRD PRIORITY IS INT1

// Third Priority/Timer1/P0.4
void secondFloor(void) interrupt 3
{
	if (floor == 3)
	{ 
		floor = 2; 
		up = 0 ;
		down = 1;
		delay(1000); 
		P2 = segCode[floor];
		down = 0;  

	}
	else 
	{ 
		while (floor <2)
		{
			floor = floor + 1 ; 
			down = 0; 
			up = 1 ;
			delay(1000); 
			P2 = segCode[floor];  
			up = 0;
		}
	}
}

//INT1 ON PIN 5 AND PIN 5 IS NOT WORKING, I SWEAR WALLAHY !!!!!! WITH PRIORITY 2

//INT4 IS URT0

// Fifth Priority/Timer2/P0.6
void thirdFloor(void) interrupt 5
{
	while (floor < 3)
	{ 
		floor = floor + 1;
		down = 0; 
		up = 1; 
		delay(1000);
		//show on 7 segment LUT
		P2 = segCode[floor];
		up = 0;
	}

	TF2 = 0;
} 



void main (void)
{

	TMOD = 0x66 ;//use timers as external interrupt 0110 0110
	EA = 0; //globl intrrupt enable
	WDTCN = 0xde;
	WDTCN = 0xad;
	initClock();
	EX0 = 1; //INT0 enable external interrupt individual 
	EX1 = 1; //INT1 enable external interrupt individual

	TL0 = 0xff; 
	TH0 = 0xff;
	TL1 = 0xff;
	TH1 = 0xff;


	IT0 = 1; //Interrupt for interrupt 0 select type is edge triggered not level 
	IT1 = 1; //Interrupt for interrupt 1 select type is edge triggered not level
	ET0 = 1; //enable timer 0 interrupt
	ET1 = 1; //enable timer 1 interrupt
	TR0= 1; // start count.
	TR1= 1; 	

	
	XBR1 = 0x3E; // route T0, INT0, T1, INT1, T2 to port pins   
	XBR0 = 0x04; // enable uart to put Tx and Rx on pins 0.0 and 0.1
	XBR2 = 0x48; //enable crossbar global
	P0 = 0xff; //set to input
//	P1 |= 0xff; //input
	P2MDOUT = 0xff;
	IR1 = 1;
	IR2 = 1;
	
	//Timer 2
	T2CON = 0x02;
	RCAP2 = 0xffff;
	TL2 = 0xff;
	TH2 = 0xff;
	T2CON = 0x06;
	ET2 = 1; //enable timer 2 interrupt
	P1MDOUT = 0xff;
	buzzer = 0;	

	//Timer 4
	T4CON = 0x02;
	RCAP4 = 0xfffc;
	TL4 = 0xfc;
	TH4 = 0xff;
	T4CON = 0x06;
	EIE2 = 0x04;	

	EA = 1;
	
    P2 = segCode[0];
//servo = 0;
	while(1)
	{
		if (IR1 == 0)
		{
			buzzer = 1;
			delay (200) ;
			if (IR2 == 0)
			{
				people = people + 1; 
				//EA = 1;
				//buzzer = 1;
				if (people > 4)
				{ 
					 buzzer = 1; 
					//EA = 0;
				}

			}
			else 
			{ 
				buzzer = 1;
			} 
		}

		if (IR2 == 0)
		{ 
			delay(200) ;
			if (IR1 == 0) 
			{
				people = people - 1;
			}
		}
	}
}