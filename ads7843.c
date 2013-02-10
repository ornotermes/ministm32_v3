/*
 * Copyright (C) 2013 Rikard Lindström <ornotermes@gmail.com>
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
 
#ifndef ADS7843_C
#define ADS7843_C

//Setup hardware
void ads7843_setup(void)
{
	//Enable clocks
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_SPI1EN); //SPI
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN); //SPI
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPCEN); //PEN_INT
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_AFIOEN); //AFIO (EXTI)
	
	//Setup GPIO
	gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, BIT13);	//PEN_INT
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, BIT6);	//MISO
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, BIT4); //CS
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, BIT5 | BIT7); //MOSI, SCK
	
	//Set up SPI as mater
	ads7843CS(1);
	spi_enable_software_slave_management(SPI1);
	spi_set_nss_high(SPI1);
	spi_set_baudrate_prescaler(SPI1, 7);
	spi_set_clock_polarity_0(SPI1);
	spi_set_clock_phase_0(SPI1);
	spi_set_dff_8bit(SPI1);
	spi_send_lsb_first(SPI1);
	spi_set_master_mode(SPI1);
	spi_enable(SPI1);
	
	//Enable PEN_INT
	nvic_enable_irq(NVIC_EXTI15_10_IRQ);
	exti_select_source(EXTI13, GPIOC);
	exti_set_trigger(EXTI13, EXTI_TRIGGER_BOTH);
	exti_reset_request(EXTI13);
	exti_enable_request(EXTI13);
	
}

uint16_t ads7843_getPos(bool y)
{

	uint16_t data = 0;
	rcc_set_ppre2(RCC_CFGR_PPRE2_HCLK_DIV4);
	ads7843CS(0);
	spi_xfer(SPI1, BIT0 | ((y)?BIT1:0) | BIT3 | BIT5);
	data |= spi_xfer(SPI1,0) <<8;
	data |= spi_xfer(SPI1,0);
	ads7843CS(1);
	rcc_set_ppre2(RCC_CFGR_PPRE2_HCLK_NODIV);
	
	return data;

}

void exti15_10_isr(void)
{
	gpio_toggle(GPIOA, BIT2); //TODO
	uint16_t touchX = ads7843_getPos(0);
	uint16_t touchY = ads7843_getPos(1);
	
	char s[] = "Touch event: (xxxxxx, xxxxxx), ";
	for(char i=0; i<6; i++)
	{
		char n = ( touchX >> (i*4) ) & 0xf;
		if (n > 9) n+='a'-10;
		else n+='0';
		s[19-i] = n;
		n = ( touchY >> (i*4) ) & 0xf;
		if (n > 9) n+='a'-10;
		else n+='0';
		s[27-i] = n;
	}
	ili9325PrintString(s);
	
	if (ads7843PEN()) ili9325PrintString("release.\n");
	else ili9325PrintString("press.\n");
	
	exti_reset_request(EXTI13);
	
}

#endif
