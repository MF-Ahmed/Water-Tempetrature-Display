#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//******************************************************* WORKING 

static volatile  unsigned char  IntRam[550];

// *********************************************************************

#define  Int_1us		PD3 
#define  Trigger		PD2 
#define  MemRW_led		PB1
#define  RS				PB0
#define  RW				PE0
#define  En1			PE1
#define  En2			PE2
#define  Ldata			PORTC

// #define SS1_Req ((PINH>>0)&1)
//********************************************************************************
static volatile unsigned char Trigger_set=0,DataLogg_End_flag=0,Tx_Reg=0,i,Rx_Cmd_PC=0,d=0,F1_start=0,F1_stop=0,F2_start=0,F2_stop=0,F3_start=0,F3_stop=0,F4_start=0,F4_stop=0,Process_Cmpt=0;
static volatile unsigned int F1_Counter=0,F2_Counter=0,F3_Counter=0,F4_Counter=0,Count_mSec,Count_uSec,TimeCount_uSec=0;
static volatile unsigned int TP_F1=0,TP_F2=0,TP_F3=0,TP_F4=0,ii=0;
static volatile unsigned int F1_startCnt=0,F1_stopCnt=0,F2_startCnt=0,F2_stopCnt=0,F3_startCnt=0,F3_stopCnt=0,F4_startCnt=0,F4_stopCnt=0;

static volatile unsigned char	F1_stRead=1,F2_stRead=1,F3_stRead=1,F4_stRead=1;
static volatile unsigned char	F1_spRead=1,F2_spRead=1,F3_spRead=1,F4_spRead=1;
static volatile unsigned char   F1_dsp=0,F2_dsp=0,F3_dsp=0,F4_dsp=0;
//static volatile unsigned int	i=0;


//*******************  Initilise Variables  **********************/
//unsigned char inc_Eint1=0,inc_Eint0=0;
//unsigned int P,Q,T,t1=0,t2=0,T_2,avg_cnt=0;
//unsigned int m_SECOND=0;
 //double 	Frq=0,Frq_m=0,T_P,Tmp,RPM,Avg_Frq=0,Avg_rst=0;
static volatile unsigned int  Time_hex=0,Time_ASCII_d1=0,Time_ASCII_d2=0,Time_ASCII_d3=0,d1,d2,d3,d4,d1a;
static volatile unsigned char  F1_d1,F1_d2,F1_d3,F1_d4;
static volatile unsigned char  F2_d1,F2_d2,F2_d3,F2_d4;
static volatile unsigned char  F3_d1,F3_d2,F3_d3,F3_d4;
static volatile unsigned char  F4_d1,F4_d2,F4_d3,F4_d4,valueHex=0;	
//unsigned int INTEGER_Frq,INTEGER_rpm,INTEGER_prv=0,Mtr_Rpm=0,Com_Rpm=0,Eng_Rpm=0;
 
//unsigned char DATA,D_PD,D_PC,RX_DATA;
//unsigned int j,k;
  
// ******************************************************
// ***************************************************************
static volatile unsigned char busy; 
//unsigned char p;	
//unsigned int Count_mS=0;
static volatile unsigned char value;
//*************************************
void Config_Ports(void)

{ 	DDRC = 0xFF;			// Declared PortA as Outnput  LCD D0-D7	
	PORTC = 0x00;

	DDRB = 0xFB;			// Config  PortB Tx1,Rx1,Led mem R/W, RS
	PORTB = 0x00;			

	DDRE = 0xFF;			// Declared PortC as Outnput  LCD Pc4-pc7 
	PORTE = 0x00;			//RS R/W E1 E2

	DDRA = 0x00;			// Declared PortA as input 	
	PORTA = 0x00;

	DDRD = 0xF3;			// Declared PortD as input int INT0=Trigger i/p  & int1=Int_clk 10us	
	PORTD = 0x00;
}
//******************************************* 
//*****************************************************************************

void Config_uart(void)
{
cli();

	UCSR1B = 0x98;		
	UCSR1A = 0x00;
	UCSR1C = 0x06;		//UCSR0C |= (1<<UPM01) | (1<<UPM00) |  (1<<UCSZ01) | (1<<UCSZ00);	
	UBRR1H = 0x00;			
	UBRR1L = 0x0C;		// 0x0C 38400bps	1 stopbit,non parity fr=8Mhz

sei();
}

//********************************************************        
void USART_Transmit(unsigned char data)
{ 	
	UCSR0A = 0;
	UDR0=data; 		/* PUT DATA IN TO BUFFER,SEND THE DATA */ 	
	while(!(UCSR0A & (1<<UDRE0)));
	UCSR0A = 0;
	data=0;
}
//***************************************************************************************
//*****************************************************************************       
//*******************************************************************
void Ini_Ext_Ints(void) 
{
cli();

MCUCR=0x0F;		// Rising edge INT0=trigger & Rising edge INT1=100us
GICR=0x50;		//disable INT0,INT1,PCINT0-7 Enable PORTA signal i/p
GIFR=0;			//Flag reset
PCMSK0=0x00;	//PCINT0-7 disable

sei();
}

//****************************************************************
//*****************************************************************
//*****************************************************************
//************************************
void TP_F1_display(unsigned int valueHex)
{

//Hex_to_Ascii();
/*
BCD_NO_Reg=valueHex;

R=BCD_NO_Reg;
r=R & 0x000F;

S=BCD_NO_Reg;
s=S & 0x00F0;
s=s>>4;

T=BCD_NO_Reg;
t=T & 0x0F00;
t=t>>8;

U=BCD_NO_Reg;
u=U& 0xF000;
u=u>>12;

r=r*1;
s=s*0x0A;
t=t*0x64;
u=u*0x3E8;

TP_F1=(t+s+r);
//TP_F1=(u+t+s+r);
*/
		d1=valueHex;
			d1=(d1%10);			
			F1_d1=(d1+48);

		d2=valueHex;
			d2=d2/10;
			d2=(d2%10);
			F1_d2=(d2+48);			

		d3=valueHex;
			d3=d3/100;
			d3=(d3%10);
			F1_d3=(d3+48);

/*
		d4=valueHex;
			d4=d4/1000;
			d4=(d4%10);
			F1_d4=(d4+48);
*/
	
//************************************			
 		// Set Address DD Ram 0x40 2nd line location
		LCD1_cmd(0x87);				
			LCD1_data(F1_d3);		
			LCD1_data(F1_d2);		
			LCD1_data(F1_d1);
			/*

		LCD1_cmd(0x07);				
			LCD1_data(F4_d4);

		LCD1_cmd(0x08);				
			LCD1_data(F4_d3);

		LCD1_cmd(0x09);	
			LCD1_data(F4_d2);

		LCD1_cmd(0x0B);	
			LCD1_data(F4_d1);

			*/
			
return;	

}
//********************************
//************************************
void TP_F2_display(unsigned int valueHex)
{

//Hex_to_Ascii();
		d1=valueHex;
			d1=(d1%10);			
			F2_d1=(d1+48);

		d2=valueHex;
			d2=d2/10;
			d2=(d2%10);
			F2_d2=(d2+48);			

		d3=valueHex;
			d3=d3/100;
			d3=(d3%10);
			F2_d3=(d3+48);
/*
		d4=valueHex;
			d4=d4/1000;
			d4=(d4%10);
			F2_d4=(d4+48);
*/
	
//************************************			
 		// Set Address DD Ram 0x40 2nd line location
		LCD1_cmd(0x20);				
			LCD1_data(F2_d3);

		LCD1_cmd(0x21);	
			LCD1_data(F2_d2);

		LCD1_cmd(0x22);	
			LCD1_data(F2_d1);
			/*

		LCD1_cmd(0x20);				
			LCD1_data(F4_d4);

		LCD1_cmd(0x21);				
			LCD1_data(F4_d3);

		LCD1_cmd(0x22);	
			LCD1_data(F4_d2);

		LCD1_cmd(0x24);	
			LCD1_data(F4_d1);

			*/
			
return;	

}
//********************************
//********************************************************
//************************************
void TP_F3_display(unsigned int valueHex)
{
//Hex_to_Ascii();
		d1=valueHex;
			d1=(d1%10);			
			F3_d1=(d1+48);

		d2=valueHex;
			d2=d2/10;
			d2=(d2%10);
			F3_d2=(d2+48);			

		d3=valueHex;
			d3=d3/100;
			d3=(d3%10);
			F3_d3=(d3+48);
/*
		d4=valueHex;
			d4=d4/1000;
			d4=(d4%10);
			F3_d4=(d4+48);
*/
	
//************************************			
 		// Set Address DD Ram 0x40 2nd line location
		LCD1_cmd(0x07);				
			LCD1_data(F3_d3);

		LCD1_cmd(0x08);	
			LCD1_data(F3_d2);

		LCD1_cmd(0x09);	
			LCD1_data(F3_d1);
			/*

		LCD1_cmd(0x07);				
			LCD1_data(F4_d4);

		LCD1_cmd(0x08);				
			LCD1_data(F4_d3);

		LCD1_cmd(0x09);	
			LCD1_data(F4_d2);

		LCD1_cmd(0x0B);	
			LCD1_data(F4_d1);

			*/
			
return;	

}
//********************************

//************************************
void TP_F4_display(unsigned int valueHex)
{

//Hex_to_Ascii();
		d1=valueHex;
			d1=(d1%10);			
			F4_d1=(d1+48);

		d2=valueHex;
			d2=d2/10;
			d2=(d2%10);
			F4_d2=(d2+48);			

		d3=valueHex;
			d3=d3/100;
			d3=(d3%10);
			F4_d3=(d3+48);
/*
		d4=valueHex;
			d4=d4/1000;
			d4=(d4%10);
			F4_d4=(d4+48);
*/
	
//************************************			
 		// Set Address DD Ram 0x40 2nd line location

		LCD2_cmd(0x20);				
			LCD2_data(F4_d3);

		LCD2_cmd(0x21);	
			LCD2_data(F4_d2);

		LCD2_cmd(0x22);	
			LCD2_data(F4_d1);
			/*

		LCD2_cmd(0x20);				
			LCD2_data(F4_d4);

		LCD2_cmd(0x21);				
			LCD2_data(F4_d3);

		LCD2_cmd(0x22);	
			LCD2_data(F4_d2);

		LCD2_cmd(0x24);	
			LCD2_data(F4_d1);

			*/

			
return;	

}
//********************************
//***********************************************************

//******************************************************************
//***************************************************************
//**************************************************************
void LCD_ready(void)
{

DDRC &=~(1<<DDC7);	//PC.7 input set	
busy=PIN7;	

PORTB &=~(1<<RS);		////  Pc4--> RS=0 ****}Cmd Read 
PORTE|=(1<<RW);			////Pc5-->Rw=1 // }
	while (busy==1)
		{		
		busy=PIN7;
		
		PORTE |=(1<<En1);			//E1=1  Pulse  pc6
		_delay_ms(10);
		PORTE &=~(1<<En1);			//E1=0  Pulse pc6
		
		PORTE |=(1<<En2);			//E2=1  Pulse  pc7
		_delay_ms(10);
		PORTE &=~(1<<En2);			//E2=0  Pulse  pc7
	
		}

		DDRC |=(1<<DDC7);	//set PC.7 Output
		//DDRC=0xFF;
			
return;
}
//*****************************************************************
    
void LCD1_cmd(unsigned char value)
{ 
LCD_ready();		//Check the LCD Busy flag 
Ldata=value;
 
PORTB &=~(1<<RS);		//Pc4-->RS=0  Write COMD 
PORTE &=~(1<<RW);		//Pc5-->R/W=0  Write COMD 
  
PORTE &=~(1<<En1);			//E1=0  Pulse 
PORTE |=(1<<En1);			//E1=1  Pulse 
_delay_ms(10);
PORTE &=~(1<<En1);			//E1=0  Pulse 
 		
return;
}

//**************************         *********************
void LCD2_cmd(unsigned char value)
{ 						//Check the LCD Busy flag 
LCD_ready();		//Check the LCD Busy flag 
Ldata=value;
 
PORTB &=~(1<<RS);		//Pc4-->RS=0  Write COMD 
PORTE &=~(1<<RW);		//Pc5-->R/W=0  Write COMD 
  
PORTE &=~(1<<En2);			//E2=0  Pulse 
PORTE |=(1<<En2);			//E2=1  Pulse 
_delay_ms(10);
PORTE &=~(1<<En2);			//E2=0  Pulse 
 		
return;
} 
     
 //***************************************************************
	
void LCD1_data(unsigned char value)
{ 
LCD_ready();		//Check the LCD Busy flag 
Ldata=value;
 
PORTB |=(1<<RS);		//Pc4-->RS=1  Write Data
PORTE &=~(1<<RW);		//Pc5-->R/W=0  Write Data
  
PORTE &=~(1<<En1);			//E1=0  Pulse 
PORTE |=(1<<En1);			//E1=1  Pulse 
_delay_ms(10);
PORTE &=~(1<<En1);			//E1=0  Pulse 
 		
return;
}

// **********************************          ******************
void LCD2_data(unsigned char value)
{ 
LCD_ready();		//Check the LCD Busy flag 
Ldata=value;
 
PORTB |=(1<<RS);		//Pc4-->RS=1  Write Data
PORTE &=~(1<<RW);		//Pc5-->R/W=0  Write Data
  
PORTE &=~(1<<En2);			//E2=0  Pulse 
PORTE |=(1<<En2);			//E2=1  Pulse 
_delay_ms(10);
PORTE &=~(1<<En2);			//E2=0  Pulse 
 		
return;
}

// ******************************************************
void T_Dsp(void)
{
unsigned char F1[]="         Fins Open Time Counter        ";

LCD1_cmd(0x38);				// Ini 2 lines & 5*7 matrix
LCD1_cmd(0x01);				// Clear Display Screen
LCD1_cmd(0x02);				// Return Home
LCD1_cmd(0x0F);				// Display Blink, Cursor ON

for(i=0;i<39;i++)
 		{LCD1_data(F1[i]);				// Display Char "F1 TP"	
 		}

LCD2_cmd(0x38);				// Ini 2 lines & 5*7 matrix
LCD2_cmd(0x01);				// Clear Display Screen
LCD2_cmd(0x02);				// Return Home
LCD2_cmd(0x0C);				// Display Blink, Cursor Off


} 

// *********************************************************************
// ******************************************************
void Mnu1_Dsp(void)
{
unsigned char F1[]="Fin1 = 000 mS            ";
unsigned char F2[]="Fin2 = 000 mS";
unsigned char F3[]="Fin3 = 000 mS            ";
unsigned char F4[]="Fin4 = 000 mS";

LCD1_cmd(0x38);				// Ini 2 lines & 5*7 matrix
LCD1_cmd(0x01);				// Clear Display Screen
LCD1_cmd(0x02);				// Return Home
LCD1_cmd(0x0F);				// Display Blink, Cursor ON


for(i=0;i<25;i++)
 		{LCD1_data(F1[i]);				// Display Char "F1 TP"	
 		}

for(i=0;i<13;i++)
 		{LCD1_data(F2[i]);				// Display Char "A"	
 		}

LCD2_cmd(0x38);				// Ini 2 lines & 5*7 matrix
LCD2_cmd(0x01);				// Clear Display Screen
LCD2_cmd(0x02);				// Return Home
LCD2_cmd(0x0F);				// Display Blink, Cursor ON

for(i=0;i<25;i++)
 		{LCD2_data(F3[i]);				// Display Char "A"	
 		}

for(i=0;i<13;i++)
 		{LCD2_data(F4[i]);				// Display Char "A"	
 		}



//LCD2_cmd(0xC0);				// Set Address DD Ram 0x40 2nd line location

} 
// ******************************************************
void Mnu2_Dsp(void)
{
unsigned char F1[]="Fin1 = 000.0 mS         ";
unsigned char F2[]="Fin2 = 000.0 mS";
unsigned char F3[]="Fin3 = 000.0 mS         ";
unsigned char F4[]="Fin4 = 000.0 mS";

LCD1_cmd(0x38);				// Ini 2 lines & 5*7 matrix
LCD1_cmd(0x01);				// Clear Display Screen
LCD1_cmd(0x02);				// Return Home
LCD1_cmd(0x0F);				// Display Blink, Cursor ON


for(i=0;i<24;i++)
 		{LCD1_data(F1[i]);				// Display Char "F1 TP"	
 		}

for(i=0;i<15;i++)
 		{LCD1_data(F2[i]);				// Display Char "A"	
 		}

LCD2_cmd(0x38);				// Ini 2 lines & 5*7 matrix
LCD2_cmd(0x01);				// Clear Display Screen
LCD2_cmd(0x02);				// Return Home
LCD2_cmd(0x0F);				// Display Blink, Cursor ON

for(i=0;i<24;i++)
 		{LCD2_data(F3[i]);				// Display Char "A"	
 		}

for(i=0;i<15;i++)
 		{LCD2_data(F4[i]);				// Display Char "A"	
 		}



//LCD2_cmd(0xC0);				// Set Address DD Ram 0x40 2nd line location

} 



//**********************************************************************************
//****************************** INTERRUPT SECTION   *******************************
//**********************************************************************************
// **********************************************************************************

SIGNAL(SIG_PIN_CHANGE1)	//PCINT 8---15 INPUT Read Fin status from PORTA 
{

F1_start=((PIND>>0)&1);
F1_stop=((PIND>>1)&1);

F2_start=((PIND>>2)&1);
F2_stop=((PIND>>3)&1);

F3_start=((PIND>>4)&1);
F3_stop=((PIND>>5)&1);

F4_start=((PIND>>6)&1);
F4_stop=((PIND>>7)&1);

if((F1_start)&(F1_stRead))
	{
	
	//F1_startCnt=TimeCount_uSec;
	F1_startCnt=Count_mSec;
	F1_stRead=0;
	//PCMSK1 &=~(1<<PCIF8);
	}
else if((F1_stop)&(F1_spRead))
	{
	//F1_stopCnt=TimeCount_uSec;
	F1_stopCnt=Count_mSec;
	F1_spRead=0;	
	}

else if((F2_start)&(F2_stRead))
	{
	//F2_startCnt=TimeCount_uSec;
	F2_startCnt=Count_mSec;	
	F1_stRead=0;
	}
else if((F2_stop)&(F2_spRead))
	{
	//F2_stopCnt=TimeCount_uSec;
	F2_stopCnt=Count_mSec;
	F1_spRead=0;
	//PCIFR &=~(1<<PCIF9);
	}

else if((F3_start)&(F3_stRead))
	{
	//F3_startCnt=TimeCount_uSec;
	F3_startCnt=Count_mSec;
	F1_stRead=0;	
	}
else if((F3_stop)&(F3_spRead))
	{
	//F3_stopCnt=TimeCount_uSec;
	F3_stopCnt=Count_mSec;
	F1_spRead=0;
	//PCIFR &=~(1<<PCIF9);
	}

else if((F4_start)&(F4_stRead))
	{
	//F4_startCnt=TimeCount_uSec;
	F4_startCnt=Count_mSec;	
	F4_stRead=0;
	}
else if((F4_stop)&(F4_spRead))
	{
	//F4_stopCnt=TimeCount_uSec;
	F4_stopCnt=Count_mSec;	
	F4_spRead=0;
	}

GIFR=0;			//Flag reset
PCMSK0=0xFF;	//PCINT0-7 Enable
}

// ************************************************************************
SIGNAL(SIG_INTERRUPT1)	// 100_uS INPUT Int1 //  1us=1MHz,10us=100KHz,100us=10KHz
{
Count_uSec++;
//TimeCount_uSec++;
//cli();

//GICR=0x00;		//INT1=1uS Disable ,INT0=Trigger i/p disable
GIFR=0;			//Flag reset
//PCMSK1=0xFF;	//PCINT8-15 Enable

if(Count_uSec==5)		//For Real Time process
		{		
			IntRam[ii]=PINA;
			ii++;
			
		}
else if(Count_uSec==10)		//For Real Time process 100us
		{	Count_mSec++;
		 	Count_uSec=0;			
		}
// ********************
//GICR=0x40;		//INT1=1uS Enable ,INT0=Trigger i/p disable
//GIFR=0;			//Flag reset

if(Count_mSec==255)		//RecordTimeSet=1m Sec to 250mSec 
	{
		GICR=0x50;		//INT1=100uS Disable ,INT0=Trigger i/p Enable
		GIFR=0;			//Flag reset
		PCMSK0=0;	//PCINT0-7 Disable
		//F1_Counter=Count_mSec;
		 Count_uSec=0;
		 Count_mSec=0;		//PORTJ &=~(1<<Start_DL);	// G LED OFF  Start Data Logging		
		//TimeCount_uSec=0;
		
		Process_Cmpt=1;
		DataLogg_End_flag=1;
	}	// end of else if

//sei(); 
}
//*********************************************************************************

SIGNAL(SIG_INTERRUPT0)	//INT0 Trigger input
{
// *************************************************
//cli();

//MCUCR=0x0F;		//INT0,INT1 rising edge enable
GICR=0x90;		//INT1 100uS clk Enable ,INT0=trigger  disable
GIFR=0;			//Flag reset
PCMSK0=0xFF;	//PCINT0-7 Enable

F1_Counter=0;
F2_Counter=0;
F3_Counter=0;
F4_Counter=0;
Trigger_set=1;

//sei();  
}

//**************************************************************************************
//SIGNAL(SIG_USART_RX)
//ISR(USART_RX_vect)

SIGNAL(SIG_USART1_RECV)
{
cli();
Rx_Cmd_PC=UDR1;	
					//RxBuf=Rx_Cmd_PC;
					//USART_Transmit(RxBuf);
UCSR1A = 0x00;	
sei();		
}

//**********************************************************************************
// ************************************************************************
//*********************************************************************
//*********************  MAIN   PROGRAM   *****************************    
//*********************************************************************     
 int main(void)
{ 	

Config_Ports(); 

//
Mnu1_Dsp();

for(ii=0;ii<10;ii++)
	{
		_delay_ms(100);
	}

valueHex=235;
d1=valueHex;
			d1=(d1%10);			
			F1_d1=(d1+48);

		d2=valueHex;
			d2=d2/10;
			d2=(d2%10);
			F1_d2=(d2+48);			

		d3=valueHex;
			d3=d3/100;
			d3=(d3%10);
			F1_d3=(d3+48);

/*
		d4=valueHex;
			d4=d4/1000;
			d4=(d4%10);
			F1_d4=(d4+48);
*/
	
//************************************			
 		// Set Address DD Ram 0x40 2nd line location

_delay_ms(100);
//LCD1_cmd(0x80);				// Return Home
		LCD1_cmd(0x87);				
			LCD1_data(F1_d3);
		
			LCD1_data(F1_d2);
		
			LCD1_data(F1_d1);
		


		LCD1_cmd(0xA0);				
			LCD1_data(F1_d3);	
			LCD1_data(F1_d2);		
			LCD1_data(F1_d1);




	
for(;;)
{		

;		//TP_F4=0;
	//	TP_F4=(F4_stopCnt - F4_startCnt);		

	//	TP_F4_display(TP_F4);
	




	
					
// ***********************************************
}	//for end 

//USART_Transmit(0x0A);//line fead
//USART_Transmit(0x0D);//C R         
}	// main end  

//***********************************************************************
//***********************************************************************


 



