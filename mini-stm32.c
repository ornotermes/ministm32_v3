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

#include "back.h"

//--- Set some clocks. ---//
void clock_setup(void)
{
	// Select main clock //
	rcc_clock_setup_in_hse_8mhz_out_72mhz();
}

void gpio_setup(void)
{

	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPBEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPCEN);
	
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, BIT2 | BIT3); //LEDs
	gpio_toggle(GPIOA, BIT2);
	
}

//--- Set up USART1 with 115200bps 8N1 ---/
void serial_setup(void)
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
	clock_setup();
	gpio_setup();
	serial_setup();
	
	ili9325Init();
	ili9325Orientation(1);
	ili9325BackInit(&back_width, &back_height, &back_colors[0], &back_data[0]);
	ili9325BackDraw();
	ili9325Light(1);
	
	//Hello world in two colors
	ili9325SetLocation(0,0);
	ili9325SetColor(C16_BLACK,0,0);
	ili9325PrintStringBlend("The cake is a lie!\n");
	
	//Display model
	char s[] = "Display: ilixxxx\n";
	for(char i=0; i<4; i++)
	{
		char n = ( _ili9325Model >> (i*4) ) & 0xf;
		if (n > 9) n+='a'-10;
		else n+='0';
		s[15-i] = n;
	}
	ili9325PrintStringBlend(s);
	
	ads7843_setup();
	ili9325PrintStringBlend("Touch screen initialized.\n");
	
	while(1)
	{
	}

	return 0;
}
