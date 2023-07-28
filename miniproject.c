#include<intrins.h>
#include<reg51.h>
#include <stdio.h>
#include <math.h>

#define lcd_data P2

sfr16 DPTR=0x82;

sbit trig1=P1^3;
sbit echo1=P1^4;

sbit trig2=P1^5;
sbit echo2=P1^6;

// infrared module ------------------
sbit IR=P1^7;

unsigned int range1=0;
unsigned int range2=0;
int widthofgate = 488;    //distance between the gate
int widthofvehicle=0;

int vacancies = 2;
int count=0;

sbit rs=P1^2;
sbit rw=P1^1;
sbit en=P1^0;

void lcd_init();
void cmd(unsigned char a);
void dat(unsigned char b);
void show(unsigned char *s);
void lcd_delay();

void lcd_init()
{
  cmd(0x38);
  cmd(0x0e);
  cmd(0x06);
  cmd(0x0c);
  cmd(0x80);
}

void cmd(unsigned char a)
{
  lcd_data=a;
  rs=0;
  rw=0;
  en=1;
  lcd_delay();
  en=0;
}

void dat(unsigned char b)
{
  lcd_data=b;
  rs=1;
  rw=0;
  en=1;
  lcd_delay();
  en=0;
}

void show(unsigned char *s)
{
  while(*s) {
    dat(*s++);
  }
}

void lcd_delay()
{
  unsigned int i;
  for(i=0;i<=1000;i++);
}


void send_pulse1(void) 
{
  TH0=0x00;TL0=0x00; 
   trig1=1;
   _nop_();_nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();_nop_();
   trig1=0;
}

void send_pulse2(void) 
{
  TH0=0x00;TL0=0x00; 
   trig2=1;
   _nop_();_nop_();_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();_nop_();_nop_();
   trig2=0;
}

unsigned char ultrasonic1()
{
  unsigned char get;
  send_pulse1();

  while(!echo1){
  send_pulse1();
  }
  while(echo1);
  DPH=TH0;
  DPL=TL0;
  TH0=TL0=0xff;
  if(DPTR<30000)
    get=DPTR/59;
  else
    get=0;
  return get;
}

unsigned char ultrasonic2()
{
  unsigned char get;
  send_pulse2();

  while(!echo2){
  send_pulse2();
  }
  while(echo2);
  DPH=TH0;
  DPL=TL0;
  TH0=TL0=0xff;
  if(DPTR<30000)
    get=DPTR/59;
  else
    get=0;
  return get;
}
void delay(){
	unsigned int i,j;
	for(i=0;i<1000;i++){
		for(j=0;j<1000;j++);
		}
}

void delayfew(){
	unsigned int i,j;
	for(i=0;i<500;i++){
		for(j=0;j<1000;j++);
		}
}

void main()
{
	//ultrasonic 1
  TMOD=0x09;
  TH0=TL0=0;
  TR0=1;
  lcd_init();
  P3|=(1<<2);
  while(1) {
  if(count==0){
  	 cmd(0x80);
  	 show("THE PARKING LOT");
  	 }																			
	//IR Sensor detection
		if(IR==0 && count==0) {
			cmd(0x01);  /* clears the lcd display */
			cmd(0x80);
			show("WELCOME TO LOT");
			delay();
			cmd(0x01);  /* clears the lcd display */
		    cmd(0x80);
		    show("Checking Space");
		    cmd(0xc0);
		    show("Please Wait...");
		    count=1;
		    delay(2000);
		}
		
		range1=ultrasonic1();
      range2=ultrasonic2();
      widthofvehicle = widthofgate - (range1 + range2);
      cmd(0xc0);
      dat((widthofvehicle/100)+48);
      dat(((widthofvehicle/10)%10)+48);
      dat((widthofvehicle%10)+48);
		
		if (count==1 && vacancies>0) {
			cmd(0x01);  /* clears the lcd display */
			cmd(0x80);
			show("VACANCY AVAILABLE");
			cmd(0xc0);
    		show("PLEASE PROCEED");
    		delayfew();	
    		
    		if(widthofvehicle<300 && widthofvehicle>100) {
		    	vacancies-=1;
		    	cmd(0x01);  /* clears the lcd display */
	        	cmd(0x80);
				show("VEHICLE ENTERED");
				cmd(0xc0);
				show("Width: ");
				cmd(0xc7);
				dat((widthofvehicle/100)+48);
    			dat(((widthofvehicle/10)%10)+48);
    			dat((widthofvehicle%10)+48);	
				delay();
				
				cmd(0x01);  /* clears the lcd display */
				cmd(0x80);
				show("VACANCIES = ");
				cmd(0x8d);
				dat(vacancies+48);
				delay();
				count=0;
				cmd(0x01);
			}
    	}
    		
    	else if(count==0 && widthofvehicle<300 && widthofvehicle>100 && vacancies<2) {
	      vacancies+=1;
	      cmd(0x01);  /* clears the lcd display */
	      cmd(0x80);
			show("Thankyou, Visit");
			cmd(0xc0);
			show("Again");
			delay();
	      cmd(0x01);  /* clears the lcd display */
	      cmd(0x80);
			show("VACANCIES = ");
	      cmd(0x8d);
			dat(vacancies+48);
			delay();
			cmd(0x01);
		}
	  
		else if(count==1 && vacancies==0) {
		   cmd(0x01);  /* clears the lcd display */
	      cmd(0x80);
			show("SORRY, NO MORE");
			cmd(0xc0);
			show("VACANCY");
		   count=0;
		   delay();
		   cmd(0x01);
	  	}	
  }
}  
