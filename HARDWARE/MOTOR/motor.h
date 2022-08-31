/**************************************************
	* @File Name: motor.h
	* @brief 电机控制相关变量宏定义
	* @author 王现刚 (2891854535@qq.com)
	* @Version : 1.0
	* @date 2022-08-29
	* 
***************************************************/
#ifndef __MOTOR_H
#define __MOTOR_H

#include "sys.h"
#include "stdlib.h"	


/********** 驱动器 端口定义 **************
//DRIVER_DIR   PA4 (DCMI_D6)
//DRIVER_OE    PE6 (DCMI_D7)
//STEP_PULSE   PC7 (TIM8_CH2  DCMI_D1)
******************************************/
#define DRIVER_DIR       PAout(4) // 旋转方向
#define DRIVER_OE        PAout(6) // 使能脚 低电平有效 
#define RCR_VAL          255      //每计数（RCR_VAL+1）次，中断一次，这个值（0~255）设置大一些可以降低中断频率
#define DRIVER_RST_S     2000     //电机复位的速度
//#define DRIVER_PRESS_FRE 800      //电机每分钟按压频率调节,此值为800时为每分钟按压60次
#define DRIVER_PRESS_FRE 1500     //电机每分钟按压频率调节,此值为11500时为每分钟按压110加减3次
typedef enum
{
	CW = 1,//高电平顺时针
	CCW = 0,//低电平逆时针
}DIR_Type;//运行方向

extern long target_pos;//有符号方向
extern long current_pos;//有符号方向

void Driver_Init(void);//驱动器初始化
void TIM8_OPM_RCR_Init(u16 arr,u16 psc);//TIM8_CH2 单脉冲输出+重复计数功能初始化
void TIM8_Startup(u32 frequency);   //启动定时器8
void Locate_Rle(long num,u32 frequency,DIR_Type dir); //相对定位函数
void Locate_Abs(long num,u32 frequency);//绝对定位函数
void rst_driver(void);

void ctr_driver(uint8_t mode);
#endif





