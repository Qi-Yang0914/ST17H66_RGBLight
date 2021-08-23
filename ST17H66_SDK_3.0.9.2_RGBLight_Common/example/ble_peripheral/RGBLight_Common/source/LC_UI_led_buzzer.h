/**
 *	@file		LC_UI_led_buzzer.h
 *	@author		YQ
 *	@date		09/16/2020
 *	@version	1.2.0
 *
 */

/*!
 * 	@defgroup	LC_UI_led_buzzer
 *	@brief
 *	@{*/

#ifndef	LC_UI_LED_BUZZER_H_
#define	LC_UI_LED_BUZZER_H_
/*------------------------------------------------------------------*/
/*						C++ guard macro								*/
/*------------------------------------------------------------------*/
#ifdef	__cplusplus
extern	"C"	{
#endif
/*------------------------------------------------------------------*/
/*						head files include 							*/
/*------------------------------------------------------------------*/
#include "LC_Common.h"
/*------------------------------------------------------------------*/
/*						Pins definitions							*/
/*------------------------------------------------------------------*/
#define		MY_GPIO_LED_R		P34
#define		MY_GPIO_LED_G		P2
#define		MY_GPIO_LED_B		P3

#if(LC_RGBLight_Module == RGBWLight)
#define		MY_GPIO_LED_WW		P11
#endif

/*------------------------------------------------------------------*/
/*						MACROS										*/
/*------------------------------------------------------------------*/
#define		RGB_COLOR_MAX		(255)
#define		RGB_PWM_MAX			(255)
#define		RGB_WHITE_MAX		(180)
#define		RGB_LEVEL_PECENT	(100)
#define		RGB_PWM_COLOR_MUL	(RGB_PWM_MAX/RGB_COLOR_MAX)

//	BaoLianDeng APP Mode defines
#define		APP_STATIC_RED		0x80
#define		APP_STATIC_WHITE	0x86

#define		APP_JUMP_THREE		0x87
#define		APP_JUMP_SEVEN		0x88

#define		APP_FADE_THREE		0x89
#define		APP_FADE_G_B		0x94

#define		APP_FLASH_SEVEN		0x95
#define		APP_FLASH_WHITE		0x9C
/*------------------------------------------------------------------*/
/*						UI Task Events definitions					*/
/*------------------------------------------------------------------*/
#define		UI_EVENT_LEVEL1		0x0001
#define		UI_EVENT_LEVEL2		0x0002
#define		UI_EVENT_LEVEL3		0x0004
#define		UI_EVENT_LEVEL4		0x0008
#define		UI_EVENT_LEVEL5		0x0010
/*------------------------------------------------------------------*/
/*						Data structures								*/
/*------------------------------------------------------------------*/

/*------------------------------------------------------------------*/
/*						external variables							*/
/*------------------------------------------------------------------*/
extern uint8	LC_Ui_Led_Buzzer_TaskID;
/*------------------------------------------------------------------*/
/*						User function prototypes					*/
/*------------------------------------------------------------------*/
void	LC_RGBLight_Reserve_Mode		(void			);
void	LC_RGBLight_Reserve_Alarm		(void			);
void 	LC_UI_Led_Buzzer_Task_Init		(uint8 task_id	);
uint16	LC_UI_Led_Buzzer_ProcessEvent	(uint8 task_id, uint16 events);


#ifdef	__cplusplus
}
#endif

#endif	/*	LC_UI_LED_BUZZER_H_	*/
/**	@}*/
