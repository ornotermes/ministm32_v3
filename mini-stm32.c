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

void printhex(unsigned int i) {
	ili9325PrintString("0x");
	for(unsigned int digit=0x1000;digit!=0;digit/=16) {
		unsigned char c=((i/digit)%16);
		if(c<10)
			ili9325PrintChar('0'+c);
		else
			ili9325PrintChar('a'+c-10);
	}
}

int main(void)
{
	clock_setup();
	gpio_setup();
	serial_setup();
	
	ili9325Init();
	ili9325Orientation(1);
	ili9325ColorSet(C16_WHITE,C16_BLUE,C16_BLACK);
	ili9325BackImage(&back_width, &back_height, &back_colors[0], &back_data[0]);
	ili9325BackMode(BACK_IMAGE);
	ili9325Clear();
	ili9325Light(1);
	
	ili9325PrintString("The cake is a lie!\n");
	
	//Display model
	char s[] = "Display: ilixxxx\n";
	for(char i=0; i<4; i++)
	{
		char n = ( _ili9325Model >> (i*4) ) & 0xf;
		if (n > 9) n+='a'-10;
		else n+='0';
		s[15-i] = n;
	}
	ili9325PrintString(s);
	
	ads7843_setup();
	ili9325PrintString("Touch screen initialized.\n");
	
	while(1)
	{
		unsigned long x=ads7843_getPos(0)/8;
		unsigned long y=ads7843_getPos(1)/8;
		ili9325SetLocation(30,215);
		ili9325PrintString("Touch event: ");
		printhex(x);
		ili9325PrintString(", ");
		printhex(y);
		if (ads7843PEN()) ili9325PrintString(" release.\n");
		else              ili9325PrintString(" press.  \n");
		
		if (!ads7843PEN()) {
			ili9325GoTo(320-x*320/0x1000, y*240/0x1000);
			ili9325Point(C16_WHITE);
		}
	}

	return 0;
}
