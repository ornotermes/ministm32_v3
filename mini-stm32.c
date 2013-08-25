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

#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "ili9325.h"
#include "ads7843.h"

//#include "back.h"


//--- Set some clocks. ---//
void clockSetup(void)
{
	// Select main clock //
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
}

void gpioSetup(void)
{

	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPBEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPCEN);
	
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, BIT2 | BIT3); //LEDs
	
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
	//ili9325PrintString("Touch screen initialized.\n");
	ads7843Calibrate();
	
	/*ili9325printf("\nPrintf-test:\nc: %c\ni: %i\nu: %u\nx: %x\nX: %X\no: %o\ns: %s\n+08i: %+08i\n_9u: %_9u\n04x; %04x\ni: %i",\
		 'x', 0-1234, 56789, 0x17af, 0xF3ED, 1597, ";D", 12, 64, 0xf3, 0);*/
	
	while(1)
	{
		ads7843Task();
		if(ads7843Press)
		{
			ili9325GoTo(ads7843X, ads7843Y);
			ili9325Point(_ili9325ColorFront);
		}
	}

	return 0;
}
