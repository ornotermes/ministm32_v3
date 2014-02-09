/*
 * Copyright (C) 2012-2013 Rikard Lindström <ornotermes@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//---- Includes headers ------------------------------------------------------//

#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdlib.h>

#include "ili9325.h"
#include "ads7843.h"
#include "syscall.c"
#include "clock.h"
//#include "back.h"

//---- Variables -------------------------------------------------------------//

bool button1 = 0;
bool button2 = 0;
unsigned int button1Count = 0;
unsigned int button2Count = 0;

//---- Function prototypes ---------------------------------------------------//

void clockSetup(void);
void gpioSetup(void);
void serialSetup(void);
void buttonTask(void);
int main(void);

//---- Source ----------------------------------------------------------------//

//--- Set some clocks. ---//
void clockSetup(void)
{
	// Select main clock //
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	//Make sure RTC is running
	rtc_auto_awake(LSE, 0x7fff);
}

void gpioSetup(void)
{

	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPBEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPCEN);
	
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, BIT2 | BIT3); //LEDs
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, BIT0 | BIT1); //Buttons
	gpio_set(GPIOA, BIT0 | BIT1); //pull up
	
}

//--- Set up USART1 with 115200bps 8N1 ---/
void serialSetup(void)
{

	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_USART1EN);

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);

	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_enable(USART1);

}

void buttonTask(void)
{
	button1 = !gpio_get(GPIOA, BIT0);
	if(button1) button1Count++;
	else button1Count = 0;
	
	button2 = !gpio_get(GPIOA, BIT1);
	if(button2) button2Count++;
	else button2Count = 0;
}

int main(void)
{
	clockSetup();
	gpioSetup();
	serialSetup();
	
	//Init display
	ili9325Init();
	ili9325Orientation(1);
	ili9325ColorSet(C16_WHITE,C16_BLUE,C16_BLACK);
	//ili9325BackImage(&back_width, &back_height, &back_colors[0], &back_data[0]);
	//ili9325BackMode(BACK_IMAGE);
	ili9325Clear();
	ili9325Light(1);
	//ili9325printf("Display: ili%04i\n", _ili9325Model;
	
	//Init touch
	ads7843Setup();
	ili9325PrintString("Press KEY1 to re-calibrate.\n");
	ili9325PrintString("Press KEY2 to set time.\n");
	
	/*ili9325printf("\nPrintf-test:\nc: %c\ni: %i\nu: %u\nx: %x\nX: %X\no: %o\ns: %s\n+08i: %+08i\n_9u: %_9u\n04x; %04x\ni: %i",\
		 'x', 0-1234, 56789, 0x17af, 0xF3ED, 1597, ";D", 12, 64, 0xf3, 0);*/
	
	while(1)
	{
		ClockShow();
				
		buttonTask();
		if(button1Count == 10) ads7843Calibrate();
		if(button2Count == 10) ClockSet();
		
		ads7843Task();
		if(ads7843Press)
		{
			ili9325GoTo(ads7843X, ads7843Y);
			ili9325Point(_ili9325ColorFront);
		}
	}
	
	return 0;
}
