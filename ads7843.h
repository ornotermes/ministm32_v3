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
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/spi.h>

#define ads7843CS(x)		(x)?gpio_set(GPIOA, GPIO4):gpio_clear(GPIOA, GPIO4)
#define ads7843PEN()		gpio_get(GPIOC, BIT13)

void ads7843_setup(void);

#include "ads7843.c"

#endif
