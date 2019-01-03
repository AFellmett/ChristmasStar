/*
 * Christmas Star.c
 *
 * Created: 24.11.2018 22:47:02
 * Author : afell
 */ 

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "light_ws2812.h"

#define MAXPIX 2
#define MAXCOLORS 10
#define MAXDENSITY 0xaf

#define INC_RED (1<<0)
#define DEC_RED (1<<1)
#define INC_GREEN (1<<2)
#define DEC_GREEN (1<<3)
#define INC_BLUE (1<<4)
#define DEC_BLUE (1<<5)

#define COLOR_RED MAXDENSITY,0,0
#define COLOR_ORANGE MAXDENSITY,MAXDENSITY/4,0
#define COLOR_YELLOW MAXDENSITY,MAXDENSITY/2,0
#define COLOR_GREEN 0,MAXDENSITY,0
#define COLOR_LIGHT_BLUE 0,MAXDENSITY,MAXDENSITY
#define COLOR_BLUE 0,0,MAXDENSITY
#define COLOR_VIOLET MAXDENSITY,0,MAXDENSITY
#define COLOR_WHITE MAXDENSITY,MAXDENSITY,MAXDENSITY
#define COLOR_BLACK 0,0,0

uint8_t selectedColor = MAXCOLORS-1;
struct cRGB led;

ISR(PCINT0_vect)
{
	if(!(PINB & (1 << PINB1)))
	{
		selectedColor++;
		selectedColor%=MAXCOLORS;
	}
}

void fade(uint8_t red, uint8_t green, uint8_t blue)
{
	uint8_t start = 0;
	uint8_t i = 0;
	
	if(led.r > red)
		start |= DEC_RED;
	if(led.r < red)
		start |= INC_RED;
	if(led.g > green)
		start |= DEC_GREEN;
	if(led.g < green)
		start |= INC_GREEN;
	if(led.b > blue)
		start |= DEC_BLUE;
	if(led.b < blue)
		start |= INC_BLUE;
	while(led.r != red || led.g != green || led.b != blue)
	{
		if((start & INC_RED)== INC_RED)
			led.r++;
		if((start & DEC_RED)== DEC_RED)
			led.r--;
		if((start & INC_GREEN)== INC_GREEN)
			led.g++;
		if((start & DEC_GREEN)== DEC_GREEN)
			led.g--;
		if((start & INC_BLUE)== INC_BLUE)
			led.b++;
		if((start & DEC_BLUE)== DEC_BLUE)
			led.b--;
		if(led.r == red)
		{
			start &= ~INC_RED;
			start &= ~DEC_RED;
		}
		if(led.g == green)
		{
			start &= ~INC_GREEN;
			start &= ~DEC_GREEN;
		}
		if(led.b == blue)
		{
			start &= ~INC_BLUE;
			start &= ~DEC_BLUE;
		}
		for(i = 0; i < MAXPIX; i++)
			ws2812_sendarray((uint8_t*)&led,3);
		_delay_ms(7);
	}
}

void rainbow()
{
	uint8_t start = 0;
	uint8_t i = 0;
	fade(COLOR_RED);
	while(selectedColor == 8)
	{
		if((start & INC_RED)== INC_RED)
			led.r++;
		if((start & DEC_RED)== DEC_RED)
			led.r--;
		if((start & INC_GREEN)== INC_GREEN)
			led.g++;
		if((start & DEC_GREEN)== DEC_GREEN)
			led.g--;
		if((start & INC_BLUE)== INC_BLUE)
			led.b++;
		if((start & DEC_BLUE)== DEC_BLUE)
			led.b--;
		if(led.r == MAXDENSITY)
		{
			start &= ~DEC_BLUE;
			start &= ~INC_RED;
			start |= DEC_RED;
			start |= INC_GREEN;
		}
		if(led.g == MAXDENSITY)
		{
			start &= ~DEC_RED;
			start &= ~INC_GREEN;
			start |= DEC_GREEN;
			start |= INC_BLUE;
		}
		if(led.b == MAXDENSITY)
		{
			start &= ~DEC_GREEN;
			start &= ~INC_BLUE;
			start |= DEC_BLUE;
			start |= INC_RED;
		}
		for(i = 0; i < MAXPIX; i++)
			ws2812_sendarray((uint8_t*)&led,3);
		_delay_ms(1000);
	}
}

int main(void)
{
	#ifdef __AVR_ATtiny10__
	CCP=0xD8;		// configuration change protection, write signature
	CLKPSR=0;		// set cpu clock prescaler =1 (8Mhz) (attiny 4/5/9/10)
	#endif
	
	// Set direction (1 = output; 0 = input)
	DDRB &= ~(1<<DDB1);
	
	// Set pull-up (1 = enable; 0 = disable)
	PUEB |= (1<<PUEB1);
	
	// Enable pin change interrupt
	PCICR |= (1 << PCIE0);    
	
	// Enable pin change interrupt for PCINT1
	PCMSK |= (1 << PCINT1);    
	
	// Clear interrupt flag
	//PCIFR = 1;
	
	//Enable interrupts	
	sei();
	//uint8_t currentSelection = selectedColor;
	while(1)
	{	
		//if(currentSelection != selectedColor)
		//{
			switch(selectedColor)
			{
				case 0: fade(COLOR_RED);
					break;
				case 1: fade(COLOR_ORANGE);
					break;
				case 2: fade(COLOR_YELLOW);
					break;
				case 3: fade(COLOR_GREEN);
					break;
				case 4: fade(COLOR_LIGHT_BLUE);
					break;
				case 5: fade(COLOR_BLUE);
					break;
				case 6: fade(COLOR_VIOLET);
					break;
				case 7: fade(COLOR_WHITE);
					break;
				case 8: rainbow();
					break;
				case MAXCOLORS:
				default: fade(COLOR_BLACK);			
			}
//		currentSelection = selectedColor;
		//}
//		_delay_ms(250);
	}
}
