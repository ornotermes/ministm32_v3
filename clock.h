#ifndef CLOCK_H
#define CLOCK_H

//---- Includes headers ------------------------------------------------------//

#include <libopencm3/stm32/f1/rtc.h>
#include <time.h>

//---- Definitions -----------------------------------------------------------//

//---- Structs ---------------------------------------------------------------//

//---- Variables -------------------------------------------------------------//

struct tm * TimeSet;
uint8_t ClockExit = 0;

//---- Function prototypes ---------------------------------------------------//

void ClockShow(void);
void ClockSet(void);
void BtnClockOK(void);
void BtnClockCancel(void);
void BtnClockMinuteDown(void);
void BtnClockMinuteUp(void);
void BtnClockHourDown(void);
void BtnClockHourUp(void);
void BtnClockDayDown(void);
void BtnClockDayUp(void);
void BtnClockMonthDown(void);
void BtnClockMonthUp(void);
void BtnClockYearDown(void);
void BtnClockYearUp(void);

//---- Include source --------------------------------------------------------//

#include "clock.c"

#endif
