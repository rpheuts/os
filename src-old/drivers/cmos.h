#ifndef CMOS_H_
#define CMOS_H_

#define RTC_SECONDS	0x00
#define RTC_MINUTES	0x02
#define RTC_HOURS		0x04
#define RTC_DAYOFWEEK	0x06
#define RTC_DATEDAY	0x07
#define RTC_DATEMONTH	0x08
#define RTC_DATEYEAR	0x09

extern int CMOS_GetSeconds();
extern int CMOS_GetMinutes();
extern int CMOS_GetHours();
extern int CMOS_GetDayOfWeek();
extern int CMOS_GetDateDay();
extern int CMOS_GetDateMonth();
extern int CMOS_GetDateYear();

#endif
