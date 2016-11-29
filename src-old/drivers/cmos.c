#include <system.h>
#include <kernlib.h>
#include "cmos.h"

unsigned char CMOS_GetValue(short key)
{
	disable_interrupts();
	outportb(0x70, key);
	unsigned char ret = inportb(0x71);
	enable_interrupts();
	return ret;
}

int CMOS_GetSeconds()
{
	return unbcd((int)CMOS_GetValue(RTC_SECONDS));
}

int CMOS_GetMinutes()
{
	return unbcd((int)CMOS_GetValue(RTC_MINUTES));
}

int CMOS_GetHours()
{
	return unbcd((int)CMOS_GetValue(RTC_HOURS));
}

int CMOS_GetDayOfWeek()
{
	return unbcd((int)CMOS_GetValue(RTC_DAYOFWEEK));
}

int CMOS_GetDateDay()
{
	return unbcd((int)CMOS_GetValue(RTC_DATEDAY));
}

int CMOS_GetDateMonth()
{
	return unbcd((int)CMOS_GetValue(RTC_DATEMONTH));
}

int CMOS_GetDateYear()
{
	return unbcd((int)CMOS_GetValue(RTC_DATEYEAR));
}
