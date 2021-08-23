/**
 *	@file		LC_Common.h
 *	@author		YQ
 *	@date		10/20/2020
 *	@version	1.0.1
 *
 */

/*!
 * 	@defgroup	LC_Common
 *	@brief
 *	@{*/
#ifndef		LC_COMMON_H_
#define		LC_COMMON_H_
/*------------------------------------------------------------------*/
/*						C++ guard macro								*/
/*------------------------------------------------------------------*/
#ifdef	__cplusplus
	 extern  "C" {
#endif
/*------------------------------------------------------------------*/
/* 				 head files include 							 	*/
/*------------------------------------------------------------------*/

#include "att.h"
#include "bcomdef.h"
#include "gapbondmgr.h"
#include "gapgattserver.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "global_config.h"
#include "hci.h"
#include "hci_tl.h"
#include "linkdb.h"
#include "ll.h"
#include "ll_def.h"
#include "ll_hw_drv.h"
#include "OSAL.h"
#include "OSAL_PwrMgr.h"
#include "OSAL_Clock.h"
#include "ota_app_service.h"
#include "peripheral.h"
#include "pwm.h"
#include "pwrmgr.h"
#include "rf_phy_driver.h"
#include "simpleBLEPeripheral.h"
#include "sbpProfile_ota.h"
#include "adc.h"
#include "voice.h"
#include "osal_snv.h"
#include "LC_Event_Handler.h"
/*------------------------------------------------------------------*/
/*						Pins definitions							*/
/*------------------------------------------------------------------*/


/*------------------------------------------------------------------*/
/*						MACROS										*/
/*------------------------------------------------------------------*/
//	module option
#define		LC_RGBLight_Key_Enable			1		//	1:Key module enable,0:key module disable.
#define		LC_RGBLight_IR_Enable			1		//	1:IR  module enable,0:IR  module disable.
#define		LC_RGBLight_Mic_Enable			1		//	1:Mic module enable,0:Mic module disable.

//	Light LED Module option
#define		RGBLight						1
#define		RGBWLight						2

#ifndef		LC_RGBLight_Module
#define		LC_RGBLight_Module				RGBLight
#endif
/*------------------------------------------------------------------*/
/*						UI Task Events definitions					*/
/*------------------------------------------------------------------*/

/*------------------------------------------------------------------*/
/* 					 	Data structures							 	*/
/*------------------------------------------------------------------*/
typedef uint8_t				u8;
typedef uint16_t			u16;
typedef uint32_t			u32;
typedef signed   char		int8_t;         //!< Signed 8 bit integer
typedef unsigned char		uint8_t;		//!< Unsigned 8 bit integer
typedef signed   short		int16_t;		//!< Signed 16 bit integer
typedef unsigned short		uint16_t;		//!< Unsigned 16 bit integer
typedef signed   int		int32_t;		//!< Signed 32 bit integer
typedef unsigned int		uint32_t;		//!< Unsigned 32 bit integer
typedef signed   char		int8;			//!< Signed 8 bit integer
typedef unsigned char		uint8;			//!< Unsigned 8 bit integer
typedef signed   short		int16;			//!< Signed 16 bit integer
typedef unsigned short		uint16;         //!< Unsigned 16 bit integer
typedef signed   long		int32;			//!< Signed 32 bit integer
typedef unsigned long		uint32;         //!< Unsigned 32 bit integer


typedef struct
{
	 uint32 		 dev_timeout_poweroff_cnt;
	 uint8			 dev_poweron_switch_flag;
	 uint8			 dev_power_flag;
	 uint8			 dev_lowpower_flag;
	 uint8			 dev_ble_con_state;
	 uint8			 dev_batt_value;
}lc_dev_sys_param;

typedef struct
{
	uint8		app_write_data[LC_RXD_VALUE_LEN];	//	datas from APP
	uint8		app_notofy_data[LC_RXD_VALUE_LEN];	//	device notify
	uint8		app_write_len;
	uint8		app_notify_len;
}lc_app_set_t;

typedef	struct
{
	uint16		RGB_rValue;					//	value write in register = value_new*light_level/10(0-2550)
	uint16		RGB_gValue;
	uint16		RGB_bValue;
	uint16		RGB_wValue;
	uint16		RGB_rValue_New;				//	renew color value(0-255)from app & ir keyboard		
	uint16		RGB_gValue_New;
	uint16		RGB_bValue_New;
	uint16		RGB_wValue_New;
	uint16		RGB_Speed_Reserved;			//	original value of speed:0 - 100
	uint16		RGB_Mode_Change_Speed;		//	speed of mode
	uint8		RGB_Mode_Change_Color_Num;	//	number of color in dynamic mode
	uint8		RGB_Mode_Fade_Color_Num;	//	sequence of color in fade mode
	uint8		RGB_Mode_Flash_Time_Num;	//	flash times of one mode
	uint8		RGB_Light_State;			//	on	1,off	0
	uint8		RGB_Light_Color_Sequence;	//	default	0		1		2		3		4		5
											//		(R G B)	(R B G)	(G R B)	(G B R)	(B R G)	(B G R)
	uint8		RGB_Light_Level;			//	level of lightness:0--100
	uint8		RGB_Light_Mode;				//	static mode,	dynamic mode
	uint8		RGB_Light_Mode_Reserved;	//	reserved mode before turn off
	uint8		RGB_Light_Mode_Auto;		//	on	1,off	0
	uint8		RGB_Mic_Mode;				//	01:classical;02:soft;03:beats;04:disco
}lc_rgblight_t;

typedef	enum{
	State_Off	=	0,
	State_On,
}bit_dev_state_e;

/*------------------------------------------------------------------*/
/* 					 external variables							 	*/
/*------------------------------------------------------------------*/
extern		lc_dev_sys_param		LC_Dev_System_Param;
extern		lc_app_set_t			LC_App_Set_Param;
extern		lc_rgblight_t			LC_RGBLight_Param;
/*------------------------------------------------------------------*/
/* 					 User function prototypes					 	*/
/*------------------------------------------------------------------*/
uint32		clock_time_exceed_func 			(uint32 ref, uint32 span_ms		);
uint8		halfbyte_into_str				(uint8 byte						);
void 		Printf_Hex 						(void* data, uint16 len			);
void	 	LC_Common_ProcessOSALMsg	 	(osal_event_hdr_t *pMsg			);
void 		LC_Timer_Start					(void);
void 		LC_Timer_Stop					(void);
void 		LC_App_Push_Data				(uint8 channel,uint8 len,uint8 *push_data);


#ifdef	__cplusplus
}
#endif

#endif	/* LC_COMMON_H_ */
/** @}*/
