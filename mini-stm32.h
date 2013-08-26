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

#ifndef MINI_STM32_H
#define MINI_STM32_H

#define LED1TOGGLE()
#define LED1ON()
#define LED1OFF()

bool button1 = 0;
bool button2 = 0;
unsigned int button1Count = 0;
unsigned int button2Count = 0;

void clockSetup(void);
void gpioSetup(void);
void serialSetup(void);
void buttonTask(void);
int main(void);

#endif
