#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/delay.h>

void WR_Data_Mtr(void);
void WR_Data_Eng(void);

//*******************  Initilise Variables  **********************/
unsigned char inc_Eint1=0,inc_Eint0=0,i;
unsigned int P,Q,T,t1=0,t2=0,T_2,avg_cnt=0;
unsigned int m_SECOND=0;
 double 	Frq=0,Frq_m=0,T_P,Tmp,RPM,Avg_Frq=0,Avg_rst=0;
unsigned int  Com_rpm,d1,d2,d3,d4,d1a;
unsigned char  Com_rpm_d1,Com_rpm_d2,Com_rpm_d3,Com_rpm_d4,Frq_Hx;	
unsigned int INTEGER_Frq,INTEGER_rpm,INTEGER_prv=0,Mtr_Rpm=0,Com_Rpm=0,Eng_Rpm=0;
 
unsigned char DATA,D_PD,D_PC,RX_DATA;
unsigned int j,k;
  
/******************************************************/  
// unsigned char count;
 #define RS		4
 #define RW		5
 #define En1	6
 #define En2	7
 #define Ldata	PORTA
//***************************************************************
unsigned char busy; 
unsigned char p;	
unsigned int Count_mS=0;
unsigned char value;
	
void InitVars(void)
{

}

//*************************************
void Config_Ports(void)

{ 	DDRA = 0xFF;			// Declared PortA as Outnput  LCD D0-D7	
	PORTA = 0x00;

	DDRB = 0x4F;			// Config  PortB as spi	
	PORTB = 0x00;			

	DDRC = 0xFF;			// Declared PortC as Outnput  LCD Pc4-pc7 
	PORTC = 0x00;			//RS R/W E1 E2

	DDRD = 0xF2;			// Declared PortD as input Tx0 Rx0 INT0 INT1	
	PORTD = 0x00;

	DDRE = 0x06;			// Declared PortE as input	
	PORTE = 0x00;
}
//******************************************* 
void Config_Control_Regs(void)
{
/* SPI Configuration */
/* Serial Communication Configuration */    
}
//*****************************************  

void Config_Timer0(void)
{
	TCCR0 =(0<<WGM01)|(0<<WGM00) ;// normal mode 0  0xFF overflow
	TCCR0 =(0<<CS02)|(1<<CS01)|(1<<CS00);//pre scaller clk*64=0.125uS*64==>8uS one count
	TIMSK=1<<TOIE0;	//Enable interrupt on TMR0 overflow
	TCNT0 = 0x83;	// Initial  Timer 0 Value				
	sei();		//Global interrupt enable
} 

//******************************************************/               

void Config_timer2(void)
{ 
	TCCR2 =(0<<WGM21)|(0<<WGM20) ;// normal mode 0 0xFF overflow
	TCCR2 =(0<<CS22)|(1<<CS21)|(1<<CS20);//pre scaller clk*32=0.125uS*32==>4uS one count
	TIMSK=1<<TOIE2;
	TCNT2 = 0x05;	// Timer 0 Initial Value				
	sei();			//globel interrupt enables
}    

        
void config_intr(void)
{   MCUCR =0X0E;	//rising edge int1  / falling edge int0
	EMCUCR =0X00;	//INT2 sense disable
	GICR =0X00;	//int1/int0 Interrupt Request Disable
	sei();		//globel interrupt enables
} 
       


void Config_uart(void)
{   					
	UCSR0B = 0x18;		/* No Interrupts,RXEN enabled TXEN enabled */
	UCSR0A = 0x00;		/* USART Data Register Empty */
	UCSR0C = 0x06;		/* No Parity, 1 Stop Bit */
	UBRR0H = 0x00;		/* Baud Rate= 9600 * USBRRnHL =51d,0X33  */
	UBRR0L = 0x33;		/* Error: 0.2% Baud Rate Registers*/
}  

//*****************************************************************
//*****************************************************************

void USART_Transmit(unsigned char data)
{ 	
	UDR0=data; 		/* PUT DATA IN TO BUFFER,SEND THE DATA */ 	
	while(!(UCSR0A & (1<<UDRE0)))
	;	
} 
//************************************
void Frq_Display(unsigned int Frq_Cnv_hex)
{
			d1=Frq_Cnv_hex;
			d1=(d1%10);			
			Com_rpm_d1=(d1+48);

		d2=Frq_Cnv_hex;
			d2=d2/10;
			d2=(d2%10);
			Com_rpm_d2=(d2+48);			

		d3=Frq_Cnv_hex;
			d3=d3/100;
			d3=(d3%10);
			Com_rpm_d3=(d3+48);			
	
		d4=Frq_Cnv_hex;
			d4=d4/1000;
			d4=(d4%10);
			Com_rpm_d4=(d4+48);

				USART_Transmit(0x3C);
				USART_Transmit(Com_rpm_d4);	
				USART_Transmit(Com_rpm_d3);	
				USART_Transmit(Com_rpm_d2);
				USART_Transmit(Com_rpm_d1);
				USART_Transmit('H');
				USART_Transmit('z');
				USART_Transmit(0x3E);				
	
return;	

}
//********************************
void RPM_Display(unsigned int Rpm_Cnv_hex)
{
			d1=Rpm_Cnv_hex;
			d1=(d1%10);			
			Com_rpm_d1=(d1+48);

		d2=Rpm_Cnv_hex;
			d2=d2/10;
			d2=(d2%10);
			Com_rpm_d2=(d2+48);			

		d3=Rpm_Cnv_hex;
			d3=d3/100;
			d3=(d3%10);
			Com_rpm_d3=(d3+48);			
	
		d4=Rpm_Cnv_hex;
			d4=d4/1000;
			d4=(d4%10);
			Com_rpm_d4=(d4+48);

				USART_Transmit(0x3C);
				USART_Transmit(Com_rpm_d4);	
				USART_Transmit(Com_rpm_d3);	
				USART_Transmit(Com_rpm_d2);
				USART_Transmit(Com_rpm_d1);
				USART_Transmit('R');
				USART_Transmit('p');
				USART_Transmit('m');
				USART_Transmit(0xF8);				
				USART_Transmit(0x3E);
				USART_Transmit(0x0A);//line fead
				USART_Transmit(0x0D);//C R

			
return;	

}
//********************************************************

//***********************************************************

void Fun_Ext_Int0(void)
{

TIMSK|=1<<TOIE0;	//Enable interrupt on TMR0 overflow
GICR |=(1<<INT0);	//int0 Interrupt Request EN	
sei();		//globel interrupt enables
//*****************************
do
{	
	if(inc_Eint0==1)
			{	t1=0;		
				t1=T;}

	else if (inc_Eint0==2)
			{	t2=0;				
				t2=T;
			if(t2>t1)
				{	T_P=0;			
					T_P=(t2-t1);				
					Frq=(1/T_P);
					Frq=Frq*1000;			
					//RPM=Frq*60;						
					//Frq_Hx=Avg_Frq;				
					Avg_Frq=Avg_Frq+Frq;
					avg_cnt++; 				
				}
			}

else if (inc_Eint0>2)
		{	inc_Eint0=0;		
			m_SECOND=0;
			TCNT0 =0x83;	// Timer 0 Initial Value											
		}

		
if(avg_cnt==100)
		{		
				Avg_rst=(Avg_Frq)/(avg_cnt);
				RPM=Avg_rst*60;
				INTEGER_Frq=Avg_rst;
				INTEGER_rpm=RPM;
				Eng_Rpm=RPM;
						
				Avg_Frq=0;
				Avg_rst=0;
		
				//Frq_Display(INTEGER_Frq);
	 			//RPM_Display(INTEGER_rpm);
				//WR_Data_Mtr();
				WR_Data_Eng();					
		}

}while(avg_cnt<100);

avg_cnt=0;								
				
	
TIMSK&=~(0<<TOIE0);	//Disable interrupt on TMR0 overflow
GICR&=~(0<<INT0);	//int0 Interrupt Request Disable
cli();		//globel interrupt Disable
}
//******************************************************************
//***************************************************************
//**************************************************************
void LCD_ready(void)
{

DDRA&=~(1<<DDA7);	//PA.7 input set	
busy=PIN7;	

PORTC&=~(1<<RS);		////  Pc4--> RS=0 ****}Cmd Read 
PORTC|=(1<<RW);			////Pc5-->Rw=1 // }
	while (busy==1)
		{		
		busy=PIN7;
		
		PORTC|=(1<<En1);			//E1=1  Pulse  pc6
		_delay_ms(10);
		PORTC&=~(1<<En1);			//E1=0  Pulse pc6
		
		PORTC|=(1<<En2);			//E2=1  Pulse  pc7
		_delay_ms(10);
		PORTC&=~(1<<En2);			//E2=0  Pulse  pc7
	
		}
		DDRA|=(1<<DDA7);	//set PA.7 Output
			
return;
}
//*****************************************************************
    
void LCD1_cmd(unsigned char value)
{ 						//Check the LCD Busy flag 
LCD_ready();			//Check the LCD Busy flag 
Ldata=value;

PORTC&=~(1<<RS);		//Pc4-->RS=0  Write COMD  
PORTC&=~(1<<RW);		//Pc5-->R/W=0  Write COMD 
	

PORTC&=~(1<<En1);			//E1=0  Pulse 					
_delay_ms(10);
PORTC|=(1<<En1);			//E1=1  Pulse 
_delay_ms(10);
PORTC&=~(1<<En1);			//E1=0  Pulse 
//******************************************
return;  				
	
}

//**************************         *********************
void LCD2_cmd(unsigned char value)
{ 						//Check the LCD Busy flag 
LCD_ready();			//Check the LCD Busy flag 
Ldata=value;

PORTC&=~(1<<RS);		//Pc4-->RS=0  Write COMD  
PORTC&=~(1<<RW);		//Pc5-->R/W=0  Write COMD 


//******************************************
PORTC&=~(1<<En2);			//E2=0  Pulse  
PORTC|=(1<<En2);			//E2=1  Pulse 
_delay_ms(10);
PORTC&=~(1<<En2);			//E2=0  Pulse  
	
return;  				
	
} 
     
 //***************************************************************
	
void LCD1_data(unsigned char value)
{ 
LCD_ready();		//Check the LCD Busy flag 
Ldata=value;
 
PORTC|=(1<<RS);		//Pc4-->RS=1  Write Data
PORTC&=~(1<<RW);		//Pc5-->R/W=0  Write Data
  
PORTC&=~(1<<En1);			//E1=0  Pulse 
PORTC|=(1<<En1);			//E1=1  Pulse 
_delay_ms(10);
PORTC&=~(1<<En1);			//E1=0  Pulse 
 		
return;
}

//**********************************          ******************
void LCD2_data(unsigned char value)
{ 
LCD_ready();		//Check the LCD Busy flag 
Ldata=value;

PORTC|=(1<<RS);		//Pc4-->RS=1  Write Data
PORTC&=~(1<<RW);		//Pc5-->R/W=0  Write Data

PORTC&=~(1<<En2);			//E2=0  Pulse 
PORTC|=(1<<En2);			//E2=1  Pulse 
_delay_ms(10);
PORTC&=~(1<<En2);			//E2=0  Pulse 
 		
return;
}

//******************************************************
void Mnu_Dsp(void)
{
unsigned char Tmp1[]="T1=300'C  ";
unsigned char Tmp2[]="T2=205'C  ";
unsigned char Tmp3[]="T3=155'C  ";
unsigned char Tmp4[]="T4=100'C  ";
unsigned char Tmp5[]="T5=090'C  ";
unsigned char Tmp6[]="T6=080'C  ";
unsigned char Tmp7[]="T7=055'C  ";
unsigned char Tmp8[]="T8=035'C ";
unsigned char Tmp9[]="T9=025'C  ";
unsigned char Tmp10[]="TA=001'C  ";
unsigned char Tmp11[]="TB=000'C  ";
unsigned char Com_Rpm[]="Com_Rpm=3000 ";
unsigned char Mtr_Rpm[]="Mtr_Rpm=2200 ";
unsigned char Eng_Rpm[]="Eng_Rpm=1100 ";


LCD1_cmd(0x38);				// Ini 2 lines & 5*7 matrix

LCD1_cmd(0x01);				// Clear Display Screen

LCD1_cmd(0x02);				// Return Home

LCD1_cmd(0x0F);				// Display Blink, Cursor ON


for(i=0;i<10;i++)
 		{LCD1_data(Tmp1[i]);				// Display Char "A"	
 		}

for(i=0;i<10;i++)
 		{LCD1_data(Tmp2[i]);				// Display Char "A"	
 		}

for(i=0;i<10;i++)
 		{LCD1_data(Tmp3[i]);				// Display Char "A"	
 		}

for(i=0;i<10;i++)
 		{LCD1_data(Tmp4[i]);				// Display Char "A"	
 		}

for(i=0;i<10;i++)
 		{LCD1_data(Tmp5[i]);				// Display Char "A"	
 		}

for(i=0;i<10;i++)
 		{LCD1_data(Tmp6[i]);				// Display Char "A"	
 		}

for(i=0;i<10;i++)
 		{LCD1_data(Tmp7[i]);				// Display Char "A"	
 		}

for(i=0;i<9;i++)
 		{LCD1_data(Tmp8[i]);				// Display Char "A"	
 		}

LCD2_cmd(0x38);				// Ini 2 lines & 5*7 matrix

LCD2_cmd(0x01);				// Clear Display Screen

LCD2_cmd(0x02);				// Return Home

LCD2_cmd(0x0F);				// Display Blink, Cursor ON


for(i=0;i<10;i++)
 		{LCD2_data(Tmp9[i]);				// Display Char "A"	
 		}

for(i=0;i<10;i++)
 		{LCD2_data(Tmp10[i]);				// Display Char "A"	
 		}

for(i=0;i<10;i++)
 		{LCD2_data(Tmp11[i]);				// Display Char "A"	
 		}
LCD2_cmd(0xC0);				// Set Address DD Ram 0x40 2nd line location


for(i=0;i<13;i++)
 		{LCD2_data(Com_Rpm[i]);				// Display Char "A"	
 		}

for(i=0;i<13;i++)
 		{LCD2_data(Mtr_Rpm[i]);				// Display Char "A"	
 		}

for(i=0;i<13;i++)
 		{LCD2_data(Eng_Rpm[i]);				// Display Char "A"	
 		}

}
 
//*********************************************************************
void Hex_to_Ascii(void)
{
for(j=0;j<4;j++)
			{_delay_ms(250);}

			d1=Com_rpm;
			d1=(d1%10);			
			Com_rpm_d1=(d1+48);	

		d2=Com_rpm;
			d2=d2/10;
			d2=(d2%10);
			Com_rpm_d2=(d2+48);			

		d3=Com_rpm;
			d3=d3/100;
			d3=(d3%10);
			Com_rpm_d3=(d3+48);
	
		d4=Com_rpm;
			d4=d4/1000;
			d4=(d4%10);
			Com_rpm_d4=(d4+48);

}
//*********************************************************
void WR_Data_Com(void)
{
Com_rpm=Com_Rpm;
Hex_to_Ascii();
	
//************************************
			
 		LCD2_cmd(0xC8);				// Set Address DD Ram 0x40 2nd line location
			LCD2_data(Com_rpm_d4);	

		LCD2_cmd(0xC9);				
			LCD2_data(Com_rpm_d3);

		LCD2_cmd(0xD0);	
			LCD2_data(Com_rpm_d2);

		LCD2_cmd(0xD1);	
			LCD2_data(Com_rpm_d1);

	

}
//********************************
void WR_Data_Eng(void)
{
Com_rpm=Eng_Rpm;
Hex_to_Ascii();
	
//************************************
			
 		LCD2_cmd(0xE2);				// Set Address DD Ram 0x40 2nd line location
			LCD2_data(Com_rpm_d4);	

		LCD2_cmd(0xE3);				
			LCD2_data(Com_rpm_d3);

		LCD2_cmd(0xE4);	
			LCD2_data(Com_rpm_d2);

		LCD2_cmd(0xE5);	
			LCD2_data(Com_rpm_d1);					
 

	}
//*********************************************************
//*********************************************************************
void WR_Data_Mtr(void)
{

Com_rpm=Mtr_Rpm;
Hex_to_Ascii();

 		LCD2_cmd(0xD5);				// Set Address DD Ram 0x40 2nd line location
			LCD2_data(Com_rpm_d4);	

		LCD2_cmd(0xD6);				
			LCD2_data(Com_rpm_d3);

		LCD2_cmd(0xD7);	
			LCD2_data(Com_rpm_d2);

		LCD2_cmd(0xD8);	
			LCD2_data(Com_rpm_d1);
	}
//*********************************************************
void Fun_Ext_Int1(void)

{
TIMSK|=1<<TOIE1;	//Enable interrupt on TMR0 overflow
GICR |=1<<INT1;	//int1 Interrupt Request EN	
sei();		//globel interrupt enables
//*************************************
do
{	
	if(inc_Eint1==1)
			{	t1=0;		
				t1=T;}

	else if (inc_Eint1==2)
			{	t2=0;				
				t2=T;
			if(t2>t1)
				{	T_P=0;			
					T_P=(t2-t1);				
					Frq=(1/T_P);
					Frq=Frq*1000;			
					//RPM=Frq*60;						
					//Frq_Hx=Avg_Frq;				
					Avg_Frq=Avg_Frq+Frq;
					avg_cnt++; 				
				}
			}

else if (inc_Eint1>2)
		{	inc_Eint1=0;		
			m_SECOND=0;
			TCNT0 =0x83;	// Timer 0 Initial Value											
		}
		
if(avg_cnt==100)
		{				
				Avg_rst=(Avg_Frq)/(avg_cnt);
				RPM=Avg_rst*60;
				INTEGER_Frq=Avg_rst;
				INTEGER_rpm=RPM;
				Com_Rpm=RPM;
				Mtr_Rpm=Avg_rst;
						
				Avg_Frq=0;
				Avg_rst=0;
		
				//Frq_Display(INTEGER_Frq);
	 			//RPM_Display(INTEGER_rpm);
				WR_Data_Com();
				WR_Data_Mtr();
				//	WR_Data_Eng();
									
		}

}while(avg_cnt<100);

avg_cnt=0;								
				
	
TIMSK&=~(0<<TOIE1);	//Disable interrupt on TMR1 overflow
GICR&=~(0<<INT1);	//int1 Interrupt Request Disable
cli();		//globel interrupt Disable

}


//*********************************************************************
//*********************  MAIN   PROGRAM   *****************************    
//*********************************************************************     
               
int main(void)

{ 	unsigned char d;
	Config_Ports();
	//InitVars();  	
  	//Config_Control_Regs();
  	Config_Timer0();	
	Config_uart();		       
	config_intr();
	Mnu_Dsp();
	
for(d=0;d<1000;d++)
	{	Fun_Ext_Int0();
		Fun_Ext_Int1();				

	}	//for end 

USART_Transmit('E');
USART_Transmit('X'); 
USART_Transmit(0x0A);//line fead
USART_Transmit(0x0D);//C R         
}	// main end  

//***********************************************************************
//***********************************************************************

SIGNAL(SIG_OVERFLOW0)
{	m_SECOND++;	
	TCNT0 =0x83;	// Timer 0 Initial Value
	//PORTC ^=(1<<PC1);	
} 
//***************************************************

SIGNAL(SIG_OVERFLOW2)
{	 m_SECOND++;	//
	TCNT2 =0x82;	// Timer 2 Initial Value
	//	PORTC ^=(1<<PC2);   
} 
//****************************************************
//**************************************************** 
SIGNAL(SIG_INTERRUPT1)
 
{ 	inc_Eint1++;
	T=m_SECOND;
   	//	PORTC ^=(1<<PC0);
	//TCCR0B =(0<<CS02)|(1<<CS01)|(1<<CS00);//pre scaller clk*64=0.125uS*64==>8uS one count
	//TCNT0 = 0x82;	// Timer 0 Initial Value
}  
//******************************************************/ 

SIGNAL(SIG_INTERRUPT0)
 
{  	inc_Eint0++;
	T=m_SECOND;	
	//T_2=m_SECOND2;
   	//	PORTC ^=(1<<PC3);
}  
//******************************************************/ 

 



