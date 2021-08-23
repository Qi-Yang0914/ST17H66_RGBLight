/**
 *	@file		LC_RTC8563.h
 *	@author		YQ
 *	@date		12/07/2020
 *	@version	1.0.0
 */
/*!
 *	@defgroup	LC_RTC8563
 *	@brief
 *	@{*/

#ifndef		_LC_RTC8563_H
#define		_LC_RTC8563_H
/*------------------------------------------------------------------*/
/*						C++ guard macro								*/
/*------------------------------------------------------------------*/
#ifdef	__cplusplus
	extern "C"	{
#endif
/*------------------------------------------------------------------*/
/*						head files include 							*/
/*------------------------------------------------------------------*/
#include "LC_Common.h"
/*------------------------------------------------------------------*/
/*						Pins definitions							*/
/*------------------------------------------------------------------*/

/*------------------------------------------------------------------*/
/*						MACROS										*/
/*------------------------------------------------------------------*/
#define	LC_DEV_ALARM_NUM_MAX		(1*2)
#define	LC_H66_SYSUTC_DIFF_2000		946656000	//	2000-01-01 00:00:00
#define	LC_RGB_DEFAULT_UNIX			1514779200	//	2018-01-01 12:00:00	monday

/*------------------------------------------------------------------*/
/*						UI Task Events definitions					*/
/*------------------------------------------------------------------*/

/*------------------------------------------------------------------*/
/*						Data structures								*/
/*------------------------------------------------------------------*/
typedef struct
{
	u8 Enable; //闹钟使能(bit0:Minute,bit1:hour,bit2:day,bit3:weekday)
	u8 State;  //闹钟状态(0:停止,1:启动(无中断),2:启动(有中断))
	u8 Func;   //闹钟功能(0:turn on,1:turn off)
	u8 Second; //秒(0--59)
	u8 Minute; //分(0--59)
	u8 Hour;   //时(0--23)
	u8 Day;    //日(1--31)
	u8 WeekDay;//周(0--6)(bit7:enable   bit0--bit6:周一--周日)
	u8 Month;  //月(1--12)
	u8 Year;   //年(0--99)
}LC_RTC8563_Alarm_Struct;//RTC8563闹钟结构体

typedef struct {
	u32	dev_utc_time;	//对应UTC时间戳
	u32	dev_year;		//当前时间“年”
	u8	dev_month;		//当前时间“月”
	u8	dev_day;		//当前时间“日”
	u8	dev_week;		//当前时间“周”：7代表周日，1--6代表周一到周六
	u8	dev_week_old;	//用于记录UTC转换的星期变化，变化后APP星期加1
	u8	dev_week_app;	//app设置的星期基础上起始：1-7（周一--周日）
				
	u8	dev_hour;		//当前时间“时”
	u8	dev_minute;		//当前时间“分”
	u8	dev_second;		//当前时间“秒”
}LC_RTC8563_RTCTime ;

/*------------------------------------------------------------------*/
/*						external variables							*/
/*------------------------------------------------------------------*/
extern	LC_RTC8563_Alarm_Struct	LC_Dev_RTC8563_Alarm[LC_DEV_ALARM_NUM_MAX];
extern	LC_RTC8563_RTCTime		LC_Dev_RTC8563_DateTime;
extern	LC_RTC8563_RTCTime		LC_Dev_Read_DateTime;
/*------------------------------------------------------------------*/
/*						User function prototypes					*/
/*------------------------------------------------------------------*/
u8 		my_rtc_week_count                   (u8 Year_h, u8 Year_l, u8 Month, u8 Day	);//    《当前年月日算出周几》(Year_h:年高位（20）,Year_l:年低位(当前年-2000))
void 	my_rtc_localtime                    (u32 time,	LC_RTC8563_RTCTime *t		);//    《根据U32秒算出当前年月日时分秒》(time:U32秒，LC_RTC8563_RTCTime *t:年月日)
u32		my_rtc_mktime                       (LC_RTC8563_RTCTime dt					);//    《根据当前年月日时分秒算出U32秒》
void	LC_Get_SysRtcTime					(void									);
void	LC_Set_SysRtcTime					(uint32 utc_time						);
void	LC_RGBLight_Alarm_Process           (void									);

#ifdef	__cplusplus
	}
#endif

#endif		/**	LC_RTC8563.h **/
/**	@}*/

