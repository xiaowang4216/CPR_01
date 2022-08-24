#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "sram.h"
#include "malloc.h"
#include "tftlcd.h"
#include "led.h"
#include "rtc.h"
#include "adc.h"
#include "key.h"
#include "adc.h"
#include "timer.h"
#include "touch.h"
#include "mytext.h"
#include "sdio_sdcard.h"
#include "GUI.h"
#include "ff.h"
#include "exfuns.h"
#include "w25qxx.h"
#include "fontupd.h"
#include "includes.h"
#include "EmWinHZFont.h"
#include "WM.h"
#include "DIALOG.h"
#include "usbh_usr.h" 
#include "fattester.h"
#include "pwm.h"
#include "display.h"

#include "os_control.h"

//任务优先级
#define START_TASK_PRIO				3
//任务堆栈大小	
#define START_STK_SIZE 				128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
extern void start_task(void *p_arg);



USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;



int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init(168);       	//延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//中断分组配置
	uart_init(115200);    	//串口波特率设置
	LCD_Init();			    //初始化LCD
	LCD_Clear(GRAYBLUE);
	W25QXX_Init();			//初始化W25Q128
	LED_Init();   			//LED初始化
	KEY_Init();				//按键初始化
	ADC1_Init();            //电池电量采集
	ADC3_Init();            //按压深度采集
	My_RTC_Init();          //RTC初始化	
	_system_start(20); 
	
	motor_init(500-1,84-1);     //输出2kHZ的PWM波,用于驱动电机
	TIM_SetCompare2(TIM3,100);  //低电平时间占五分之一

	
	FSMC_SRAM_Init(); 		//SRAM初始化	
	mem_init(SRAMIN); 		//内部RAM初始化
	mem_init(SRAMEX); 		//外部RAM初始化
	mem_init(SRAMCCM);		//CCM初始化
	
	exfuns_init();			//为fatfs文件系统分配内存
	f_mount(fs[0],"0:",1);	//挂载SD卡
	f_mount(fs[1],"1:",1);	//挂载FLASH
	
	_system_start(40);        //系统启动提示

	
	while(SD_Init())	    //检测SD卡
	{
		LCD_ShowString(30,90,200,16,16,"SD Card Failed!");
		delay_ms(200);
		LCD_Fill(30,90,200+30,70+16,WHITE);
		delay_ms(200);		    
	}
	_system_start(60); 
	while(font_init())		//初始化字库
	{
		LCD_ShowString(30,70,200,16,16,"Font Error!");
		update_font(30,90,16,"0:");	//如果字库不存在就更新字库
		delay_ms(2000);
		LCD_Clear(WHITE);	//清屏
		break;
	}
	_system_start(80); 
	TP_Init();			    //初始化触摸屏	
	OSInit(&err);		    //初始化UCOSIII
	OS_CRITICAL_ENTER();    //进入临界区
	//初始化USB主机
	USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_cb); 
	_system_start(100); 
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	while(1);
}


