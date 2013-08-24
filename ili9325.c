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
 
#ifndef ILI9325_C
#define ILI9325_C

#include <stdarg.h>

//Set port direction. 0: output, 1: input.
void ili9325PortDirection(bool input)
{
	if (input)
	{
		gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, 0xff00);	//PB8:PB15=(DB8:DB15)
		gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, 0x00ff);	//PC0:PC12=(DB0:DB7)
	}
	else //output
	{
		gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, 0xff00);	//PB8:PB15=(DB8:DB15)
		gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, 0x00ff);	//PC0:PC12=(DB0:DB7)
	}
}

//Get data from DB0:DB15
uint16_t ili9325PortRead()
{
	return gpio_get(GPIOC, 0x00ff) | gpio_get(GPIOB, 0xff00);
}

//Write a command, like a register address
void ili9325WriteCommand(uint16_t command)
{
	ili9325RS(0);
	ili9325PortWrite(command);
	ili9325WR(0);
	ili9325WR(1);
	ili9325RS(1);
}

//Write data to register
void ili9325WriteRegister(uint16_t reg, uint16_t data)
{
	ili9325CS(0);
	ili9325WriteCommand(reg);
	ili9325WriteData(data);
	ili9325CS(1);
}

//Read a register
uint16_t ili9325ReadReg(uint16_t reg)
{
	uint16_t data = 0;
	ili9325CS(0);
	ili9325WriteCommand(reg);
	ili9325PortDirection(1);
	ili9325RD(0);
	data = gpio_get(GPIOB, 0xff00) | gpio_get(GPIOC, 0x00ff);
	ili9325RD(1);
	ili9325PortDirection(0);
	ili9325CS(1);
	return data;
}

//Read current data
uint16_t ili9325ReadData()
{
	uint16_t data = 0;
	ili9325PortDirection(1);
	ili9325RD(0);
	data = ili9325PortRead();
	ili9325RD(1);
	ili9325PortDirection(0);
	return data;
}

void ili9325Init(void)
{

	// Hardware setup //

	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, 0x1f00);	//PC8=CS, PC9=RS, PC10=WR, PC11=RD, PC12=LIGHT)
	ili9325CS(1);
	ili9325RS(1);
	ili9325WR(1);
	ili9325RD(1);
	
	ili9325PortDirection(0);
	
	// Software Init //
		
	_ili9325Model = ili9325ReadReg(0x0000);
	
	switch (_ili9325Model)
	{
	
	//Init code for ili9325
	case 0x9325:
		//PSU part I (set VC[2:0], VRH[3:0], VCM[5:0], VDV[5:0], PON=0, BT[2:0]=000)
		ili9325WriteRegister(0x0010,0x0000); //Power Control 1 - BTmask0x0700
		ili9325WriteRegister(0x0011,0x0007); //Power Control 2 - VCmask0x0007=0x0007(Vref(Vci1)=1.0 * Vci)
		ili9325WriteRegister(0x0012,0x008d); //Power Control 3 - VCIREmask0x0080=0x0080(2.5Vref), VRHmask0x000f=0x000d(VREG1OUT=4.625V), PONmask0x0000=0x0000(VGL output enable)
		ili9325WriteRegister(0x0013,0x1900); //Power Control 4 - VDVmask0x1f00=0x1900(Vcom = VREG1OUT * 1.24)
		ili9325WriteRegister(0x0029,0x0025); //Power Control 7 - VCMmask0x003f=0x0025(VcomH = VREG1OUT * 0.87)
	
		//stabilizing time for PSU(rec 50ms+)
		for (int i = 0; i < 15000; i++) __asm__("nop");
	
		//PSU part II (Set BT[2:0], PON=1, AP[2:0], APE=1, DC1[2:0], DC2[2:0]
		ili9325WriteRegister(0x0010,0x1690); //Power Control 1 - SAPmask0x1000=0x1000(Source driver enable), BTmask0x0700=0x0600(step up factor), APmask0x0070=0x0010(Contrast G1.0/S1.0), APEmask0x0080=0x0080(enable PSU)
		ili9325WriteRegister(0x0012,0x009d); //Power Control 3 - VCIREmask0x0080=0x0080(2.5Vref), VRHmask0x000f=0x000d(VREG1OUT=4.625V), PONmask0x0000=0x0010(VGL output enable)
	
		//stabilizing time for PSU(rec 80ms+)
		for (int i = 0; i < 80000; i++) __asm__("nop");
	
		//Set other registers
		ili9325WriteRegister(0x0060,0x2700); //Gate Scan Control 1 - LVmask0x3f00=0x2700(320 lines)
		ili9325WriteRegister(0x0061,0x0001); //Gate Scan Control 2 - REVmask0x0001=0x0001(greyscale inverstion)
		ili9325WriteRegister(0x0003,0x1030); //Set default GRAM Acces Direction for default rotation (vertical/0)
		ili9325WriteRegister(0x002B,0x000e); //Set frame rate to 112 fps
	
		//Enable display
		ili9325WriteRegister(0x0007,0x0133); //Display Control 1
		break;
		for (int i = 0; i < 30000; i++) __asm__("nop");
	
	//if display not identified, flash led
	default:
		gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, BIT2);
		while(1)
		{
			gpio_toggle(GPIOA, BIT2);
			for(int i = 0; i<500000; i++)__asm__("nop");
		}
	
	}
	
}

//Set screen orientation
void ili9325Orientation(uint8_t rot)
{
	bool scanV = 0, incH = 0, incV = 0;
	switch (rot)
	{
		case 0: //0 deg
			_ili9325Rotation = 0;
			incV = 1;
			incH = 1;
			break;
		case 1: //90 deg
			_ili9325Rotation = 1;
			scanV = 1;
			incV = 1;
			break;
		case 2: //180 deg
			_ili9325Rotation = 2;
			break;
		case 3: //270 deg
			_ili9325Rotation = 3;
			scanV = 1;
			incH = 1;
			break;
		default:
			__asm__("nop");
	}
	uint16_t temp = (1<<12)|(incV<<5)|(incH<<4)|(scanV<<3);
	ili9325WriteRegister(0x0003, temp);
}

//Go to the screenlocation x,y for your screen rotation.
void ili9325GoTo(uint16_t x, uint16_t y)
{
	uint16_t tx = 0, ty = 0;
	switch (_ili9325Rotation)
	{
		case 0:
			tx = x;
			ty = y;
			break;
		case 1:
			tx = ili9325Width - y;
			ty = x;
			break;
		case 2:
			tx = ili9325Width - x;
			ty = ili9325Height - y;
			break;
		case 3:
			tx = y;
			ty = ili9325Height - x;
			break;
	}
	ili9325WriteRegister(0x0020,tx); //Native X pos
	ili9325WriteRegister(0x0021,ty); //Native Y pos
}

//Put down a single colored dot.
void ili9325Point(uint16_t color)
{
	ili9325WriteRegister(0x0022,color);
}

//Print a single 7seg digit
void ili9325PrintDigit(uint8_t digit, uint16_t x, uint16_t y)
{
	uint16_t color = 0;
	for(uint8_t ty = 0; ty < 24; ty++)
	{
		ili9325GoTo(x,y+ty);
		ili9325CS(0);
		ili9325WriteCommand(0x0022);
		for(uint8_t tx = 0; tx < 17; tx++)
		{
			if((font_7seg[(digit*24)+ty]&(1<<(16-tx))) > 0) color = _ili9325ColorFront;
			else if((font_7seg[(8*24)+ty]&(1<<(16-tx))) > 0) color = _ili9325ColorFill;
			else color = _ili9325ColorBack;
			ili9325WriteData(color);
		}
		ili9325CS(1);
	}
}

//Select wich font to use. 
void ili9325SetFont(uint8_t font)
{
	switch (font)
	{
	case 6: //6px normal
		_ili9325FontWidth = &font6x12Width;
		_ili9325FontHeight = &font6x12Height;
		_ili9325FontData = &font6x12Data;
		break;
	case 8: //8px normal
		_ili9325FontWidth = &font8x16Width;
		_ili9325FontHeight = &font8x16Height;
		_ili9325FontData = &font8x16Data;
		break;
	case 9: //8px bold
		_ili9325FontWidth = &font8x16bWidth;
		_ili9325FontHeight = &font8x16bHeight;
		_ili9325FontData = &font8x16bData;
		break;	
	}
}

//Set text location
void ili9325SetLocation(uint16_t x, uint16_t y)
{
	_ili9325LocationX = x;
	_ili9325LocationY = y;
}

//Set colors to use
void ili9325ColorSet(uint16_t front, uint16_t back, uint16_t fill)
{
	_ili9325ColorFront = front;
	_ili9325ColorBack = back;
	_ili9325ColorFill = fill;
}

//Print a single char
void ili9325PrintChar(char character)
{
	uint16_t color = 0;
	switch(character)
	{
	case '\n':
	case 13: //CR
		_ili9325LocationY += (*_ili9325FontHeight);
		if (_ili9325LocationY>((*_ili9325BackHeight)-(*_ili9325FontHeight))) _ili9325LocationY=0;
		_ili9325LocationX = _ili9325TextXOffset;
		break;
		
	default:
		character -= 32;
		for	(uint16_t y = 0; y < (*_ili9325FontHeight); y++)
		{
			ili9325GoTo(_ili9325LocationX, _ili9325LocationY+y);
			ili9325CS(0);
			ili9325WriteCommand(0x0022);
			uint8_t charByte = (*_ili9325FontData)[(character)*(*_ili9325FontHeight)+y];
			for (uint16_t x = 0; x < (*_ili9325FontWidth); x++)
			{
				if ( ( charByte & ( 1<<(8-x) ) ) > 0) 
				{
					if ( _ili9325FontMode == FONT_SOLID ) color = _ili9325ColorFront;
					if ( _ili9325BackMode == FONT_GRADIENT ) __asm__("nop");
				}
				else
				{
					if ( _ili9325BackMode == BACK_SOLID ) color = _ili9325ColorBack;
					if ( _ili9325BackMode == BACK_IMAGE ) color = *(_ili9325BackColors + ( *(_ili9325BackData + (*_ili9325BackWidth)*(_ili9325LocationY+y-1)+(_ili9325LocationX+x) )));
				}
				ili9325WriteData(color);
			}
			for (uint16_t x = (*_ili9325FontWidth); x < (*_ili9325FontWidth)+(*_ili9325FontSpace); x++)
			{
				if ( _ili9325BackMode == BACK_SOLID ) { ili9325WriteData(_ili9325ColorBack); }
				if ( _ili9325BackMode == BACK_IMAGE ) { ili9325WriteData(*(_ili9325BackColors + ( *(_ili9325BackData + (*_ili9325BackWidth)*(_ili9325LocationY+y-1)+(_ili9325LocationX+x) )))); }
			}
			ili9325CS(1);
		}
		_ili9325LocationX += (*_ili9325FontWidth) + (*_ili9325FontSpace);
		break;
	}
}

void ili9325PrintString(char chars[])
{
	for(uint8_t c = 0; chars[c] != 0; c++)
	{
		ili9325PrintChar(chars[c]);
	}
}

//Draw image generated with img2h.py
void ili9325Image(const uint16_t (*width), const uint16_t (*height), const uint16_t (*colors)[], const uint8_t (*data)[], uint16_t destX, uint16_t destY)
{
	for	(uint16_t y = 0; y < (*height); y++)
	{
		ili9325GoTo(destX, destY+y);
		ili9325CS(0);
		ili9325WriteCommand(0x0022);
		for (uint16_t x = 0; x < (*width); x++)
		{
			ili9325WriteData( (*colors)[(*data)[(*width)*y+x]] );
		}
		ili9325CS(1);
	}
}

void ili9325BackImage(const uint16_t *width, const uint16_t *height, const uint16_t *colors, const uint8_t *data)
{
	_ili9325BackWidth = width;
	_ili9325BackHeight = height;
	_ili9325BackColors = colors;
	_ili9325BackData = data;
}

void ili9325BackMode(uint8_t mode)
{
	_ili9325BackMode = mode;
}

void ili9325Clear(void)
{
	_ili9325LocationX = 0;
	_ili9325LocationY = 0;
	
	for	(uint16_t y = 0; y < ili9325Width; y++)
	{
		ili9325GoTo(0, 1+y);
		ili9325CS(0);
		ili9325WriteCommand(0x0022);
		for (uint16_t x = 0; x < ili9325Height; x++)
		{
			if ( _ili9325BackMode == BACK_SOLID )
			{
				ili9325WriteData( _ili9325ColorBack );
			}
			if ( _ili9325BackMode == BACK_IMAGE )
			{
				if ( (y < (*_ili9325BackHeight)) & (x < (*_ili9325BackWidth)))
				{
					ili9325WriteData(*(_ili9325BackColors + ( *(_ili9325BackData + (*_ili9325BackWidth)*y+x) )));
				}
				else
				{
					ili9325WriteData( _ili9325ColorBack );
				}
			}
		}
		ili9325CS(1);
	}
}

void ili9325printf( char * fmt, ... )
{
	va_list args;
	char *p; //Pointer to fmt char
	char format = 0; //In format reading mode
	char formatFill = ' '; //Fill character
	char formatSign = 0; //Always sign (+/-)
	char formatReset = 0; //Reset all settings when we are done with a format
	char formatOutput[13]; //Temporary output buffer
	char i = 0; //For flipping formatOutput
	int formatPadding = 0; //How many charecters the format should use
	
	int formatInt = 0;
	unsigned int formatUnsigned = 0;
	char *formatString;
	
	va_start (args, fmt);
	
	for(p=fmt; *p>0; p++)
	{	
		if (*p == '%')
		{
			format = 1;
			p++;
		}
		

		if(format)
		{
			switch(*p)
			{				
			case 'c':
				formatInt = va_arg( args, int );
				ili9325PrintChar(formatInt);
				formatReset = 1;
				break;
				
			case 'd':
			case 'i':
				formatInt = va_arg( args, int ); //Load next number from argument list
				if(formatSign && formatInt>=0) ili9325PrintChar('+'); //Print + if sign is requested and number is poitive
				if(formatInt<0)
				{
					ili9325PrintChar('-'); //always show - if number i negative
					formatInt = formatInt - formatInt - formatInt; //make number positive to make modulud and division to work
				}
				i = 0;
				if( formatInt == 0 )
				{
					formatOutput[i] = (char)('0'); //If number is zero show that, the function for other numbers won't do it
					i++;
				}
				for(; formatInt; i++) //Convert the number, when formatInt == 0 there is nothing more to convert
				{
					char mod = formatInt % 10; //Extract least significant base 10 digit
					formatOutput[i] = (char)('0'+mod); //Add the numerical value of '0' to make a character
					formatInt = (formatInt - mod) / 10; //Divide by 10 to create a new least significant digit
				}
				for(int j = 0; j < (formatPadding - i - formatSign); j++) //calculate about of padding needed.
				{
					ili9325PrintChar(formatFill);
				}
				for( ; i > 0; i--) ili9325PrintChar(formatOutput[i-1]); //Print the last extracted digit first (that was most significat digit in the original number)
				formatReset = 1; //We are done with this format, clean up
				break;
				
			case 'u':
				formatUnsigned = va_arg( args, unsigned int );
				i = 0;
				if( formatUnsigned == 0 )
				{
					formatOutput[i] = (char)('0');
					i++;
				}
				for(; formatUnsigned; i++)
				{
					char mod = formatUnsigned % 10;
					formatOutput[i] = (char)('0'+mod);
					formatUnsigned = (formatUnsigned - mod) / 10;
				}
				for(int j = 0; j < (formatPadding - i); j++) 
				{
					ili9325PrintChar(formatFill);
				}
				for( ; i > 0; i--) ili9325PrintChar(formatOutput[i-1]);
				formatReset = 1;
				break;
				
			case 'x':
				formatUnsigned = va_arg( args, unsigned int );
				i = 0;
				if( formatUnsigned == 0 )
				{
					formatOutput[i] = (char)('0');
					i++;
				}
				for(; formatUnsigned; i++)
				{
					char mod = formatUnsigned % 16;
					if (mod > 9) formatOutput[i] = (char)('a'+mod-10);
					else formatOutput[i] = (char)('0'+mod);
					formatUnsigned = (formatUnsigned - mod) / 16;
				}
				for(int j = 0; j < (formatPadding - i); j++) 
				{
					ili9325PrintChar(formatFill);
				}
				for( ; i > 0; i--) ili9325PrintChar(formatOutput[i-1]);
				formatReset = 1;
				break;
				
			case 'X':
				formatUnsigned = va_arg( args, unsigned int );
				i = 0;
				if( formatUnsigned == 0 )
				{
					formatOutput[i] = (char)('0');
					i++;
				}
				for(i = 0; formatUnsigned; i++)
				{
					char mod = formatUnsigned % 16;
					if (mod > 9) formatOutput[i] = (char)('A'+mod-10);
					else formatOutput[i] = (char)('0'+mod);
					formatUnsigned = (formatUnsigned - mod) / 16;
				}
				for(int j = 0; j < (formatPadding - i); j++) 
				{
					ili9325PrintChar(formatFill);
				}
				for( ; i > 0; i--) ili9325PrintChar(formatOutput[i-1]);
				formatReset = 1;
				break;
				
			case 'o':
				formatUnsigned = va_arg( args, unsigned int );
				i = 0;
				if( formatUnsigned == 0 )
				{
					formatOutput[i] = (char)('0');
					i++;
				}
				for(i = 0; formatUnsigned; i++)
				{
					char mod = formatUnsigned % 8;
					formatOutput[i] = (char)('0'+mod);
					formatUnsigned = (formatUnsigned - mod) / 8;
				}
				for(int j = 0; j < (formatPadding - i); j++) 
				{
					ili9325PrintChar(formatFill);
				}
				for( ; i > 0; i--) ili9325PrintChar(formatOutput[i-1]);
				formatReset = 1;
				break;
				
			case 's':
				formatString = va_arg( args, char * );
				ili9325PrintString(formatString);
				formatReset = 1;
				break;
			
			case '_':
				formatFill='_';
				break;

			case '0':
				if(formatPadding == 0) formatFill='0';
			case '1' ... '9':
				formatPadding *= 10;
				formatPadding += ((*p) - '0');
				break;
				
			case '+':
				formatSign = 1;
				break;
				
			case '%':
				ili9325PrintChar('%');
				formatReset = 1;
				break;
			}
			
			if(formatReset)
			{
				//Restore everything
				format = 0;
				formatFill = ' ';
				formatSign = 0;
				formatPadding = 0;
				formatOutput[0] = '\0';
				formatReset = 0;
			}
		}
		else ili9325PrintChar(*p);
	}
	va_end(args);
}

#endif
