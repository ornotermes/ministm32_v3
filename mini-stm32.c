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
#include "ili9325.h"
#include "gpl.h"
#include "rainbow.h"

/* Set STM32 to 72 MHz. */
void setup(void)
{
	/* Select main clock */
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	/* Enable GPIOA, GPIOB and GPIOC clock. */
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPBEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPCEN);
}

int main(void)
{
	setup();
	
	ili9325Init();
	ili9325Orientation(1);
    ili9325Clear(C16_BLACK);
	ili9325Light(1);
	
	//rainbow banner
	ili9325Image(&rainbow_width, &rainbow_height, &rainbow_colors, &rainbow_data, 0, 0);
	
	//Hello world in two colors
	ili9325SetLocation(120,24);
	ili9325SetColor(C16_GREEN,0,0);
	ili9325PrintString("Hello ");
	ili9325SetColor(C16_YELLOW,0,0);
	ili9325PrintString("World");
	ili9325SetColor(C16_GREEN,0,0);
	ili9325PrintString("!");
	
	//Normal 8px text
	ili9325SetLocation(0,64);
	ili9325SetFont(8);
	ili9325SetColor(C16_CYAN,0,0);
	ili9325PrintString("1234abcdABCD?!@,._");
	
	//Bold 8px text
	ili9325SetLocation(0,80);
	ili9325SetFont(9);
	ili9325SetColor(C16_MAGENTA,0,0);
	ili9325PrintString("1234abcdABCD?!@,._");
	
	//7-seg display
	ili9325SetColor(C16_BLUE,0,C16_DK_BLUE);
	for(uint8_t d = 0; d < 18; d++)
	{
		ili9325SetLocation(0,80);
		ili9325PrintDigit(d,d*16,120);
	}
		
	//GPL v3 symbol
	ili9325Image(&gpl_width, &gpl_height, &gpl_colors, &gpl_data, 96, 180);
	
	while(1);

	return 0;
}
