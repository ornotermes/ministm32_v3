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

#ifndef ADS7843_H
#define ADS7843_H

#include <libopencm3/stm32/f1/rcc.h>
#include <libopencm3/stm32/f1/gpio.h>
#include <libopencm3/stm32/f1/nvic.h>
#include <libopencm3/stm32/f1/bkp.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/spi.h>

#include "target.h" //Target symbol for calibration

#define ads7843CS(x)		(x)?gpio_set(GPIOA, GPIO4):gpio_clear(GPIOA, GPIO4)
#define ads7843PEN()		gpio_get(GPIOC, BIT13)

#define ads7843WaitPress()		while(!ads7843Press) ads7843Task() //Wait for the touch screen to be pressed.
#define ads7843WaitRelease()	while(ads7843Press) ads7843Task() //Wait for the touch screen to be released.

#define _ads7843RegCalScaleX	BKP_DR10
#define _ads7843RegCalScaleY	BKP_DR9
#define _ads7843RegCalOffsetX	BKP_DR8
#define _ads7843RegCalOffsetY	BKP_DR7

unsigned int ads7843X = 0;
unsigned int ads7843Y = 0;
unsigned int _ads7843RawX = 0;
unsigned int _ads7843RawY = 0;
bool ads7843Press = 0;
bool ads7843Calibrated = 0;

uint16_t _ads7843CalScaleX = 0;
uint16_t _ads7843CalScaleY = 0;
uint16_t _ads7843CalOffsetX = 0;
uint16_t _ads7843CalOffsetY = 0;


void ads7843Setup(void);
uint16_t ads7843GetPos(bool y);
void ads7843Task(void);
void ads7843Calibrate(void);
void ads7843Wait(void);
void ads7843CalWrite(void);
bool ads7843CalRead(void);

#include "ads7843.c"

#endif
