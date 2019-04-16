#include <C8051F020_defs.h>
#include<stdio.h>
#include<stdlib.h>
P1MDOUT |= 0x40;//output lel seven_segment and the motor that controls the elevator's movement
P0MDOUT |= 0x40;//output lel motor that controls the door closure and opening
EA=0; //Disable all interrupts
P2 =1;// set p3 as input
p2=0;
void  ext_int_0 () interrupt 0
{
P0=1;//el port da zero by default as it's op port, on push_open_button, interrupt is gonna take place,triggering the motor which takes the port's op as ip  
}
void ext_int_1() interrupt 2
{
P0=1; //open the door as well, but this time in response for the up or down external button interrupt
}
 
void main (void)
{

char table[]={DB,3FH,06H,5BH,4FH,66H,6DH,7DH,07H,7FH,6FH};
//conter_up
for(int i=0;i<4;i++) 
{
// check if the open_button is clicked
if(P2.1==1)// this push_button gonna repressent the Open button
{
 EA=1;//enable global interrupt
 EX0=1;//enable INT0
}
//check if the up_button is clicked
if(P2.5)//this push_button represents the Up button
{
  EX1 = 1;    // Enable INT1
  EA  = 1;
}
P1=table[i];
delay(1000);
}




//counter_down
for(int i=4;i<0;i--) 
{
// check if the open_button is clicked
if(P2.1==1)// this push_button gonna repressent the Open button
{ EA=1;//enable global interrupt
 EX0=1;//enable INT0
}
//check if the up_button is clicked
if(P2.7)//this push_button represents the down button
{
  EX1 = 1;    // Enable INT1
  EA  = 1;
}
P1=table[i];
delay(1000);
}

}


