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
void ads7843Setup(void)
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
	spi_set_master_mode(SPI1);
	spi_enable(SPI1);
	
	//Enable PEN_INT
	nvic_enable_irq(NVIC_EXTI15_10_IRQ);
	exti_select_source(EXTI13, GPIOC);
	exti_set_trigger(EXTI13, EXTI_TRIGGER_BOTH);
	exti_reset_request(EXTI13);
	exti_enable_request(EXTI13);
	
	ads7843CalRead();
	
	/*TODO check for cal data*/
	
}

uint16_t ads7843GetPos(bool y)
{

	uint16_t data = 0;
	rcc_set_ppre2(RCC_CFGR_PPRE2_HCLK_DIV4);
	ads7843CS(0);
	spi_xfer(SPI1, BIT7 | ((y)?BIT6:0) | BIT4 | BIT2);
	data |= spi_xfer(SPI1,0)<<8;
	data |= spi_xfer(SPI1,0);
	ads7843CS(1);
	rcc_set_ppre2(RCC_CFGR_PPRE2_HCLK_NODIV);
	
	return data;

}

void ads7843Task(void)
{
	ads7843Press = !ads7843PEN();
	if (ads7843Press) {
		_ads7843RawX = ads7843GetPos(0);
		_ads7843RawY = ads7843GetPos(1);
		/*TODO calculate adjusted X/Y*/
		ads7843X = 320 - (_ads7843RawX - _ads7843CalOffsetX) / _ads7843CalScaleX;
		ads7843Y = (_ads7843RawY - _ads7843CalOffsetY) / _ads7843CalScaleY;
	}
}

void ads7843Calibrate(void)
{
	unsigned int calVal[3][2];
	unsigned char calNum = 0;
	ili9325Clear();
	ads7843Calibrated = 0;
	while(!ads7843Calibrated)
	{
		ili9325SetLocation(70,10);
		ili9325PrintString("Press targets to calibrate.");
		switch(calNum)
		{
		case 0:
			ili9325Mask(&target_width, &target_height, &target_data, 80-16, 120-16);
			break;
		case 1:
			ili9325Mask(&target_width, &target_height, &target_data, 240-16, 60-16);
			break;
		case 2:
			ili9325Mask(&target_width, &target_height, &target_data, 160-16, 180-16);
			break;
		default:
			ili9325Clear();
			
			ili9325printf("Raw cal data:\n0: (%i, %i)\n1: (%i, %i)\n2: (%i, %i)\n", calVal[0][0], calVal[0][1], calVal[1][0], calVal[1][1], calVal[2][0], calVal[2][1]);
			
			_ads7843CalScaleX = ((calVal[0][0] - calVal[1][0])*2)/320;
			_ads7843CalScaleY = ((calVal[2][1] - calVal[1][1])*2)/240;
			_ads7843CalOffsetX = calVal[1][0] - (calVal[2][0] - calVal[1][0]);
			_ads7843CalOffsetY = calVal[1][1] - (calVal[0][1] - calVal[1][1]);
			
			ili9325printf("Calculated calibration:\nScale X: %i\nScale Y: %i\nOffset X: %i\nOffset Y: %i\n", _ads7843CalScaleX, _ads7843CalScaleY, _ads7843CalOffsetX, _ads7843CalOffsetY);
			
			/*TODO: save cal data*/
			
			ads7843Calibrated = 1;
			
			ads7843CalWrite();
			ili9325PrintString("Calibration data stored.\n");
			
			ili9325PrintString("Tap the screen to continue.\n");
			ads7843Wait();
			ili9325Clear();
			return;
		}
		ads7843WaitPress();
		calVal[calNum][0] = _ads7843RawX;
		calVal[calNum][1] = _ads7843RawY;
		ads7843WaitRelease();
		ili9325Clear();
		calNum++;
	}
}

//Wait for touchscreen to be pressed, then released
void ads7843Wait(void)
{
	ads7843WaitPress();
	ads7843WaitRelease();
}

void ads7843CalWrite(void)
{
	_ads7843RegCalScaleX = _ads7843CalScaleX;
	_ads7843RegCalScaleY = _ads7843CalScaleY;
	_ads7843RegCalOffsetX = _ads7843CalOffsetX;
	_ads7843RegCalOffsetY = _ads7843CalOffsetY;
}

bool ads7843CalRead(void)
{
	if(_ads7843RegCalScaleX!=0 || _ads7843RegCalScaleY!=0 || _ads7843RegCalOffsetX!=0 || _ads7843RegCalOffsetY!=0)
	{
		_ads7843CalScaleX = _ads7843RegCalScaleX;
		_ads7843CalScaleY = _ads7843RegCalScaleY;
		_ads7843CalOffsetX = _ads7843RegCalOffsetX;
		_ads7843CalOffsetY = _ads7843RegCalOffsetY;
		ads7843Calibrated = 1;
	}
	return ads7843Calibrated;
}


void exti15_10_isr(void)
{
	exti_reset_request(EXTI13);
}

#endif
