/**
 *	@file		LC_Key.h
 *	@author		YQ
 *	@date		09/17/2020
 *	@version	1.0.0
 *
 */

/*!
 * 	@defgroup	LC_Key
 *	@brief
 *	@{*/
/*------------------------------------------------------------------*/
/* 					 head files include 						 	*/
/*------------------------------------------------------------------*/
#include "LC_Key.h"
#include "LC_UI_Led_Buzzer.h"
#include "LC_RGBLight_Mode.h"
#include "LC_Mic_Sample.h"
/*------------------------------------------------------------------*/
/* 					 	public variables		 					*/
/*------------------------------------------------------------------*/
uint8	LC_Key_TaskID;
lc_key_struct_data	LC_Key_Param	=	{
	.key_down_sys_tick		=	0,
	.key_down_flag			=	0,
	.key_repeated_num		=	0,

};
/*------------------------------------------------------------------*/
/* 					 	public functions		 					*/
/*------------------------------------------------------------------*/
/*!
 *	@fn			LC_Gpio_Key_Init
 *	@brief		Initialize the key pins. 
 *	@param[in]	none.
 *	@return		none.
 */
void LC_Gpio_Key_Init(void)
{
	hal_gpio_fmux(MY_KEY_NO1_GPIO, Bit_DISABLE) ;              		//set pin as gpio
	hal_gpio_cfg_analog_io(MY_KEY_NO1_GPIO,Bit_DISABLE) ;			//set pin as digital
	hal_gpio_pin_init(MY_KEY_NO1_GPIO, IE) ;                  		//set gpio input
	hal_gpio_pull_set(MY_KEY_NO1_GPIO, STRONG_PULL_UP) ;      		//pull up 150k
	hal_gpioin_register(MY_KEY_NO1_GPIO, LC_Key_Pin_IntHandler, LC_Key_Pin_IntHandler);

	hal_gpio_fmux(MY_KEY_NO2_GPIO, Bit_DISABLE) ;              		//set pin as gpio
	hal_gpio_cfg_analog_io(MY_KEY_NO2_GPIO,Bit_DISABLE) ;			//set pin as digital
	hal_gpio_pin_init(MY_KEY_NO2_GPIO, IE) ;                  		//set gpio input
	hal_gpio_pull_set(MY_KEY_NO2_GPIO, STRONG_PULL_UP) ;      		//pull up 150k
	hal_gpioin_register(MY_KEY_NO2_GPIO, LC_Key_Pin_IntHandler, LC_Key_Pin_IntHandler);

	hal_gpio_fmux(MY_KEY_NO3_GPIO, Bit_DISABLE) ;              		//set pin as gpio
	hal_gpio_cfg_analog_io(MY_KEY_NO3_GPIO,Bit_DISABLE) ;			//set pin as digital
	hal_gpio_pin_init(MY_KEY_NO3_GPIO, IE) ;                  		//set gpio input
	hal_gpio_pull_set(MY_KEY_NO3_GPIO, STRONG_PULL_UP) ;      		//pull up 150k
	hal_gpioin_register(MY_KEY_NO3_GPIO, LC_Key_Pin_IntHandler, LC_Key_Pin_IntHandler);

	hal_pwrmgr_register(MOD_USR8, NULL, NULL);	
	hal_pwrmgr_lock(MOD_USR8);

}
/*!
 *	@fn			LC_Key_Task_Init 
 *	@brief		Initialize function for the KEY Task. 
 *	@param[in]	task_id		: 	the ID assigned by OSAL,
 *								used to send message and set timer.
 *	@retrurn	none.
 */
void LC_Key_Task_Init(uint8 task_id)
{
	LC_Key_TaskID	=	task_id;
	LOG("LC_Gpio_Key_Init:\n");
	osal_start_timerEx(LC_Key_TaskID, KEY_EVENT_LEVEL1, 100);
}
/*!
 *	@fn			LC_Key_ProcessEvent
 *	@brief		KEY Task event processor.This function
 *				is called to processs all events for the task.Events
 *				include timers,messages and any other user defined events.
 *	@param[in]	task_id			:The OSAL assigned task ID.
 *	@param[in]	events			:events to process.This is a bit map and can
 *									contain more than one event.
 */
uint16	LC_Key_ProcessEvent(uint8 task_id, uint16 events)
{
	VOID task_id;	// OSAL required parameter that isn't used in this function
	if(events & SYS_EVENT_MSG){
		uint8	*pMsg;
		if((pMsg = osal_msg_receive(LC_Key_TaskID)) != NULL){
			LC_Common_ProcessOSALMsg((osal_event_hdr_t *)pMsg);
            // Release the OSAL message
			VOID osal_msg_deallocate(pMsg);
		}
		return(events ^ SYS_EVENT_MSG);
	}
	if(events & KEY_EVENT_LEVEL1){
		static 	uint8 	LC_last_button_pressed		=	0 ; 	//key pressed once time
		static 	uint8 	LC_last_button_numbale		=	0 ; 	//key value now
		
		static	uint32	LC_last_button_press_time		=	0 ; 	//tick of releasing key
		static	uint32	LC_last_button_release_time		=	0 ; 	//tick of pressing key
		static	uint32	LC_key_time_temp				=	0 ; 	//deal key event every 20ms

//		static	uint8	Key_Long_Press_3s_Enable		=	0;		//key pressed 3s once
		static	uint8	Key_Press_Once_Enable			=	0;		//key pressed once flag
		static	uint8	Key_Value_Reserved				=	0;
		static	uint8	Key_Mode_Cnt					=	0;
//		static	uint8	Key_Mic_Cnt						=	0;

		LC_key_time_temp = hal_systick()|1;
//		if(LC_Key_Param.key_down_flag){
//			if(LC_last_button_numbale && clock_time_exceed_func(LC_last_button_press_time,1000)){
//				LC_Key_Param.key_repeated_num	=	0;
//				if(!Key_Long_Press_3s_Enable){
//					Key_Long_Press_3s_Enable	=	1;
//					if(Key_Press_Once_Enable){
//						Key_Press_Once_Enable	=	0;
//					}
//					LOG("Key_Long_Press_3s: \n") ;
//				}
//			}			
//		}else{ 
//			if(Key_Long_Press_3s_Enable){
//				Key_Long_Press_3s_Enable	=	0;
//				LOG("Key_Long_Release:\n");
//			}
//		}
		
		if(LC_Key_Param.key_down_flag){
			if(!LC_last_button_pressed && clock_time_exceed_func(LC_last_button_release_time,20)){
				LC_last_button_pressed		=	1 ;
				LC_last_button_press_time	=	LC_key_time_temp ;
				LC_last_button_numbale		=	LC_Key_Param.key_down_flag ;
				Key_Value_Reserved			=	LC_Key_Param.key_down_flag;
			}
		}else{
			if(LC_last_button_pressed && clock_time_exceed_func(LC_last_button_press_time,20) ){
				LC_last_button_release_time	=	LC_key_time_temp;
				LC_last_button_pressed 		=	0 ;
			}
		}
		if(LC_Key_Param.key_repeated_num && LC_Key_Param.key_down_sys_tick && clock_time_exceed_func(LC_Key_Param.key_down_sys_tick,300)){
			LOG("Key total Kick num: %d, key is %d\n",LC_Key_Param.key_repeated_num,Key_Value_Reserved) ;

			if((LC_Key_Param.key_repeated_num == 1) && (Key_Press_Once_Enable == State_Off)){
				Key_Press_Once_Enable	=	State_On;
				if(Key_Value_Reserved == 1){
					if(LC_RGBLight_Param.RGB_Light_State == State_On){
						// LOG("Key_Mode_Cnt %d\n",Key_Mode_Cnt);
						LC_Mic_Sample_Stop();
						if(Key_Mode_Cnt < 12){
							LC_RGBLight_Param.RGB_Light_Mode	=	RGB_Fade_ThreeColors + Key_Mode_Cnt;
						}else if(Key_Mode_Cnt < 20){
							LC_RGBLight_Param.RGB_Light_Mode	=	RGB_Flash_SevenColors + Key_Mode_Cnt - 12;
						}else if(Key_Mode_Cnt < 22){
							LC_RGBLight_Param.RGB_Light_Mode	=	RGB_Jump_ThreeColors + Key_Mode_Cnt - 20;
						}else{
							Key_Mode_Cnt	=	0;
						}
						LC_RGBLight_Modetick	=	0;
						LC_RGBLight_Param.RGB_Mode_Change_Color_Num	=	0;
						LC_RGBLight_Param.RGB_Light_Mode_Auto		=	State_Off;
						LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
						LC_RGBLight_Reserve_Mode();
						Key_Mode_Cnt++;
					}
				}else if(Key_Value_Reserved == 2){
					if(LC_RGBLight_Param.RGB_Light_State == State_On){
						if(LC_RGBLight_Param.RGB_Light_Mode	!= RGB_Mic_Mode_Classical){
							LOG("turn on mic\n");
							LC_Mic_Sample_Start();
							LC_RGBLight_Param.RGB_Light_Mode_Auto	=	State_Off;
							LC_RGBLight_Param.RGB_Light_Mode		=	RGB_Mic_Mode_Classical;
							LC_RGBLight_Reserve_Mode();
						}else if(LC_RGBLight_Param.RGB_Light_Mode == RGB_Mic_Mode_Classical){
							LC_Mic_Sample_Stop();
							LC_RGBLight_Param.RGB_Light_Mode_Auto	=	State_Off;
							LC_RGBLight_Param.RGB_Light_Mode		=	RGB_Jump_SevenColors;
							LC_RGBLight_Param.RGB_Mode_Change_Speed		=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
							LOG("turn off  mic\n");
							LC_RGBLight_Reserve_Mode();
						}
					}
				}else if(Key_Value_Reserved == 3){
					if(LC_RGBLight_Param.RGB_Light_State == State_On){
						LC_RGBLight_Turn_Onoff(State_Off);
					}else if(LC_RGBLight_Param.RGB_Light_State == State_Off){
						LC_RGBLight_Turn_Onoff(State_On);
					}
				}
			}


			if(Key_Press_Once_Enable == State_On){
				Key_Press_Once_Enable	=	State_Off;
				LOG("Key Once Release:\n");
			}
			LC_Key_Param.key_down_sys_tick		=	0;
			LC_Key_Param.key_repeated_num		=	0;
			LC_last_button_numbale	=	0;
			Key_Value_Reserved		=	0;
		}
		if(LC_last_button_numbale && !LC_Key_Param.key_down_flag && clock_time_exceed_func(LC_last_button_press_time,20)){
			LC_Key_Param.key_repeated_num++ ;
			LC_Key_Param.key_down_sys_tick = LC_key_time_temp ;
//			LOG("key time num: %d, key is%d\n",LC_Key_Param.key_repeated_num,LC_last_button_numbale);
			LC_last_button_numbale = 0 ;
			
		}
        if(LC_Key_Param.key_down_flag || LC_Key_Param.key_repeated_num){
			osal_start_timerEx(LC_Key_TaskID, KEY_EVENT_LEVEL1, 20);
        }
		return(events ^ KEY_EVENT_LEVEL1);
	}

    // Discard unknown events
    return 0;
}
/** @}*/

