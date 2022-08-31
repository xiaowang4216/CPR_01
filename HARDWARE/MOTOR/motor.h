/**************************************************
	* @File Name: motor.h
	* @brief ���������ر����궨��
	* @author ���ָ� (2891854535@qq.com)
	* @Version : 1.0
	* @date 2022-08-29
	* 
***************************************************/
#ifndef __MOTOR_H
#define __MOTOR_H

#include "sys.h"
#include "stdlib.h"	


/********** ������ �˿ڶ��� **************
//DRIVER_DIR   PA4 (DCMI_D6)
//DRIVER_OE    PE6 (DCMI_D7)
//STEP_PULSE   PC7 (TIM8_CH2  DCMI_D1)
******************************************/
#define DRIVER_DIR       PAout(4) // ��ת����
#define DRIVER_OE        PAout(6) // ʹ�ܽ� �͵�ƽ��Ч 
#define RCR_VAL          255      //ÿ������RCR_VAL+1���Σ��ж�һ�Σ����ֵ��0~255�����ô�һЩ���Խ����ж�Ƶ��
#define DRIVER_RST_S     2000     //�����λ���ٶ�
//#define DRIVER_PRESS_FRE 800      //���ÿ���Ӱ�ѹƵ�ʵ���,��ֵΪ800ʱΪÿ���Ӱ�ѹ60��
#define DRIVER_PRESS_FRE 1500     //���ÿ���Ӱ�ѹƵ�ʵ���,��ֵΪ11500ʱΪÿ���Ӱ�ѹ110�Ӽ�3��
typedef enum
{
	CW = 1,//�ߵ�ƽ˳ʱ��
	CCW = 0,//�͵�ƽ��ʱ��
}DIR_Type;//���з���

extern long target_pos;//�з��ŷ���
extern long current_pos;//�з��ŷ���

void Driver_Init(void);//��������ʼ��
void TIM8_OPM_RCR_Init(u16 arr,u16 psc);//TIM8_CH2 ���������+�ظ��������ܳ�ʼ��
void TIM8_Startup(u32 frequency);   //������ʱ��8
void Locate_Rle(long num,u32 frequency,DIR_Type dir); //��Զ�λ����
void Locate_Abs(long num,u32 frequency);//���Զ�λ����
void rst_driver(void);

void ctr_driver(uint8_t mode);
#endif





