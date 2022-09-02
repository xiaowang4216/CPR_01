#include "adc.h"
#include "delay.h"
#include "DIALOG.h"
#include "display.h"
#include "GUI.h"
#include "EmWinHZFont.h"
#include "exfuns.h"
#include "ff.h"
#include "fontupd.h"
#include "includes.h"
#include "key.h"
#include "led.h"
#include "log.h"
#include "main_menu.h"
#include "malloc.h"
#include "motor.h"
#include "mytext.h"
#include "os_control.h"
#include "rtc.h"
#include "sdio_sdcard.h"
#include "sram.h"
#include "sys.h"
#include "tftlcd.h"
#include "timer.h"
#include "touch.h"
#include "usart.h"
#include "usbh_usr.h" 
#include "w25qxx.h"
#include "WM.h"


//任务优先级
#define START_TASK_PRIO				3
//任务堆栈大小	
#define START_STK_SIZE 				128
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);


//TOUCH任务
//设置任务优先级
#define TOUCH_TASK_PRIO				4
//任务堆栈大小
#define TOUCH_STK_SIZE				128
//任务控制块
OS_TCB TouchTaskTCB;
//任务堆栈
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch任务
void touch_task(void *p_arg);

//按键任务
//设置任务优先级
#define KEY_TASK_PRIO 				5
//任务堆栈大小
#define KEY_STK_SIZE				128
//任务控制块
OS_TCB KeyTaskTCB;
//任务堆栈
CPU_STK KEY_TASK_STK[KEY_STK_SIZE];
//led0任务
void key_task(void *p_arg);

//LED0任务
//设置任务优先级
#define LED0_TASK_PRIO 				12
//任务堆栈大小
#define LED0_STK_SIZE				128
//任务控制块
OS_TCB Led0TaskTCB;
//任务堆栈
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
//led0任务
void led0_task(void *p_arg);

//字库更新任务
//设置任务优先级
#define FONTUPDATA_TASK_PRIO		7
//任务堆栈大小
#define FONTUPDATA_STK_SIZE			128
//任务控制块
OS_TCB FontupdataTaskTCB;
//任务堆栈
CPU_STK FONTUPDATA_TASK_STK[FONTUPDATA_STK_SIZE];
//字库更新任务
void fontupdata_task(void *p_arg);

//EMWINDEMO任务
//设置任务优先级
#define EMWINDEMO_TASK_PRIO			8
//任务堆栈大小
#define EMWINDEMO_STK_SIZE			1024
//任务控制块
OS_TCB EmwindemoTaskTCB;
//任务堆栈
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_task任务
void emwindemo_task(void *p_arg);

//USB检测U盘任务
//设置任务优先级
#define USB_TASK_PRIO			6
//任务堆栈大小
#define USB_STK_SIZE			512
//任务控制块
OS_TCB USBTaskTCB;
//任务堆栈
CPU_STK USB_TASK_STK[USB_STK_SIZE];
//emwindemo_task任务
void usb_task(void *p_arg);

//数据传输任务
#define MSGTR_TASK_PRIO             10
//任务堆栈大小
#define MSGTR_STK_SIZE			256
static  OS_TCB   MSGTRTaskTCB;
static  CPU_STK  MSGTR_TASK_STK[MSGTR_STK_SIZE];
static void msgtr_task(void *p_arg);

//电机控制
#define MOTOR_TASK_PRIO             9
//任务堆栈大小
#define MOTOR_STK_SIZE			256
static  OS_TCB   MOTORTaskTCB;
static  CPU_STK  MOTOR_TASK_STK[MOTOR_STK_SIZE];
static void motor_task(void *p_arg);

/*
*******************************************************************************************************
*                               变量
*******************************************************************************************************
*/
/* USB相关变量 */
USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

OS_SEM       AppPrintfSemp;  /* 用于printf互斥 */
OS_SEM       SEM_SYNCH;	     /* 用于同步 */
OS_SEM       SEM_DRIVER_CTR; /* 用于电机控制 */

extern uint8_t motor_sta;    /* 用于获取当前电机的执行状态 */
extern int     mode_sec;     /* 用于获取当前的模式 */
extern int     U_Disk_Flag;
//创建任务信号量
static  void  AppObjCreate (void);


//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//开启CRC时钟
	GUI_Init();  			//STemWin初始化
	WM_SetCreateFlags(WM_CF_MEMDEV); //启动所有窗口的存储设备
	
	OS_CRITICAL_ENTER();	//进入临界区
	AppObjCreate();
	//STemWin Demo任务	
	OSTaskCreate((OS_TCB*     )&EmwindemoTaskTCB,		
				 (CPU_CHAR*   )"Emwindemo task", 		
                 (OS_TASK_PTR )emwindemo_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )EMWINDEMO_TASK_PRIO,     
                 (CPU_STK*    )&EMWINDEMO_TASK_STK[0],	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE/10,	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	//触摸屏任务
	OSTaskCreate((OS_TCB*     )&TouchTaskTCB,		
				 (CPU_CHAR*   )"Touch task", 		
                 (OS_TASK_PTR )touch_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )TOUCH_TASK_PRIO,     
                 (CPU_STK*    )&TOUCH_TASK_STK[0],	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE/10,	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);			 
	//LED0任务
	OSTaskCreate((OS_TCB*     )&Led0TaskTCB,		
				 (CPU_CHAR*   )"Led0 task", 		
                 (OS_TASK_PTR )led0_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )LED0_TASK_PRIO,     
                 (CPU_STK*    )&LED0_TASK_STK[0],	
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED0_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);	
	//字库更新任务
	OSTaskCreate((OS_TCB*     )&FontupdataTaskTCB,		
				 (CPU_CHAR*   )"Fontupdata task", 		
                 (OS_TASK_PTR )fontupdata_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )FONTUPDATA_TASK_PRIO,     
                 (CPU_STK*    )&FONTUPDATA_TASK_STK[0],	
                 (CPU_STK_SIZE)FONTUPDATA_STK_SIZE/10,	
                 (CPU_STK_SIZE)FONTUPDATA_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	//按键任务
	OSTaskCreate((OS_TCB*     )&KeyTaskTCB,		
				 (CPU_CHAR*   )"Key task", 		
                 (OS_TASK_PTR )key_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )KEY_TASK_PRIO,     
                 (CPU_STK*    )&KEY_TASK_STK[0],	
                 (CPU_STK_SIZE)KEY_STK_SIZE/10,	
                 (CPU_STK_SIZE)KEY_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);	
	//USB任务
	OSTaskCreate((OS_TCB*     )&USBTaskTCB,		
				 (CPU_CHAR*   )"USB task", 		
                 (OS_TASK_PTR )usb_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )USB_TASK_PRIO,     
                 (CPU_STK*    )&USB_TASK_STK[0],	
                 (CPU_STK_SIZE)USB_STK_SIZE/10,	
                 (CPU_STK_SIZE)USB_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	/**************创建MsgPro任务*********************/
	OSTaskCreate((OS_TCB     *)&MSGTRTaskTCB,             
                 (CPU_CHAR   *)"Msgtr task",
                 (OS_TASK_PTR )msgtr_task, 
                 (void       *)0,
                 (OS_PRIO     )MSGTR_TASK_PRIO,
                 (CPU_STK    *)&MSGTR_TASK_STK[0],
                 (CPU_STK_SIZE)MSGTR_STK_SIZE / 10,
                 (CPU_STK_SIZE)MSGTR_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
	/**************创建MsgSD任务*********************/
	OSTaskCreate((OS_TCB     *)&MOTORTaskTCB,             
                 (CPU_CHAR   *)"Motor task",
                 (OS_TASK_PTR )motor_task, 
                 (void       *)0,
                 (OS_PRIO     )MOTOR_TASK_PRIO,
                 (CPU_STK    *)&MOTOR_TASK_STK[0],
                 (CPU_STK_SIZE)MOTOR_STK_SIZE / 10,
                 (CPU_STK_SIZE)MOTOR_STK_SIZE,
                 (OS_MSG_QTY  )0,
                 (OS_TICK     )0,
                 (void       *)0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//退出临界区
}


/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通讯
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{
	OS_ERR      err;
	
	/* 
	   创建信号量数值为1的时候可以实现互斥功能，也就是只有一个资源可以使用 
	   本例程是将串口1的打印函数作为保护的资源。防止串口打印的时候被其它任务抢占
	   造成串口打印错乱。
	*/
	OSSemCreate((OS_SEM    *)&AppPrintfSemp,
				(CPU_CHAR  *)"AppPrintfSemp",
				(OS_SEM_CTR )1,
				(OS_ERR    *)&err);
	
	/* 创建计数值为0，用于实现任务同步功能 */
	OSSemCreate((OS_SEM    *)&SEM_SYNCH,
				(CPU_CHAR  *)"SEM_SYNCH",
				(OS_SEM_CTR )0,
				(OS_ERR    *)&err);
	/* 创建计数值为0，用于实现系统控制电机功能 */
	OSSemCreate((OS_SEM    *)&SEM_DRIVER_CTR,
				(CPU_CHAR  *)"SEM_DRIVER_CTR",
				(OS_SEM_CTR )0,
				(OS_ERR    *)&err);
}

/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式		  			  
*	形    参: 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
*********************************************************************************************************
*/
void  App_Printf(CPU_CHAR *format, ...)
{
    CPU_CHAR  buf_str[80 + 1];
    va_list   v_args;
    OS_ERR    os_err;


    va_start(v_args, format);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) format,
                                  v_args);
    va_end(v_args);

	/* 互斥操作 */
    OSSemPend((OS_SEM  *)&AppPrintfSemp,
              (OS_TICK  )0u,
              (OS_OPT   )OS_OPT_PEND_BLOCKING,
              (CPU_TS  *)0,
              (OS_ERR  *)&os_err);

    printf("%s", buf_str);

   (void)OSSemPost((OS_SEM  *)&AppPrintfSemp,
                   (OS_OPT   )OS_OPT_POST_1,
                   (OS_ERR  *)&os_err);

}


//EMWINDEMO任务
void emwindemo_task(void *p_arg)
{
	/* 显示光标 */
	GUI_CURSOR_Show();
	/* 显示主界面 */
	MainTask();
	while(1)
	{
		GUI_Delay(10);
	}
}

//字库更新任务
//KEY_UP键长按2s更新字库
void fontupdata_task(void *pdata)
{
	OS_ERR err;
	while(1)
	{
		if(WK_UP == 1)				//KEY_UP键按下
		{
			OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_PERIODIC,&err);//延时2s
			if(WK_UP == 1)			//还是KEY_UP键
			{
				LCD_Clear(WHITE);
				OSSchedLock(&err);		//调度器上锁
				LCD_ShowString(10,50,250,30,16,"Font Updataing,Please Wait...");
				update_font(10,70,16,"0:");//更新字库
				LCD_Clear(WHITE);
				POINT_COLOR = RED;
				LCD_ShowString(10,50,280,30,16,"Font Updata finshed,Please Restart!");
				OSSchedUnlock(&err);	//调度器解锁
			}
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//延时10ms
	}
}

//按键处理任务
void key_task(void *pdata)
{
	u8       key;
	OS_ERR   err;
	double temp_val;
	while(1)
	{
		/* 循环检测按键值 */
		key = KEY_Scan(0);
		switch(key)
		{
			/* KEY0按下 */
			case KEY0_PRES:
				//获取当前电压值
				temp_val = Get_Adc_Average(3,ADC_Channel_4,10);
				//转换电压值
				temp_val = (double)temp_val*(3.3/4096);		

				App_Printf("%.4f",temp_val);
			break;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//延时10ms
	}
}

//TOUCH任务
void touch_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		/* 触摸检测 */
		GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//延时5ms
	}
}

//LED0任务
void led0_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		/* 红色LED闪烁 */
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//延时500ms
	}
}

//USB处理任务
void usb_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		/* 循环检测USB状态 */
		USBH_Process(&USB_OTG_Core, &USB_Host);
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//延时10ms
	}
}

/*
*********************************************************************************************************
*	函 数 名: msgtr_task
*	功能说明: 目前功能是等待点击蓝牙按钮,当按钮按下时,生成一条错误日志
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 3
*********************************************************************************************************
*/	
static void msgtr_task(void *p_arg)
{
	OS_ERR      err;
	
	(void)p_arg;
	  
	while(1)
	{	
		 /* 等待获取信号量同步消息，接收到后执行串口打印 */
		OSSemPend((OS_SEM *)&SEM_SYNCH,
				  (OS_TICK )0,
				  (OS_OPT  )OS_OPT_PEND_BLOCKING,
				  (CPU_TS  )0,
				  (OS_ERR *)&err);			
		write_log(2);
		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);//延时50ms			
	}   
}

/*
*********************************************************************************************************
*	函 数 名: motor_task
*	功能说明: 用于电机控制的线程
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 9
*********************************************************************************************************
*/	
static void motor_task(void *p_arg)
{	
	OS_ERR      err;
	
	(void)p_arg;
	
	while(1)
	{			
		if(motor_sta == 1)       //运行状态
		{
			ctr_driver(mode_sec);
			OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);
		}
		else if(motor_sta == 0 || motor_sta == 2)  //停止或运行状态
		{
			get_driver_start(); //获取人工设置的初始位置
		}
		else
		{
			ctr_driver(3);
			/* 延时500ms */
			OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);	
		}					  	 	       											  
	}   
}

/**************************************************
	* 
	* @brief 系统初始化函数
	* 
***************************************************/
void my_system_init(void)
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
	Driver_Init();			//驱动器初始化	
	TIM8_OPM_RCR_Init(999,168-1); //1MHz计数频率  单脉冲+重复计数模式 
	OSInit(&err);		    //初始化UCOSIII
	OS_CRITICAL_ENTER();    //进入临界区
	rst_driver();
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
}


