#ifndef CLOCK_C
#define CLOCK_C

void ClockShow(void)
{
	uint16_t tempx = _ili9325LocationX, tempy = _ili9325LocationY;
	ili9325SetLocation(207, 228);
	time_t now = rtc_get_counter_val();
	struct tm * Time = localtime(&now);
	ili9325printf("%i-%02i-%02i %02i:%02i", 1900 + Time->tm_year, 1+Time->tm_mon, Time->tm_mday, Time->tm_hour, Time->tm_min);
	ili9325SetLocation(tempx, tempy);
}

void ClockSet(void)
{
	ClockExit = false;
	ili9325Clear();
	ili9325SetFont(9);
	time_t now = rtc_get_counter_val();
	TimeSet = localtime(&now);
	TimeSet->tm_sec = 0;
	uint8_t count = 12;
	struct ili9325Button Button[count];
		Button[0].x = 16; Button[0].y = 48; Button[0].width = 32; Button[0].height = 32; Button[0].text = "+"; Button[0].callback = BtnClockYearUp;
		Button[1].x = 80; Button[1].y = 48; Button[1].width = 32; Button[1].height = 32; Button[1].text = "+"; Button[1].callback = BtnClockMonthUp;
		Button[2].x = 144; Button[2].y = 48; Button[2].width = 32; Button[2].height = 32; Button[2].text = "+"; Button[2].callback = BtnClockDayUp;
		Button[3].x = 208; Button[3].y = 48; Button[3].width = 32; Button[3].height = 32; Button[3].text = "+"; Button[3].callback = BtnClockHourUp;
		Button[4].x = 272; Button[4].y = 48; Button[4].width = 32; Button[4].height = 32; Button[4].text = "+"; Button[4].callback = BtnClockMinuteUp;
		
		Button[5].x = 16; Button[5].y = 128; Button[5].width = 32; Button[5].height = 32; Button[5].text = "-"; Button[5].callback = BtnClockYearDown;
		Button[6].x = 80; Button[6].y = 128; Button[6].width = 32; Button[6].height = 32; Button[6].text = "-"; Button[6].callback = BtnClockMonthDown;
		Button[7].x = 144; Button[7].y = 128; Button[7].width = 32; Button[7].height = 32; Button[7].text = "-"; Button[7].callback = BtnClockDayDown;
		Button[8].x = 208; Button[8].y = 128; Button[8].width = 32; Button[8].height = 32; Button[8].text = "-"; Button[8].callback = BtnClockHourDown;
		Button[9].x = 272; Button[9].y = 128; Button[9].width = 32; Button[9].height = 32; Button[9].text = "-"; Button[9].callback = BtnClockMinuteDown;
		
		Button[10].x = 16; Button[10].y = 172; Button[10].width = 96; Button[10].height = 32; Button[10].text = "Cancel"; Button[10].callback = BtnClockCancel;
		Button[11].x = 208; Button[11].y = 172; Button[11].width = 96; Button[11].height = 32; Button[11].text = "OK"; Button[11].callback = BtnClockOK;
	ili9325ButtonLayout(Button, count);	
	ili9325SetLocation(0, 100);
	ili9325printf(" %i  -  %02i  -  %02i     %02i  :  %02i\n", 1900 + TimeSet->tm_year, 1+TimeSet->tm_mon, TimeSet->tm_mday, TimeSet->tm_hour, TimeSet->tm_min);
	while(!ClockExit)
	{
		ads7843Task();
		if(ads7843Press)
		{
			ili9325ButtonScan(Button, count, ads7843X, ads7843Y);
			ili9325SetLocation(0, 100);
			ili9325printf(" %i  -  %02i  -  %02i     %02i  :  %02i\n", 1900 + TimeSet->tm_year, 1+TimeSet->tm_mon, TimeSet->tm_mday, TimeSet->tm_hour, TimeSet->tm_min);
			while(ads7843Press)
			{
				ads7843Task();
			}
		}
	}
	ili9325Clear();
	ili9325SetFont(6);
}

void BtnClockOK(void)
{	
	time_t new = mktime(TimeSet);
	if(new == -1)
	{
		ili9325SetLocation(0, 0);
		ili9325printf("Error!\n");
	}
	else
	{
		rtc_set_counter_val(new);
		ClockExit = true;
	}
}

void BtnClockCancel(void)
{
	ClockExit = true;
}

void BtnClockMinuteDown(void)
{
	TimeSet->tm_min--;
}

void BtnClockMinuteUp(void)
{
	TimeSet->tm_min++;
}

void BtnClockHourDown(void)
{
	TimeSet->tm_hour--;
}

void BtnClockHourUp(void)
{
	TimeSet->tm_hour++;
}

void BtnClockDayDown(void)
{
	TimeSet->tm_mday--;
}

void BtnClockDayUp(void)
{
	TimeSet->tm_mday++;
}

void BtnClockMonthDown(void)
{
	TimeSet->tm_mon--;
}

void BtnClockMonthUp(void)
{
	TimeSet->tm_mon++;
}

void BtnClockYearDown(void)
{
	TimeSet->tm_year--;
}

void BtnClockYearUp(void)
{
	TimeSet->tm_year++;
}

#endif
