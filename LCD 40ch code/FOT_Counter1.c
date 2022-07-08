#include <avr/io.h>
#include <avr/delay.h>
//***************************************************************
 unsigned char count;
//***************************************************************
      
void set_portd(void)/* PortD Setup Code */ 

{  	DDRD = 0xFF;		/* RXD,TXD,  */
	PORTD = 0x00;		/* Initial Value */
} 

//***************************************************************

void set_portb(void)/* Portb Setup Code */ 

{  	DDRB = 0xFF;		/* ,  */
	PORTB = 0x00;		/*  */
} 

//***************************************************************

void set_portc(void)/* PortD Setup Code */ 

{  	DDRC = 0xFF;		/*  */
	PORTC = 0x00;		/*  */
} 

//**************************************************************
void LCD_ready(void)

{
unsigned char busy;
busy=1;

PORTB=!(1<<PB0);		////  Pb0--> RS=0 **** 
PORTB=(1<<PB1);			////Pb1-->Rw=1 // 
	while (busy==1)
		{
		busy=PIN7;
		PORTC=(1<<PC4);			//E1=1  Pulse 
		_delay_ms(1);
		PORTC=!(1<<PC4);			//E1=0  Pulse 
		
		PORTC=(1<<PC5);			//E2=1  Pulse 
		_delay_ms(1);
		PORTC=!(1<<PC5);			//E2=0  Pulse 
	
		}
return;
}
//*****************************************************************
    
void LCD1_cmd(unsigned char value)
{ 						//Check the LCD Busy flag 
LCD_ready();			//Check the LCD Busy flag 
PORTD=value;
PORTB=!(1<<PB0);		//Pb0-->RS=0  Write COMD  
PORTB=!(1<<PB1);		//Pb1-->R/W=0  Write COMD 
//PORTB=0x00;	

PORTC=!(1<<PC4);			//E1=0  Pulse 					
_delay_ms(1);
PORTC=(1<<PC4);			//E1=1  Pulse 
_delay_ms(1);
PORTC=!(1<<PC4);			//E1=0  Pulse 
//******************************************
return;  				
	
}

//**************************         *********************
void LCD2_cmd(unsigned char value)
{ 						//Check the LCD Busy flag 
LCD_ready();			//Check the LCD Busy flag 
PORTD=value;
PORTB=!(1<<PB0);		//Pb0-->RS=0  Write COMD  
PORTB=!(1<<PB1);		//Pb1-->R/W=0  Write COMD 
//PORTB=0x00;	

//******************************************
PORTC=(1<<PC5);			//E2=1  Pulse 
_delay_ms(1);
PORTC=!(1<<PC5);			//E2=0  Pulse  
	
return;  				
	
} 
     
 //***************************************************************
	
void LCD1_data(unsigned char value)
{ 
LCD_ready();		//Check the LCD Busy flag 
PORTD=value;
PORTB=(1<<PB0);		////Pb1-->Rw=0 //*****  Pb0--> RS=1 ****   
 
//PORTB=0x01;				//Pb1-->Rw=0 //  Pb0--> RS=1   

PORTC=(1<<PC4);			//E1=1  Pulse 
_delay_ms(1);
PORTC=!(1<<PC4);			//E1=0  Pulse 

PORTB=!(1<<PB0);		////Pb1-->Rw=0 //***  Pb0--> RS=0  ***  		
return;
}

//**********************************          ******************
void LCD2_data(unsigned char value)
{ 
LCD_ready();		//Check the LCD Busy flag 
PORTD=value;
PORTB=(1<<PB0);		////Pb1-->Rw=0 //*****  Pb0--> RS=1 ****   
 
//PORTB=0x01;				//Pb1-->Rw=0 //  Pb0--> RS=1   

PORTC=(1<<PC5);			//E2=1  Pulse 
_delay_ms(1);
PORTC=!(1<<PC5);			//E2=0  Pulse 

PORTB=!(1<<PB0);		////Pb1-->Rw=0 //***  Pb0--> RS=0  ***  		
return;
}

//******************************************************
          
               
void main(void)

{unsigned char i,value;
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


set_portd();				// Port D Setup 
set_portb();				// PortB Setup 
set_portc();				// Port C Setup 

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



//LCD_cmd(0xC0);				// Set Address DD Ram 0x40 2nd line location

 

}
   
               
  




	
	
