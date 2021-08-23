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

#ifndef		LC_MIC_SAMPLE_H_
#define		LC_MIC_SAMPLE_H_
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
#define		MAX_VOICE_BUF_SIZE		512

//	Mic Mode Threshold
#define		Mic_Mode_Classical		250
#define		Mic_Mode_Soft			150
#define		Mic_Mode_Beats			150
#define		Mic_Mode_Disco			200
/*------------------------------------------------------------------*/
/*						UI Task Events definitions					*/
/*------------------------------------------------------------------*/
#define		MIC_EVENT_LEVEL1		0x0001
#define		MIC_EVENT_LEVEL2		0x0002
#define		MIC_EVENT_LEVEL3		0x0004
/*------------------------------------------------------------------*/
/*						Data structures								*/
/*------------------------------------------------------------------*/

/*------------------------------------------------------------------*/
/*						external variables							*/
/*------------------------------------------------------------------*/
extern	uint8	LC_Mic_Sample_TaskID;
extern	uint8	LC_Mic_Sample_Busy;

/*------------------------------------------------------------------*/
/*						User function prototypes					*/
/*------------------------------------------------------------------*/
void	LC_Mic_Sample_Start				(void			);
void	LC_Mic_Sample_Stop				(void			);
void	LC_Mic_Sample_Task_Init			(uint8 task_id	);
uint16	LC_Mic_Sample_ProcessEvent		(uint8 task_id, uint16 events);
#ifdef	__cplusplus
	}
#endif

#endif	/**	LC_Mic_Sample.h **/
/**	@}*/
