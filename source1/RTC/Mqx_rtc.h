/*
 * Mqx_rtc.h
 *
 *  Created on: Jan 27, 2014
 *      Author: GADGEON PC
 */

#ifndef MQX_RTC_H_
#define MQX_RTC_H_

/* Local function prototypes */
extern void print_rtc_time_date();
extern void Update_rtc_from_gps(void);
extern void Get_timestamp_from_rtc(void);
extern void Rtc_init(void);
void Set_Time_Zone(int32_t );
extern void Update_rtc(char *, char *);
extern uint_16 Get_lock_time(void);
extern char update_rtc_from_gps_status;
extern void update_system_date_time_from_rtc(void);
extern void update_system_date_from_rtc(void);
extern void update_system_time_from_rtc(void);
extern void set_absolute_time(void);
#endif /* RTC_H_ */
