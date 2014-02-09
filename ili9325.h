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
 
#ifndef ILI9325_H
#define ILI9325_H

//---- Includes headers ------------------------------------------------------//

#include <libopencm3/stm32/f1/gpio.h>
#include "font_7seg.h"
#include "font6x12.h"
#include "font8x16.h"
#include "font8x16b.h"

//---- Definitions -----------------------------------------------------------//

#define C16_BLACK		0x0000
#define C16_WHITE		0xffff
#define C16_GRAY		0x39ee
#define C16_RED			0xf800
#define C16_DK_RED		0x3800
#define	C16_GREEN		0x07e0
#define C16_DK_GREEN	0x01e0
#define C16_BLUE		0x001f
#define C16_DK_BLUE		0x000e
#define	C16_YELLOW		0xffe0
#define	C16_DK_YELLOW	0x39e0
#define C16_CYAN		0x07ff
#define C16_DK_CYAN		0x01ee
#define C16_MAGENTA		0xf81f
#define C16_DK_MAGENTA	0x3807
#define C16_AMBER		0b1111110101100000

#define FONT_SOLID		0
#define FONT_GRADIENT	1

#define BACK_SOLID		0
#define BACK_IMAGE		1

#define ili9325Width	240
#define ili9325Height	320

#define ili9325CS(x)		(x)?gpio_set(GPIOC, GPIO8):gpio_clear(GPIOC, GPIO8)
#define ili9325RS(x)		(x)?gpio_set(GPIOC, GPIO9):gpio_clear(GPIOC, GPIO9)
#define ili9325WR(x)		(x)?gpio_set(GPIOC, GPIO10):gpio_clear(GPIOC, GPIO10)
#define ili9325RD(x)		(x)?gpio_set(GPIOC, GPIO11):gpio_clear(GPIOC, GPIO11)
#define ili9325Light(x) 	(x)?gpio_set(GPIOC, GPIO12):gpio_clear(GPIOC, GPIO12)
#define ili9325PortWrite(x)	gpio_clear(GPIOC, 0x00ff); gpio_clear(GPIOB, 0xff00); gpio_set(GPIOC, ((x) & 0x00ff)); gpio_set(GPIOB, ((x) & 0xff00)) //Write x to port
//#define ili9325PortWrite(x)	GPIO_BSRR(GPIOC) = (0x00ff & !x)<<16 | (0x00ff & x); GPIO_BSRR(GPIOB) = (0xff00 & !x)<<16 | (0xff00 & x)
#define ili9325WriteData(x)	ili9325PortWrite(x); ili9325WR(0); ili9325WR(1) //Write data to display

//---- Variables -------------------------------------------------------------//

//Store display model, if you want to know exactly what display you have.
uint16_t _ili9325Model = 0;

//Store rotation information for ili9325GoTo. Set this from ili9325Orientation
uint8_t _ili9325Rotation = 0; //Don't set it by hand, use ili9325_orientation()

//Store font settings. Set these from ili9325SetFont()
const uint8_t (*_ili9325FontWidth) = &font6x12Width;
const uint8_t (*_ili9325FontHeight) = &font6x12Height;
const uint8_t (*_ili9325FontSpace) = &font6x12Space;
const uint8_t (*_ili9325FontData)[] = &font6x12Data;

//Store colors for certaing functions. Set these from ili9325SetColor();
uint16_t _ili9325ColorFront = C16_WHITE;
uint16_t _ili9325ColorBack = C16_BLACK;
uint16_t _ili9325ColorFill = C16_GRAY;

//Location for next charecter
uint16_t _ili9325LocationX = 0;
uint16_t _ili9325LocationY = 0;

//Text Line Break X Offset
uint16_t _ili9325TextXOffset = 0;

//Text mode and backround mode fordifferent visual effects
uint8_t _ili9325FontMode = FONT_SOLID;
uint8_t _ili9325BackMode = BACK_SOLID;

//Background Image address
const uint16_t *_ili9325BackWidth;
const uint16_t *_ili9325BackHeight;
const uint16_t *_ili9325BackColors;
const uint8_t *_ili9325BackData;

//Logical resolution
uint16_t _ili9325ResX = 0;
uint16_t _ili9325ResY = 0;

//---- Function prototypes ---------------------------------------------------//

void		ili9325PortDirection(bool input);		
uint16_t	ili9325PortRead(void);
void		ili9325WriteCommand(uint16_t command);
void		ili9325WriteRegister(uint16_t reg, uint16_t data);
uint16_t	ili9325ReadReg(uint16_t reg);
uint16_t	ili9325ReadData(void);
void		ili9325Init(void);
void		ili9325GoTo(uint16_t x, uint16_t y);
void		ili9325Point(uint16_t color);
void		ili9325Orientation(uint8_t rot);
void		ili9325PrintDigit(uint8_t digit, uint16_t x, uint16_t y);
void		ili9325PrintChar(char character);
void		ili9325PrintString(char chars[]);
void		ili9325SetFont(uint8_t font);
void		ili9325SetLocation(uint16_t x, uint16_t y);
void		ili9325ColorSet(uint16_t front, uint16_t back, uint16_t fill);
void		ili9325Image(const uint16_t (*width), const uint16_t (*height), const uint16_t (*colors)[], const uint8_t (*data)[], uint16_t destX, uint16_t destY);
void		ili9325Mask(const uint16_t (*width), const uint16_t (*height), const uint8_t (*data)[], uint16_t destX, uint16_t destY);
void		ili9325BackImage(const uint16_t *width, const uint16_t *height, const uint16_t *colors, const uint8_t *data);
void		ili9325BackMode(uint8_t mode);
void		ili9325Clear(void);
void		ili9325ClearArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void		ili9325ClearLine(void);
void		ili9325printf( char * fmt, ... );
void 		ili9325Rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool fill, uint16_t fillColor, uint16_t edgeColor, uint16_t edgeWidth);

//---- Include source --------------------------------------------------------//

#include "ili9325.c"

#endif
