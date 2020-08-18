/*
 * Created: 5/23/2019 8:05:34 AM
 * Author : Christian Solorio
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "io.c"
//#include "usart_ATmega1284.h"
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		
		// prevents OCR3A from underflowing, using prescaler 64                    // 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB6 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM32: When counter (TCNT3) matches OCR3A, reset counter
	// CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum flashes {start, Song_Note, col1_1, col1_2, col1_3, col1_4, col2_1 , col2_2, col2_3, col2_4, col3_1, col3_2, col3_3, col3_4, col4_1, col4_2, col4_3, col4_4,} state;
	double C4 = 261.63;
	double D4 = 293.66;
	double E4 = 329.63;
	double F4 = 349.23;
	double G4 = 392.00;
	double A4 = 440.00;
	double B4 = 493.88;
	double C5 = 523.25;

double Twinkle[35]={261.63,261.63,392.00,392.00,440,440,392,349.23,349.23,329.63,329.63,293.66,293.66,261.63,392,392,349.23,349.23,329.63,329.63,293.66,392,392,349.23,349.23,329.63,329.63,293.66,261.63,261.63,392,392,440,440,392};
 
//unsigned char [4]= {0xFF,0xEF,};
unsigned char games = 0x00;
unsigned char life=0x00;
	

void transmit_data (unsigned char data){
	for (int i=0; i < 8; ++i){
		PORTB = 0x08;
		PORTB |= (data >> i) & 0x01;
		PORTB |= 0x04;
	}
	PORTB |= 0x02;
	PORTB = 0x00;

}

void transmit_dataD (unsigned char data){
	for (int i=0; i < 8; ++i){
		PORTD = 0x08;
		PORTD |= (data >> i) & 0x01;
		PORTD |= 0x04;
	}
	PORTD |= 0x02;
	PORTD = 0x00;

}
void second_data (unsigned char data){
	for (int i=0; i < 8; ++i){
		PORTB = 0x80;
		PORTB |= ((data >> i) & 0x01)<<4;
		PORTB |= 0x40;
	}
	PORTB |= 0x20;
	PORTB = 0x00;

}

void lightUp() {
	unsigned char tmpA;
	static int i;
	tmpA=~PINA& 0x01;
	switch (state)	{
				case start:
				
				if (tmpA==0x01)	{
					state=Song_Note;
					i=0;
					life=5;
					//Twinkle[i];
				}
				else {state=start;}
				break;
				
				case Song_Note:
				if (life<=0){
					state=start;
				}
				else {
				if (i>=35){
					state=start;
				}
				if ((Twinkle[i]==261.63)| (Twinkle[i]==293.66)){
					state=col1_1;
				}
				if ((Twinkle[i]==329.63)|(Twinkle[i]==349.23)){
					state=col2_1;
				}
				if ((Twinkle[i]==392.00) | (Twinkle[i]==493.88)){
					state=col3_1;
				}
				if ((Twinkle[i]==440) |(Twinkle[i]== 523.25)){
					state=col4_1;
				}	}
				break;
				
				case  col1_1:
				state=col1_2;
				break;
				
				case col1_2:
				state=col1_3;
				break;
				
				case col1_3:
				state=col1_4;
				break;
				
				case col1_4:
			state=Song_Note;
				break;
				
			case  col2_1:
			state=col2_2;
			break;
			
			case col2_2:
			state=col2_3;
			break;
			
			case col2_3:
			state=col2_4;
			break;
			
			case col2_4:
			state=Song_Note;
			break;
			
			case  col3_1:
			state=col3_2;
			break;
			
			case col3_2:
			state=col3_3;
			break;
			
			case col3_3:
			state=col3_4;
			break;
			
			case col3_4:
			state=Song_Note;
			break;
			
			case  col4_1:
			state=col4_2;
			break;
			
			case col4_2:
			state=col4_3;
			break;
			
			case col4_3:
			state=col4_4;
			break;
			
			case col4_4:
			state=Song_Note;
			break;		
				default:
				break;
				
			}	
			
			switch (state)	{
				case start:
				transmit_dataD(0x55);
				transmit_data(0x55);
						if (life==5)	{
							LCD_init();
							LCD_DisplayString(1,"PERFECT");
						}
						if (life>0 && life<5)	{
							LCD_init();
							LCD_DisplayString(1,"Good Job!");
						}
				
				break;
				
				case Song_Note:
				transmit_dataD(0xFF);
				transmit_data(0x00);
				PWM_off();
					if (life>0)
					{
						LCD_init();
						LCD_WriteData( life + '0' );
					}
					else 
					{
			LCD_init();
			LCD_DisplayString(1,"Loser!");
					}
				break;
				
				case col1_1:
				transmit_dataD(0x3F);
				transmit_data(0xC0);
				break;
				
				case col1_2:
				transmit_dataD(0xCF);
				transmit_data(0xC0);
				break;
				
				case col1_3:
			transmit_dataD(0xF3);
			transmit_data(0xC0);
			break;
			
			case col1_4:
			transmit_dataD(0xFC);
			transmit_data(0xC0);
			if (tmpA==0x01)
			{
				PWM_on();
				set_PWM(Twinkle[i]);
			}
			else 
			{
		    PWM_on();
			set_PWM(80);
			set_PWM(80);
			set_PWM(80);
			set_PWM(80);
			life--;
			}
			i++;
			
			break;	
				case col2_1:
				transmit_dataD(0x3F);
				transmit_data(0x30);
				break;
				
				case col2_2:
				transmit_dataD(0xCF);
				transmit_data(0x30);
				break;
				
				case col2_3:
				transmit_dataD(0xF3);
				transmit_data(0x30);
				break;
				case col2_4:
				transmit_dataD(0xFC);
				transmit_data(0x30);
			if (tmpA==0x01)
			{
				PWM_on();
				set_PWM(Twinkle[i]);
			}
			else
			{
				PWM_on();
				set_PWM(80);
				set_PWM(80);
				set_PWM(80);
				set_PWM(80);
			   life--;
			}
			i++;
				break;
				
				case col3_1:
				transmit_dataD(0x3F);
				transmit_data(0x0C);
				break;
				
				case col3_2:
				transmit_dataD(0xCF);
				transmit_data(0x0C);
				break;
				
				case col3_3:
				transmit_dataD(0xF3);
				transmit_data(0x0C);
				break;
				case col3_4:
				transmit_dataD(0xFC);
				transmit_data(0x0C);
			if (tmpA==0x01)
			{
				PWM_on();
				set_PWM(Twinkle[i]);
			}
			else
			{
				PWM_on();
				set_PWM(80);
				set_PWM(80);
				set_PWM(80);
				set_PWM(80);
				life--;
			}
			i++;
				
				break;
				
				case col4_1:
				transmit_dataD(0x3F);
				transmit_data(0x03);
				break;
				
				case col4_2:
				transmit_dataD(0xCF);
				transmit_data(0x03);
				break;
				
				case col4_3:
				transmit_dataD(0xF3);
				transmit_data(0x03);
				break;
				case col4_4:
				transmit_dataD(0xFC);
				transmit_data(0x03);
							if (tmpA==0x01)
							{
								PWM_on();
								set_PWM(Twinkle[i]);
							}
							else
							{
								PWM_on();
								set_PWM(80);
								set_PWM(80);
								set_PWM(80);
								set_PWM(80);
								life--;
							}
							i++;
				
				break;
			}
}


int main(void)
{
	DDRA=0x00;  PORTA=0xFF;
	DDRB=0xFF;  PORTB=0x00;
	DDRC=0xFF;  PORTC=0x00;
	DDRD=0xFF;  PORTD=0x00;
	//unsigned char tempA=0x00;
	//static char LightUp[8][8]= {{0xFF},{0xFF},{0xFF},{0xFF},{0xFF},{0xFF},{0xFF},{0xFF}};
	//	unsigned char row=0xC0;
	//	unsigned char col=0x3F;
	//unsigned char shiftRegister = LightUp;
	//unsigned char tmpA;
	//transmit_data(LightUp);
    /* Replace with your application code */
	state=start;
	TimerSet(200);
	PWM_on();
	TimerOn();
	LCD_init();
    while (1) 
    {
	
			lightUp();
		//transmit_dataD(col);
		//transmit_data(row);
		//lightUp();
		 while (!TimerFlag) {}
		 TimerFlag = 0;
		
    }
	//PWM_off;
	
}

