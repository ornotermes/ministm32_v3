/*
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2012 Rikard Lindström <ornotermes@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>
#include "ili9325.h"
#include "gpl.h"


/* Set STM32 to 72 MHz. */
void clock_setup(void)
{
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	/* Enable GPIOA, GPIOB and GPIOC clock. */
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPBEN);
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPCEN);
}

void gpio_setup(void)
{
	/* Set GPIO12 (in GPIO port C) to 'output push-pull'. */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO2);
}

int main(void)
{
	clock_setup();
	gpio_setup();
	
	ili9325Init();
	ili9325Orientation(1);
    ili9325Clear(C16_BLACK);
	ili9325Light(1);
	
	for(int d = 0; d < 18; d++)
		ili9325PrintDigit(d,d*16,0,C16_RED, C16_BLACK, C16_DK_RED);
	for(int d = 0; d < 18; d++)
		ili9325PrintDigit(d,d*16,24,C16_GREEN, C16_BLACK, C16_DK_GREEN);
	for(int d = 0; d < 18; d++)
		ili9325PrintDigit(d,d*16,48,C16_BLUE, C16_BLACK, C16_DK_BLUE);
	for(int d = 0; d < 18; d++)
		ili9325PrintDigit(d,d*16,72,C16_CYAN, C16_BLACK, C16_DK_CYAN);
	for(int d = 0; d < 18; d++)
		ili9325PrintDigit(d,d*16,96,C16_MAGENTA, C16_BLACK, C16_DK_MAGENTA);
	for(int d = 0; d < 18; d++)
		ili9325PrintDigit(d,d*16,120,C16_YELLOW, C16_BLACK, C16_DK_YELLOW);
	for(int d = 0; d < 18; d++)
		ili9325PrintDigit(d,d*16,144,C16_WHITE, C16_BLACK, C16_GRAY);
	
	ili9325Image(&gpl_width, &gpl_height, &gpl_colors, &gpl_data, 96, 175);
	
	/* Blink the LED (PC12) on the board. */
	while (1) {
		gpio_toggle(GPIOA, GPIO2);	/* LED on/off */
		for (int i = 0; i < 800000; i++)	/* Wait a bit. */
			__asm__("nop");
	}

	return 0;
}
