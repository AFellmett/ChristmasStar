/*
 * Christmas Star.c
 *
 * Created: 24.11.2018 22:47:02
 * Author : afell
 */ 

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "light_ws2812_config.h"
#include "light_ws2812.h"

#define MAXPIX 2
#define FADE 1
#define MAXCOLORS 9

struct cRGB colors[MAXCOLORS];

uint8_t selectedColor = 1;

ISR(PCINT0_vect)
{
	if((PINB & (1 << PINB1)) == 0)
	{
		selectedColor++;
		selectedColor%=MAXCOLORS;
	}
}


int main(void)
{
	uint8_t i = 0;

	#ifdef __AVR_ATtiny10__
	CCP=0xD8;		// configuration change protection, write signature
	CLKPSR=0;		// set cpu clock prescaler =1 (8Mhz) (attiny 4/5/9/10)
	#endif
	
	// Set direction (1 = output; 0 = input)
	DDRB |= (1<<DDB2);
	DDRB &= ~(1<<DDB1);
	
	// Set pull-up (1 = enable; 0 = disable)
	PUEB &= ~(1<<PUEB2);
	PUEB |= (1<<PUEB1);
	
	// Set level (1 = HIGH; 0 = LOW)
	PORTB &= ~(1 << PORTB2);
	
	// Enable pin change interrupt
	PCICR |= (1 << PCIE0);    
	
	// Enable pin change interrupt for PCINT1
	PCMSK |= (1 << PCINT1);    
	
	// Clear interrupt flag
	//PCIFR = 1;
	
	// Enable interrupts
	sei();

	//Black & White
	colors[0].r=000; colors[0].g=000; colors[0].b=000;//off
	colors[1].r=255; colors[1].g=255; colors[1].b=255;//white

	//Rainbowcolors
	colors[2].r=255; colors[2].g=000; colors[2].b=000;//red
	colors[3].r=255; colors[3].g=100; colors[3].b=000;//orange
	colors[4].r=100; colors[4].g=255; colors[4].b=000;//yellow
	colors[5].r=000; colors[5].g=255; colors[5].b=000;//green
	colors[6].r=000; colors[6].g=100; colors[6].b=255;//light blue (türkis)
	colors[7].r=000; colors[7].g=000; colors[7].b=255;//blue
	colors[8].r=155; colors[8].g=000; colors[8].b=255;//violet

	uint8_t currentselection = MAXCOLORS;
	while(1)
	{	
		if(currentselection != selectedColor)
		{
			for(i = 0; i < MAXPIX;i++)
				ws2812_sendarray((uint8_t *)&colors[selectedColor],3);		
		}
		_delay_ms(500);
		selectedColor++;
		selectedColor%=MAXCOLORS;
	}
}
