/**
 *	@file		LC_Mic_Sample.h
 *	@author		YQ
 *	@date		12/25/2020
 *	@version	1.0.0
 */

/*!
 *	@defgroup	LC_Mic_Sample
 *	@brief
 *	@{*/
/*------------------------------------------------------------------*/
/* 					 head files include 						 	*/
/*------------------------------------------------------------------*/
#include "LC_Mic_Sample.h"
#include "LC_RGBLight_Mode.h"
/*------------------------------------------------------------------*/
/* 					 	local variables		 						*/
/*------------------------------------------------------------------*/
static	uint8	LC_Mic_Sample_Enable	=	0;
static	int32	LC_Mic_Sample_Data		=	0;
const	static	uint8	LC_RGBLight_Mic_Buffer[24]	={
	255,   0,   0,		0, 255,   0,		  0,   0, 255,	//	red,		green,	blue,
	255, 255,   0,		0, 255, 255,		255,   0, 255,	//	yellow,	cyan,	magenta,
	255, 255, 255,		255,0,0,	//	white,
};
uint8	LC_Mic_RGBData_Index			=	0;
uint8	LC_Mic_RGBData_Speed			=	0;
/*------------------------------------------------------------------*/
/* 					 	public variables		 					*/
/*------------------------------------------------------------------*/
uint8	LC_Mic_Sample_TaskID	=	0;
uint8	LC_Mic_Sample_Busy		=	0;	//	0:busy,1:idle
int		voiceLeftBuf[MAX_VOICE_BUF_SIZE];
/*------------------------------------------------------------------*/
/* 					 	public functions		 					*/
/*------------------------------------------------------------------*/
void	disable_normal_adc_interrupt(void)
{
	MASK_ADC_INT;
}
void	enable_normal_adc_interrupt(void)
{
	ENABLE_ADC_INT;
}
void	disable_voice_adc_interrupt(void)
{
	CLEAR_VOICE_HALF_INT;
	CLEAR_VOICE_FULL_INT;
	MASK_VOICE_INT;
}

int16	voice_evt_data_caculate(int *micOut, int16 len)
{
	int lvlWin = 1400;
	int multLmt = 16;
	int multCnt = 0;
	int fltOut = 0 ;
	int tmp = 0 ;
	int32 tmp1 = 0;
	static int32 return_temp[3] = {0};
#define FLT_AVG_LEN 16 

	for (int16 ii = 2; ii<len-FLT_AVG_LEN; ii++){	
		tmp1 = 0;
		for(int16 jj=0;jj<FLT_AVG_LEN;jj++)
		{
			fltOut= ( micOut[ii+jj]<0)?(-micOut[ii+jj]):( micOut[ii+jj]) ;
			tmp1+=fltOut;
		}
		
		tmp = tmp1/FLT_AVG_LEN/lvlWin; 	
		if(tmp>0)			  
			multCnt = 0;   
		else	   
			multCnt=multCnt+1;	 

		return_temp[2]=return_temp[1];
		return_temp[1]=return_temp[0];
		
		if(multCnt>multLmt) 	  
			return_temp[0]=0;   
		else{	 
			return_temp[0] += tmp;
			if(return_temp[0]>0x0fffffff){
				return_temp[0] = 0x0fffffff ;
			}
		}

		//return_temp = micOut[ii] ;
	}
	if(return_temp[1]<=40 && return_temp[2]<=40)
		return 0;
	return return_temp[0] ;
}

void	voice_evt_handler_adpcm(voice_Evt_t *pev)
{
	uint8	left_right_chanle;
	static	uint16	voiceSampleRight_numble_tick	=	0;
	uint32	voiceSampleDual;
//	int		voiceSampleRight;
	int		voiceSampleLeft;
	uint32	i=0;

	left_right_chanle	=	1;

	if(pev->type == HAL_VOICE_EVT_DATA){
		if(LC_Mic_Sample_Enable == 0){
			return ;
		}
		for(i = 0;i<pev->size;i++){
			voiceSampleDual	=	pev->data[i];
//			voiceSampleRight	=	(int16)(voiceSampleDual & 65535);
			voiceSampleLeft		=	(int16)((voiceSampleDual>>16)&65535);
			if(left_right_chanle == 1){
				voiceLeftBuf[voiceSampleRight_numble_tick]	=	voiceSampleLeft;
				voiceSampleRight_numble_tick++;
				if(voiceSampleRight_numble_tick > (MAX_VOICE_BUF_SIZE - 4)){
//					int32	data_temp	= voice_evt_data_caculate(voiceLeftBuf, MAX_VOICE_BUF_SIZE - 4);

					LC_Mic_Sample_Data	=	voice_evt_data_caculate(voiceLeftBuf, MAX_VOICE_BUF_SIZE - 4);
//					LOG("%d\n",LC_Mic_Sample_Data);
					LC_Mic_RGBData_Speed++;
					if(LC_Mic_RGBData_Speed > 7){
						LC_Mic_RGBData_Speed	=	0;
						if(LC_Mic_Sample_Data > 50){
                            LC_RGBLight_Param.RGB_rValue	=	LC_RGBLight_Mic_Buffer[LC_Mic_RGBData_Index*3 + 0];
                            LC_RGBLight_Param.RGB_gValue	=	LC_RGBLight_Mic_Buffer[LC_Mic_RGBData_Index*3 + 1];
                            LC_RGBLight_Param.RGB_bValue	=	LC_RGBLight_Mic_Buffer[LC_Mic_RGBData_Index*3 + 2];
                            LC_Mic_RGBData_Index	=	0x07 & (LC_Mic_RGBData_Index + 1);
						#if(LC_RGBLight_Module == RGBWLight)
							LC_RGBLight_Param.RGB_wValue		=	0;
						#endif
						}else{
							if(LC_RGBLight_Param.RGB_Light_Mode == RGB_Mic_Mode_Soft){
								LC_RGBLight_Param.RGB_Light_Mode	=	RGB_Fade_SevenColors;
								LC_RGBLight_Modetick				=	0;
								LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
							}else if(LC_RGBLight_Param.RGB_Light_Mode == RGB_Mic_Mode_Beats){
								LC_RGBLight_Param.RGB_rValue		=	0;
								LC_RGBLight_Param.RGB_gValue		=	0;
								LC_RGBLight_Param.RGB_bValue		=	0;
							#if(LC_RGBLight_Module == RGBWLight)
								LC_RGBLight_Param.RGB_wValue		=	0;
							#endif
							}else if(LC_RGBLight_Param.RGB_Light_Mode == RGB_Mic_Mode_Disco){
								LC_RGBLight_Param.RGB_Light_Mode	=	RGB_Flash_SevenColors;
								LC_RGBLight_Modetick				=	0;
								LC_RGBLight_Param.RGB_Mode_Change_Speed 	=	LC_RGBLight_Mode_Speed(LC_RGBLight_Param.RGB_Speed_Reserved);
							}
						}
					}
					// LC_Mic_Sample_Data	=	0;
					voiceSampleRight_numble_tick	=	0;
					for(int16 i = 0;i<MAX_VOICE_BUF_SIZE;i++){
						voiceLeftBuf[i]	=	0;
					}
				}
	
			}
		}
	}
}
void	voiceCaputerInit(void)
{
	voice_Cfg_t	cfg;

	cfg.voiceSelAmicDmic	=	0;
	cfg.amicGain			=	0;
	cfg.voiceGain			=	36;//(v-40)*0.5db
	cfg.voiceEncodeMode		=	VOICE_ENCODE_BYP;
	cfg.voiceRate			=	VOICE_RATE_16K;
	cfg.voiceAutoMuteOnOff	=	1;

	volatile	int voiceConfigStaus	=	hal_voice_config(cfg, voice_evt_handler_adpcm);

	if(voiceConfigStaus){
		return;
	}
}

void	voiceCaptureStart(void)
{
	hal_clk_reset(MOD_ADCC);
	hal_clk_reset(MOD_VOC);
	voiceCaputerInit();
	LC_Mic_Sample_Busy	=	1;
	disable_normal_adc_interrupt();
	hal_voice_start();
}

void	voiceCaptureStop(void)
{
	disable_voice_adc_interrupt();
	hal_voice_stop();
	disable_voice_adc_interrupt();
	LC_Mic_Sample_Busy	=	0;
	hal_clk_reset(MOD_ADCC);
	hal_clk_reset(MOD_VOC);
	WaitMs(1);
	hal_voice_clear();
	enable_normal_adc_interrupt();
}
/*!
 *	@fn			LC_Mic_Sample_Start 
 *	@brief		Start Mic sampling events. 
 *	@param[in]	none.
 *	@retrurn	none.
 */
void	LC_Mic_Sample_Start(void)
{
#if(LC_RGBLight_Mic_Enable == 1)
//	if((LC_RGBLight_Param.RGB_Light_Mode != RGB_Mic_Mode_Classical) && \
//		(LC_RGBLight_Param.RGB_Light_Mode != RGB_Mic_Mode_Soft) && \
//		(LC_RGBLight_Param.RGB_Light_Mode != RGB_Mic_Mode_Beats) && \
//		(LC_RGBLight_Param.RGB_Light_Mode != RGB_Mic_Mode_Disco)){
		osal_start_timerEx(LC_Mic_Sample_TaskID, MIC_EVENT_LEVEL1, 800);
		LC_Mic_RGBData_Speed	=	0;
//	}
#endif
}
void	LC_Mic_Sample_Stop(void)
{
#if(LC_RGBLight_Mic_Enable == 1)
//	if((LC_RGBLight_Param.RGB_Light_Mode == RGB_Mic_Mode_Classical) || \
//		(LC_RGBLight_Param.RGB_Light_Mode == RGB_Mic_Mode_Soft) || \
//		(LC_RGBLight_Param.RGB_Light_Mode == RGB_Mic_Mode_Beats) || \
//		(LC_RGBLight_Param.RGB_Light_Mode == RGB_Mic_Mode_Disco)){
		osal_set_event(LC_Mic_Sample_TaskID, MIC_EVENT_LEVEL2);
		LC_Mic_RGBData_Speed	=	0;
//	}
#endif
}

/*!
 *	@fn			LC_Mic_Sample_Task_Init 
 *	@brief		Initialize function for the Mic_Sample Task. 
 *	@param[in]	task_id			:the ID assigned by OSAL,
 *								used to send message and set timer.
 *	@retrurn	none.
 */
void	LC_Mic_Sample_Task_Init(uint8 task_id)
{
	LC_Mic_Sample_TaskID	=	task_id;
	hal_voice_init();
}
/*!
 *	@fn			LC_Mic_Sample_ProcessEvent
 *	@brief		LC_Mic_Sample Task event processor.This function
 *				is called to processs all events for the task.Events
 *				include timers,messages and any other user defined events.
 *	@param[in]	task_id			:The OSAL assigned task ID.
 *	@param[in]	events			:events to process.This is a bit map and can
 *									contain more than one event.
 */
uint16	LC_Mic_Sample_ProcessEvent(uint8 task_id, uint16 events)
{
	VOID task_id;	// OSAL required parameter that isn't used in this function
	if(events & SYS_EVENT_MSG){
		uint8	*pMsg;
		if((pMsg = osal_msg_receive(LC_Mic_Sample_TaskID)) != NULL){
			LC_Common_ProcessOSALMsg((osal_event_hdr_t *)pMsg);
            // Release the OSAL message
			VOID osal_msg_deallocate(pMsg);
		}
		return(events ^ SYS_EVENT_MSG);
	}
	if(events & MIC_EVENT_LEVEL1){
		for(uint16 i = 0;i<MAX_VOICE_BUF_SIZE;i++){
			voiceLeftBuf[i]	=	0;
		}
		voiceCaptureStart();
		LC_Mic_Sample_Enable	=	1;
		return(events ^ MIC_EVENT_LEVEL1);
	}
	if(events & MIC_EVENT_LEVEL2){
		for(uint16 i = 0;i<MAX_VOICE_BUF_SIZE;i++){
			voiceLeftBuf[i]	=	0;
		}
		voiceCaptureStop();
		return(events ^ MIC_EVENT_LEVEL2);
	}

	return 0;
}
/** @}*/
 
