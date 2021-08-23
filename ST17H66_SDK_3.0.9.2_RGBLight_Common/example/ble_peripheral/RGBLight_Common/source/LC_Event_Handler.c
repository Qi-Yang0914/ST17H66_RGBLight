/**
 *	@file		LC_Event_Handler.c
 *	@author		YQ
 *	@date		01/20/2021
 *	@brief		IRQ handler.
 */

/*!
 *	@defgroup	LC_Event_Handler
 *	@brief		DO NOT put Interrupt code in XIP flash!!!
 *	@{*/

/*------------------------------------------------------------------*/
/*						head files include 							*/
/*------------------------------------------------------------------*/
#include	"LC_Event_Handler.h"
#include	"LC_UI_led_buzzer.h"
#include	"LC_IR_Rec.h"
#include	"LC_Key.h"
#include	"LC_RGBLight_Mode.h"
/*------------------------------------------------------------------*/
/* 					 	local variables			 					*/
/*------------------------------------------------------------------*/

/*------------------------------------------------------------------*/
/* 					 	public variables		 					*/
/*------------------------------------------------------------------*/

/*------------------------------------------------------------------*/
/* 					 	public functions		 					*/
/*------------------------------------------------------------------*/
/**
 *	@fn			LC_RGB_Valeu_Deal
 *	@brief		Callback of timer handler.
 *	@param[in]	evt		:IRQ event.
 *	@return		none.
 */
void	__ATTR_SECTION_SRAM__  __attribute__((used))	LC_RGB_Valeu_Deal(uint8 evt)
{
	if(evt == HAL_EVT_TIMER_6){
		if(LC_RGBLight_Param.RGB_Light_Mode > RGB_Static_White){
			LC_RGBLight_Dynamic_Mode_Process();
		}
		osal_set_event(LC_Ui_Led_Buzzer_TaskID, UI_EVENT_LEVEL1);
	}else if(evt == HAL_EVT_TIMER_5){
		LC_IR_Analysis_100ns_Cnt++;
		LC_RGBLight_Dynamic_Basic_Timer();
	}
}
/**
 *	@fn			LC_IR_Analysis_Data
 *	@brief		analysis NEC,get user code and commond code.
 *	@param[in]	nTimeL		:interval of a negtivate and a posetive edge.	
 *	@return		none.
 */
void	__ATTR_SECTION_SRAM__  __attribute__((used))	LC_IR_Analysis_Data(uint32	nTimeL)
{
	static	uint16	bit_cnt	=	0;

	if((nTimeL > 25) && (nTimeL < 50)){
		bit_cnt	=	0;
		LC_IR_Analysis_KeyValue	=	0xFFFFFFFF;
	}else{
		if(nTimeL < 19){
			if(nTimeL > 10){
				LC_IR_Analysis_KeyValue &= ~ BIT(bit_cnt);
			}
			if(bit_cnt < 32){
				bit_cnt++;
			}
			if(bit_cnt == 32){
				// osal_start_timerEx(LC_IR_Rec_TaskID, IR_REC_EVENT_LEVEL2, 10);
				osal_set_event(LC_IR_Rec_TaskID, IR_REC_EVENT_LEVEL2);
				bit_cnt	=	0;
			}
		}
	}
}
/*!
 *	@fn			LC_Key_Pin_IntHandler
 *	@brief		Callback of key Pin interrupt.
 *	@param[in]	pin		:pin of IR.
 *	@param[in]	type	:type of interrupe.
 *	@return		none.
 */
void	__ATTR_SECTION_SRAM__  __attribute__((used))	LC_Key_Pin_IntHandler(GPIO_Pin_e pin, IO_Wakeup_Pol_e type)
{
	switch(pin){
		case	MY_KEY_NO1_GPIO:
			if(type == NEGEDGE){
				LC_Key_Param.key_down_flag	=	1;
			}else{
				LC_Key_Param.key_down_flag	=	0;
			}
			osal_start_timerEx(LC_Key_TaskID, KEY_EVENT_LEVEL1, 20);
		break;

		case	MY_KEY_NO2_GPIO:
			if(type == NEGEDGE){
				LC_Key_Param.key_down_flag	=	2;
			}else{
				LC_Key_Param.key_down_flag	=	0;
			}
			osal_start_timerEx(LC_Key_TaskID, KEY_EVENT_LEVEL1, 20);
		break;

		case	MY_KEY_NO3_GPIO:
			if(type == NEGEDGE){
				LC_Key_Param.key_down_flag	=	3;
			}else{
				LC_Key_Param.key_down_flag	=	0;
			}
			osal_start_timerEx(LC_Key_TaskID, KEY_EVENT_LEVEL1, 20);
		break;
		default:
			
		break;
	}
}
/*!
 *	@fn			LC_Gpio_IR_IntHandler
 *	@brief		Callback of IR Pin interrupt.
 *	@param[in]	pin		:pin of IR.
 *	@param[in]	type	:type of interrupe.
 *	@return		none.
 */
void	__ATTR_SECTION_SRAM__  __attribute__((used))	LC_Gpio_IR_IntHandler(GPIO_Pin_e pin, IO_Wakeup_Pol_e type)
{
	switch(pin){
		case	MY_GPIO_IR_REC:
			if(type == NEGEDGE){
				// LC_IR_Analysis_Data(LC_IR_Analysis_100ns_Cnt - LC_IR_Last_Posedge_Time);
				osal_set_event(LC_IR_Rec_TaskID, IR_REC_EVENT_LEVEL1);
			}else{
				LC_IR_Last_Posedge_Time	=	LC_IR_Analysis_100ns_Cnt;
			}
		break;

		default:
		
		break;
	}
}

/** @}*/

