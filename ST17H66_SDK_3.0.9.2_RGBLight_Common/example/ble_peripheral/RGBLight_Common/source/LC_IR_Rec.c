/**
 *	@file		LC_IR_Rec.c
 *	@author		YQ
 *	@date		12/23/2020
 *	@version	1.1.2
 */

/*!
 *	@defgroup	LC_IR_Rec
 *	@brief
 *	@{*/
/*------------------------------------------------------------------*/
/* 				  head files include							 	*/
/*------------------------------------------------------------------*/
#include "LC_IR_Rec.h"
#include "LC_RGBLight_Mode.h"
#include "LC_RTC8563.h"
#include "LC_Mic_Sample.h"
#include "LC_UI_Led_Buzzer.h"
/*------------------------------------------------------------------*/
/* 					 	local variables		 						*/
/*------------------------------------------------------------------*/


/*------------------------------------------------------------------*/
/* 					 	public variables		 					*/
/*------------------------------------------------------------------*/
const		uint8	LC_RGBLight_Mode_IR_Buffer[96]	=	{
	//	normal 24keys colors
	255,  0,  0,	  0,255,  0,	  0,  0,255,	180,180,180,
	255, 28,  0,	  0,115, 28,	 85,  0,227,	255,193,193,  
	255, 57,  0,	  0,115, 54,	140,  0,227,	255,193,193,
	255,143,  0,	  0,115,143,	196,  0,227,	135,206,255,
	255,227,  0,	  0,115,227,	255,  0,227,	135,206,255,

	//	music 20keys addition colors
	//	orange			yellow			cyan			purple
	255,165,  0,	255,255,  0,	  0,255,255,	160, 32,240,

	//	normal 44keys diy colors
	240,240,240,	200,200,200,	180,180,180,	150,150,150,
	100,100,100,	 80, 80, 80,
};
volatile	uint32		LC_IR_Analysis_100ns_Cnt	=	0;
volatile	uint32		LC_IR_Analysis_KeyValue		=	0;
volatile	uint32		LC_IR_Last_Posedge_Time		=	0;
uint8		LC_IR_Rec_TaskID;

/*------------------------------------------------------------------*/
/* 					 	public functions		 					*/
/*------------------------------------------------------------------*/

/*!
 *	@fn			LC_Gpio_IR_Rec_Init
 *	@brief		Initialize the IR receiver pin.
 *	@param[in]	none.	
 *	@return		none.
 */
void	LC_Gpio_IR_Rec_Init(void)
{
//	hal_gpio_fmux(MY_GPIO_IR_REC, Bit_DISABLE);
//	hal_gpio_cfg_analog_io(MY_GPIO_IR_REC, Bit_DISABLE);
	hal_gpio_pin_init(MY_GPIO_IR_REC, IE);
	hal_gpio_pull_set(MY_GPIO_IR_REC, STRONG_PULL_UP);

	hal_gpioin_register(MY_GPIO_IR_REC, LC_Gpio_IR_IntHandler, LC_Gpio_IR_IntHandler);
}
/*!
 *	@fn			LC_RGBLight_Mode_Static_IRKeyboard
 *	@brief		IR Keyboard color change. 
 *	@param[in]	key_num				:key value of keyboard color.
 *	@param[in]	rgb_data			:pointer of color.
 *	@param[in]	temp				:.
 *	@return		none.
 */
void	LC_RGBLight_Mode_Static_IRKeyboard(uint8 key_num, const uint8 *rgb_data)
{
	LC_RGBLight_Param.RGB_Light_Mode_Auto	=	State_Off;
	LC_RGBLight_Param.RGB_Light_Mode	=	RGB_Plate_Mode;
	LC_RGBLight_Param.RGB_rValue_New	=	(*(rgb_data + key_num*3));
	LC_RGBLight_Param.RGB_gValue_New	=	(*(rgb_data + key_num*3 + 1));
	LC_RGBLight_Param.RGB_bValue_New	=	(*(rgb_data + key_num*3 + 2));
#if(LC_RGBLight_Module == RGBWLight)
	LC_RGBLight_Param.RGB_wValue_New	=	0;
	LC_RGBLight_Param.RGB_wValue		=	0;
#endif
	LC_RGBLight_Param.RGB_rValue		=	LC_RGBLight_Param.RGB_rValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
	LC_RGBLight_Param.RGB_gValue		=	LC_RGBLight_Param.RGB_gValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
	LC_RGBLight_Param.RGB_bValue		=	LC_RGBLight_Param.RGB_bValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
	
}
/*!
 *	@fn			LC_IR_Rec_Task_Init 
 *	@brief		Initialize function for the LC_IR_Rec Task. 
 *	@param[in]	task_id			:the ID assigned by OSAL,
 *								used to send message and set timer.
 *	@retrurn	none.
 */
void	LC_IR_Rec_Task_Init(uint8 task_id)
{
	LC_IR_Rec_TaskID	=	task_id;
}
/*!
 *	@fn			LC_IR_Rec_ProcessEvent
 *	@brief		LC_IR_Rec Task event processor.This function
 *				is called to processs all events for the task.Events
 *				include timers,messages and any other user defined events.
 *	@param[in]	task_id			:The OSAL assigned task ID.
 *	@param[in]	events			:events to process.This is a bit map and can
 *									contain more than one event.
 */
uint16	LC_IR_Rec_ProcessEvent(uint8 task_id, uint16 events)
{
	VOID task_id;	// OSAL required parameter that isn't used in this function
	if(events & SYS_EVENT_MSG){
		uint8	*pMsg;
		if((pMsg = osal_msg_receive(LC_IR_Rec_TaskID)) != NULL){
			LC_Common_ProcessOSALMsg((osal_event_hdr_t *)pMsg);
            // Release the OSAL message
			VOID osal_msg_deallocate(pMsg);
		}
		return(events ^ SYS_EVENT_MSG);
	}

	if(events & IR_REC_EVENT_LEVEL2){
		uint8	LC_IR_Keyboard_Num		=	0; 
		uint16	LC_IR_Keyboard_UserNum	=	0;
		
		LC_IR_Keyboard_Num		=	(uint8)((LC_IR_Analysis_KeyValue >> 24) & 0x000000ff);
		LC_IR_Keyboard_UserNum	=	(uint16)(LC_IR_Analysis_KeyValue & 0x0000ffff);
		LC_IR_Analysis_KeyValue =	0;
		LOG("ir analysis key %8x %8x\n",LC_IR_Keyboard_UserNum, LC_IR_Keyboard_Num);
		if(LC_IR_Keyboard_UserNum == IR_KeyBoard_Type_24Keys){
			// RGBLight Off
			if((LC_RGBLight_Param.RGB_Light_State	== State_On) && (LC_IR_Keyboard_Num == IRKey_Light_Off)){
				LC_RGBLight_Turn_Onoff(State_Off);
			}
			//	RGBLight On
			else if((LC_RGBLight_Param.RGB_Light_State == State_Off) && (LC_IR_Keyboard_Num == IRKey_Light_On)){
				LC_RGBLight_Turn_Onoff(State_On);
			}
			
			if(LC_RGBLight_Param.RGB_Light_State == State_On){
				if(LC_RGBLight_Param.RGB_Light_Mode == RGB_Plate_Mode){
					//	RGBLight Level Up
					if(LC_IR_Keyboard_Num == IRKey_Light_Level_Up){
						if(LC_RGBLight_Param.RGB_Light_Level < 100){
							LC_RGBLight_Param.RGB_Light_Level	+=	10;
						}else{
							LC_RGBLight_Param.RGB_Light_Level	=	100;
						}
						LC_RGBLight_Param.RGB_rValue		=	LC_RGBLight_Param.RGB_rValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
						LC_RGBLight_Param.RGB_gValue		=	LC_RGBLight_Param.RGB_gValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
						LC_RGBLight_Param.RGB_bValue		=	LC_RGBLight_Param.RGB_bValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
					#if(LC_RGBLight_Module == RGBWLight)
						LC_RGBLight_Param.RGB_wValue		=	0;
					#endif
					}
					//	RGBLight Level Down
					else if(LC_IR_Keyboard_Num == IRKey_Light_Level_Down){
						if(LC_RGBLight_Param.RGB_Light_Level > 20){
							LC_RGBLight_Param.RGB_Light_Level	-=	10;
						}else{
							LC_RGBLight_Param.RGB_Light_Level	=	10;
						}
						LC_RGBLight_Param.RGB_rValue		=	LC_RGBLight_Param.RGB_rValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
						LC_RGBLight_Param.RGB_gValue		=	LC_RGBLight_Param.RGB_gValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
						LC_RGBLight_Param.RGB_bValue		=	LC_RGBLight_Param.RGB_bValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
					#if(LC_RGBLight_Module == RGBWLight)
						LC_RGBLight_Param.RGB_wValue		=	0;
					#endif
					}
				}
				//	RGBLight Color Choose
				if((LC_IR_Keyboard_Num >= IRKey_Light_Red) && (LC_IR_Keyboard_Num <= IRKey_Mode_Smooth)){
					LC_Mic_Sample_Stop();
					if(LC_IR_Keyboard_Num == IRKey_Mode_Flash){
						LC_RGBLight_Modetick	=	0;
						LC_RGBLight_Param.RGB_Mode_Change_Color_Num	=	0;
						LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
						LC_RGBLight_Param.RGB_Light_Mode			=	RGB_Flash_SevenColors;
						LC_RGBLight_Param.RGB_Speed_Reserved		=	20;
						LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					}else if(LC_IR_Keyboard_Num	== IRKey_Mode_Strobe){
						LC_RGBLight_Modetick	=	0;
						LC_RGBLight_Param.RGB_Mode_Change_Color_Num	=	0;
						LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
						LC_RGBLight_Param.RGB_Light_Mode			=	RGB_Jump_SevenColors;
						LC_RGBLight_Param.RGB_Speed_Reserved		=	30;
						LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					}else if(LC_IR_Keyboard_Num == IRKey_Mode_Fade){
						LC_RGBLight_Modetick	=	0;
						LC_RGBLight_Param.RGB_Mode_Change_Color_Num	=	0;
						LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
						LC_RGBLight_Param.RGB_Light_Mode			=	RGB_Fade_SevenColors;
						LC_RGBLight_Param.RGB_Speed_Reserved		=	100;
						LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					}else if(LC_IR_Keyboard_Num == IRKey_Mode_Smooth){
						LC_RGBLight_Modetick	=	0;
						LC_RGBLight_Param.RGB_Mode_Change_Color_Num =	0;
						LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
						LC_RGBLight_Param.RGB_Light_Mode			=	RGB_Smooth;
						LC_RGBLight_Param.RGB_Speed_Reserved		=	100;
						LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					}else{
						LC_RGBLight_Mode_Static_IRKeyboard(LC_IR_Keyboard_Num - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
						// LOG("IR choose color %d %d %d\n",LC_RGBLight_Param.RGB_rValue,LC_RGBLight_Param.RGB_gValue,LC_RGBLight_Param.RGB_bValue);
					}
					LC_RGBLight_Reserve_Mode();
				}
			}
		}
		//	20 Keys Type
		else if(LC_IR_Keyboard_UserNum == IR_KeyBoard_Type_20Keys){
			if(LC_RGBLight_Param.RGB_Light_State == State_On){
				//	turn off
				if(LC_IR_Keyboard_Num == IRKey_On){
					LC_RGBLight_Turn_Onoff(State_Off);
				}
				//	level up
				else if(LC_IR_Keyboard_Num == IRKey_Up){
					if(LC_RGBLight_Param.RGB_Light_Mode == RGB_Plate_Mode){
						if(LC_RGBLight_Param.RGB_Light_Level < 100){
							LC_RGBLight_Param.RGB_Light_Level	+=	10;
						}else{
							LC_RGBLight_Param.RGB_Light_Level	=	100;
						}
						LC_RGBLight_Param.RGB_rValue		=	LC_RGBLight_Param.RGB_rValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
						LC_RGBLight_Param.RGB_gValue		=	LC_RGBLight_Param.RGB_gValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
						LC_RGBLight_Param.RGB_bValue		=	LC_RGBLight_Param.RGB_bValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
					#if(LC_RGBLight_Module == RGBWLight)
						LC_RGBLight_Param.RGB_wValue		=	0;
					#endif
					}
				}
				//	level down
				else if(LC_IR_Keyboard_Num == IRKey_Down){
					if(LC_RGBLight_Param.RGB_Light_Mode == RGB_Plate_Mode){
						if(LC_RGBLight_Param.RGB_Light_Level > 20){
							LC_RGBLight_Param.RGB_Light_Level	-=	10;
						}else{
							LC_RGBLight_Param.RGB_Light_Level	=	10;
						}
						LC_RGBLight_Param.RGB_rValue		=	LC_RGBLight_Param.RGB_rValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
						LC_RGBLight_Param.RGB_gValue		=	LC_RGBLight_Param.RGB_gValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
						LC_RGBLight_Param.RGB_bValue		=	LC_RGBLight_Param.RGB_bValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
					#if(LC_RGBLight_Module == RGBWLight)
						LC_RGBLight_Param.RGB_wValue		=	0;
					#endif
					}
				}			
				//	R2
				else if((LC_IR_Keyboard_Num >= IRKey_Red) && (LC_IR_Keyboard_Num <= IRKey_White)){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(LC_IR_Keyboard_Num - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	R3
				else if((LC_IR_Keyboard_Num >= IRKey_Orange) && (LC_IR_Keyboard_Num <= IRKey_Purple)){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(LC_IR_Keyboard_Num - IRKey_Orange + 20, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	jump3
				else if(LC_IR_Keyboard_Num == IRKey_Jump3){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Modetick	=	0;
					LC_RGBLight_Param.RGB_Mode_Change_Color_Num =	0;
					LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
					LC_RGBLight_Param.RGB_Light_Mode			=	RGB_Jump_ThreeColors;
					LC_RGBLight_Param.RGB_Speed_Reserved		=	30;
					LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					LC_RGBLight_Reserve_Mode();
				}
				//	jump7
				else if(LC_IR_Keyboard_Num == IRKey_Jump7){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Modetick	=	0;
					LC_RGBLight_Param.RGB_Mode_Change_Color_Num =	0;
					LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
					LC_RGBLight_Param.RGB_Light_Mode			=	RGB_Jump_SevenColors;
					LC_RGBLight_Param.RGB_Speed_Reserved		=	30;
					LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					LC_RGBLight_Reserve_Mode();
				}
				//	fade3
				else if(LC_IR_Keyboard_Num == IRKey_Fade3){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Modetick	=	0;
					LC_RGBLight_Param.RGB_Mode_Change_Color_Num =	0;
					LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
					LC_RGBLight_Param.RGB_Light_Mode			=	RGB_Fade_ThreeColors;
					LC_RGBLight_Param.RGB_Speed_Reserved		=	100;
					LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					LC_RGBLight_Reserve_Mode();
				}
				//	fade7
				else if(LC_IR_Keyboard_Num == IRKey_Fade7){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Modetick	=	0;
					LC_RGBLight_Param.RGB_Mode_Change_Color_Num =	0;
					LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
					LC_RGBLight_Param.RGB_Light_Mode			=	RGB_Fade_SevenColors;
					LC_RGBLight_Param.RGB_Speed_Reserved		=	100;
					LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					LC_RGBLight_Reserve_Mode();
				}
				//	Music1 -- Music4
				else if((LC_IR_Keyboard_Num >= IRKey_Music1) && (LC_IR_Keyboard_Num <= IRKey_Music4)){
					LC_Mic_Sample_Start();
					LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
					LC_RGBLight_Param.RGB_Light_Mode			=	LC_IR_Keyboard_Num;
					LC_RGBLight_Reserve_Mode();
				}
			}else if(LC_RGBLight_Param.RGB_Light_State == State_Off){
				//	turn on
				if(LC_IR_Keyboard_Num == 0x02){
					LC_RGBLight_Turn_Onoff(State_On);
				}
			}
		}
		//	44 Keys Type
		else if(LC_IR_Keyboard_UserNum == IR_KeyBoard_Type_44Keys){
			// RGBLight Off
			if(LC_RGBLight_Param.RGB_Light_State == State_On){
				//	turn off
				if(LC_IR_Keyboard_Num == 0x40){
					LC_RGBLight_Turn_Onoff(State_Off);
				}
				//	stop
				else if(LC_IR_Keyboard_Num	== 0x41){
					if((LC_RGBLight_Param.RGB_Light_Mode >= RGB_Fade_ThreeColors) && \
						(LC_RGBLight_Param.RGB_Light_Mode <= RGB_Smooth)){
						LC_RGBLight_Param.RGB_Light_Mode_Auto	=	State_Off;
						LC_RGBLight_Param.RGB_Light_Mode		=	RGB_Plate_Mode;
						LC_RGBLight_Param.RGB_rValue_New		=	LC_RGBLight_Param.RGB_rValue;
						LC_RGBLight_Param.RGB_gValue_New		=	LC_RGBLight_Param.RGB_gValue;
						LC_RGBLight_Param.RGB_bValue_New		=	LC_RGBLight_Param.RGB_bValue;
					}
				}
				//	level up
				else if(LC_IR_Keyboard_Num == 0x5C){
					if(LC_RGBLight_Param.RGB_Light_Mode == RGB_Plate_Mode){
						if(LC_RGBLight_Param.RGB_Light_Level < 100){
							LC_RGBLight_Param.RGB_Light_Level	+=	10;
						}else{
							LC_RGBLight_Param.RGB_Light_Level	=	100;
						}
						LC_RGBLight_Param.RGB_rValue		=	LC_RGBLight_Param.RGB_rValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
						LC_RGBLight_Param.RGB_gValue		=	LC_RGBLight_Param.RGB_gValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
						LC_RGBLight_Param.RGB_bValue		=	LC_RGBLight_Param.RGB_bValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
					#if(LC_RGBLight_Module == RGBWLight)
						LC_RGBLight_Param.RGB_wValue		=	0;
					#endif
					}
				}
				//	level down
				else if(LC_IR_Keyboard_Num == 0x5D){
					if(LC_RGBLight_Param.RGB_Light_Mode == RGB_Plate_Mode){
						if(LC_RGBLight_Param.RGB_Light_Level > 20){
							LC_RGBLight_Param.RGB_Light_Level	-=	10;
						}else{
							LC_RGBLight_Param.RGB_Light_Level	=	10;
						}
						LC_RGBLight_Param.RGB_rValue		=	LC_RGBLight_Param.RGB_rValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
						LC_RGBLight_Param.RGB_gValue		=	LC_RGBLight_Param.RGB_gValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
						LC_RGBLight_Param.RGB_bValue		=	LC_RGBLight_Param.RGB_bValue_New*LC_RGBLight_Param.RGB_Light_Level/RGB_LEVEL_PECENT;
					#if(LC_RGBLight_Module == RGBWLight)
						LC_RGBLight_Param.RGB_wValue		=	0;
					#endif
					}
				}
				//	red
				else if(LC_IR_Keyboard_Num == 0x58){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(IRKey_Light_Red - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	green
				else if(LC_IR_Keyboard_Num == 0x59){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(IRKey_Light_Green - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	blue
				else if(LC_IR_Keyboard_Num == 0x45){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(IRKey_Light_Blue - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	white
				else if(LC_IR_Keyboard_Num == 0x44){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(IRKey_Light_White - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	C1R3
				else if(LC_IR_Keyboard_Num == 0x54){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(IRKey_Col1_Row3 - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	C2R3
				else if(LC_IR_Keyboard_Num == 0x55){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(IRKey_Col2_Row3 - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	C3R3
				else if(LC_IR_Keyboard_Num == 0x49){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(IRKey_Col3_Row3 - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	C4R3
				else if(LC_IR_Keyboard_Num == 0x48){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(IRKey_Mode_Flash - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	C1R4
				else if(LC_IR_Keyboard_Num == 0x50){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(IRKey_Col1_Row4 - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	C2R4
				else if(LC_IR_Keyboard_Num == 0x51){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(IRKey_Col2_Row4 - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	C3R4
				else if(LC_IR_Keyboard_Num == 0x4D){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(IRKey_Col3_Row4 - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	C4R4
				else if(LC_IR_Keyboard_Num == 0x4C){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(IRKey_Mode_Strobe - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	R5
				else if((LC_IR_Keyboard_Num >= 0x1C) && (LC_IR_Keyboard_Num <= 0x1F)){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(LC_IR_Keyboard_Num - 0x1c + IRKey_Col1_Row5 - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	R6
				else if((LC_IR_Keyboard_Num >= 0x18) && (LC_IR_Keyboard_Num <= 0x1B)){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Mode_Static_IRKeyboard(LC_IR_Keyboard_Num - 0x18 + IRKey_Col1_Row6 - IRKey_Light_Red, LC_RGBLight_Mode_IR_Buffer);
					LC_RGBLight_Reserve_Mode();
				}
				//	jump3
				else if(LC_IR_Keyboard_Num == 0x04){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Modetick	=	0;
					LC_RGBLight_Param.RGB_Mode_Change_Color_Num =	0;
					LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
					LC_RGBLight_Param.RGB_Light_Mode			=	RGB_Jump_ThreeColors;
					LC_RGBLight_Param.RGB_Speed_Reserved		=	30;
					LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					LC_RGBLight_Reserve_Mode();
				}
				//	jump7
				else if(LC_IR_Keyboard_Num == 0x05){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Modetick	=	0;
					LC_RGBLight_Param.RGB_Mode_Change_Color_Num =	0;
					LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
					LC_RGBLight_Param.RGB_Light_Mode			=	RGB_Jump_SevenColors;
					LC_RGBLight_Param.RGB_Speed_Reserved		=	30;
					LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					LC_RGBLight_Reserve_Mode();
				}
				//	fade3
				else if(LC_IR_Keyboard_Num == 0x06){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Modetick	=	0;
					LC_RGBLight_Param.RGB_Mode_Change_Color_Num =	0;
					LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
					LC_RGBLight_Param.RGB_Light_Mode			=	RGB_Fade_ThreeColors;
					LC_RGBLight_Param.RGB_Speed_Reserved		=	100;
					LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					LC_RGBLight_Reserve_Mode();
				}
				//	fade7
				else if(LC_IR_Keyboard_Num == 0x07){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Modetick	=	0;
					LC_RGBLight_Param.RGB_Mode_Change_Color_Num =	0;
					LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
					LC_RGBLight_Param.RGB_Light_Mode			=	RGB_Fade_SevenColors;
					LC_RGBLight_Param.RGB_Speed_Reserved		=	100;
					LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					LC_RGBLight_Reserve_Mode();
				}
				//	flash
				else if(LC_IR_Keyboard_Num == 0x0B){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Modetick	=	0;
					LC_RGBLight_Param.RGB_Mode_Change_Color_Num =	0;
					LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
					LC_RGBLight_Param.RGB_Light_Mode			=	RGB_Flash_White;
					LC_RGBLight_Param.RGB_Speed_Reserved		=	20;
					LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					LC_RGBLight_Reserve_Mode();
				}
				//	auto
				else if(LC_IR_Keyboard_Num == 0x0F){
					if(LC_RGBLight_Param.RGB_Light_Mode_Auto != State_On){
						LC_Mic_Sample_Stop();
						LC_RGBLight_Modetick	=	0;
						LC_RGBLight_Param.RGB_Mode_Change_Color_Num	=	0;
						LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_On;
						LC_RGBLight_Param.RGB_Light_Mode			=	RGB_Jump_SevenColors;
						LC_RGBLight_Param.RGB_Mode_Change_Speed		=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
						LC_RGBLight_Reserve_Mode();
					}
				}
				//	Speed Up
				else if(LC_IR_Keyboard_Num == 0x17){
					if(LC_RGBLight_Param.RGB_Speed_Reserved	< 100){
						LC_RGBLight_Param.RGB_Speed_Reserved += 10;
					}else{
						LC_RGBLight_Param.RGB_Speed_Reserved	=	100;
					}
					LC_RGBLight_Param.RGB_Mode_Change_Speed	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					LC_RGBLight_Reserve_Mode();
				}
				//	Speed Down
				else if(LC_IR_Keyboard_Num == 0x13){
					if(LC_RGBLight_Param.RGB_Speed_Reserved	> 0){
						LC_RGBLight_Param.RGB_Speed_Reserved -= 10;
					}else{
						LC_RGBLight_Param.RGB_Speed_Reserved	=	0;
					}
					LC_RGBLight_Param.RGB_Mode_Change_Speed	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
					LC_RGBLight_Reserve_Mode();
				}
				//	DIY1
				else if(LC_IR_Keyboard_Num == 0x0C){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Param.RGB_Light_Mode	=	RGB_DIY_Mode;
					LC_RGBLight_Param.RGB_rValue		=	LC_RGBLight_Mode_IR_Buffer[RGB_IR_BUFFER_DIYOFFSET*3];
					LC_RGBLight_Param.RGB_gValue		=	LC_RGBLight_Mode_IR_Buffer[RGB_IR_BUFFER_DIYOFFSET*3 + 1];
					LC_RGBLight_Param.RGB_bValue		=	LC_RGBLight_Mode_IR_Buffer[RGB_IR_BUFFER_DIYOFFSET*3 + 2];
				#if(LC_RGBLight_Module == RGBWLight)
					LC_RGBLight_Param.RGB_wValue		=	0;
				#endif
				}
				//	DIY2
				else if(LC_IR_Keyboard_Num == 0x0D){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Param.RGB_Light_Mode	=	RGB_DIY_Mode;
					LC_RGBLight_Param.RGB_rValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 1)*3];
					LC_RGBLight_Param.RGB_gValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 1)*3 + 1];
					LC_RGBLight_Param.RGB_bValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 1)*3 + 2];
				#if(LC_RGBLight_Module == RGBWLight)
					LC_RGBLight_Param.RGB_wValue		=	0;
				#endif
				}
				//	DIY3
				else if(LC_IR_Keyboard_Num == 0x0E){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Param.RGB_Light_Mode	=	RGB_DIY_Mode;
					LC_RGBLight_Param.RGB_rValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 2)*3];
					LC_RGBLight_Param.RGB_gValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 2)*3 + 1];
					LC_RGBLight_Param.RGB_bValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 2)*3 + 2];
				#if(LC_RGBLight_Module == RGBWLight)
					LC_RGBLight_Param.RGB_wValue		=	0;
				#endif
				}
				//	DIY4
				else if(LC_IR_Keyboard_Num == 0x08){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Param.RGB_Light_Mode	=	RGB_DIY_Mode;
					LC_RGBLight_Param.RGB_rValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 3)*3];
					LC_RGBLight_Param.RGB_gValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 3)*3 + 1];
					LC_RGBLight_Param.RGB_bValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 3)*3 + 2];
				#if(LC_RGBLight_Module == RGBWLight)
					LC_RGBLight_Param.RGB_wValue		=	0;
				#endif
				}
				//	DIY5
				else if(LC_IR_Keyboard_Num == 0x09){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Param.RGB_Light_Mode	=	RGB_DIY_Mode;
					LC_RGBLight_Param.RGB_rValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 4)*3];
					LC_RGBLight_Param.RGB_gValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 4)*3 + 1];
					LC_RGBLight_Param.RGB_bValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 4)*3 + 2];
				#if(LC_RGBLight_Module == RGBWLight)
					LC_RGBLight_Param.RGB_wValue		=	0;
				#endif
				}
				//	DIY6
				else if(LC_IR_Keyboard_Num == 0x0A){
					LC_Mic_Sample_Stop();
					LC_RGBLight_Param.RGB_Light_Mode	=	RGB_DIY_Mode;
					LC_RGBLight_Param.RGB_rValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 5)*3];
					LC_RGBLight_Param.RGB_gValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 5)*3 + 1];
					LC_RGBLight_Param.RGB_bValue		=	LC_RGBLight_Mode_IR_Buffer[(RGB_IR_BUFFER_DIYOFFSET + 5)*3 + 2];
				#if(LC_RGBLight_Module == RGBWLight)
					LC_RGBLight_Param.RGB_wValue		=	0;
				#endif
				}
				//	Red Up
				else if(LC_IR_Keyboard_Num == 0x14){
					if(LC_RGBLight_Param.RGB_Light_Mode	== RGB_DIY_Mode){
						if(LC_RGBLight_Param.RGB_rValue < RGB_PWM_MAX){
							LC_RGBLight_Param.RGB_rValue	+=	5;
						}else{
							LC_RGBLight_Param.RGB_rValue	=	RGB_PWM_MAX;
						}
					}
				}
				//	Red Down
				else if(LC_IR_Keyboard_Num == 0x10){
					if(LC_RGBLight_Param.RGB_Light_Mode	== RGB_DIY_Mode){
						if(LC_RGBLight_Param.RGB_rValue > 0){
							LC_RGBLight_Param.RGB_rValue	-=	5;
						}else{
							LC_RGBLight_Param.RGB_rValue	=	0;
						}
					}
				}
				//	Green Up
				else if(LC_IR_Keyboard_Num == 0x15){
					if(LC_RGBLight_Param.RGB_Light_Mode	== RGB_DIY_Mode){
						if(LC_RGBLight_Param.RGB_gValue < RGB_PWM_MAX){
							LC_RGBLight_Param.RGB_gValue	+=	5;
						}else{
							LC_RGBLight_Param.RGB_gValue	=	RGB_PWM_MAX;
						}
					}
				}
				//	Green Down
				else if(LC_IR_Keyboard_Num == 0x11){
					if(LC_RGBLight_Param.RGB_Light_Mode	== RGB_DIY_Mode){
						if(LC_RGBLight_Param.RGB_gValue > 0){
							LC_RGBLight_Param.RGB_gValue	-=	5;
						}else{
							LC_RGBLight_Param.RGB_gValue	=	0;
						}
					}
				}
				//	Blue Up
				else if(LC_IR_Keyboard_Num == 0x16){
					if(LC_RGBLight_Param.RGB_Light_Mode	== RGB_DIY_Mode){
						if(LC_RGBLight_Param.RGB_bValue < RGB_PWM_MAX){
							LC_RGBLight_Param.RGB_bValue	+=	5;
						}else{
							LC_RGBLight_Param.RGB_bValue	=	RGB_PWM_MAX;
						}
					}
				}
				//	Blue Down
				else if(LC_IR_Keyboard_Num == 0x12){
					if(LC_RGBLight_Param.RGB_Light_Mode	== RGB_DIY_Mode){
						if(LC_RGBLight_Param.RGB_bValue > 0){
							LC_RGBLight_Param.RGB_bValue	-=	5;
						}else{
							LC_RGBLight_Param.RGB_bValue	=	0;
						}
					}
				}
			}
			//	RGBLight On
			else if(LC_RGBLight_Param.RGB_Light_State == State_Off){
				if(LC_IR_Keyboard_Num == 0x40){
					LC_RGBLight_Turn_Onoff(State_On);
				}
			}
		}
		return(events ^ IR_REC_EVENT_LEVEL2);
	}

	if(events & IR_REC_EVENT_LEVEL1)
	{
		LC_IR_Analysis_Data(LC_IR_Analysis_100ns_Cnt - LC_IR_Last_Posedge_Time);
		return(events^IR_REC_EVENT_LEVEL1);
	}

    // Discard unknown events
	return 0;
}
/** @}*/

