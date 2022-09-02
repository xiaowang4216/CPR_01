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


//�������ȼ�
#define START_TASK_PRIO				3
//�����ջ��С	
#define START_STK_SIZE 				128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);


//TOUCH����
//�����������ȼ�
#define TOUCH_TASK_PRIO				4
//�����ջ��С
#define TOUCH_STK_SIZE				128
//������ƿ�
OS_TCB TouchTaskTCB;
//�����ջ
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch����
void touch_task(void *p_arg);

//��������
//�����������ȼ�
#define KEY_TASK_PRIO 				5
//�����ջ��С
#define KEY_STK_SIZE				128
//������ƿ�
OS_TCB KeyTaskTCB;
//�����ջ
CPU_STK KEY_TASK_STK[KEY_STK_SIZE];
//led0����
void key_task(void *p_arg);

//LED0����
//�����������ȼ�
#define LED0_TASK_PRIO 				12
//�����ջ��С
#define LED0_STK_SIZE				128
//������ƿ�
OS_TCB Led0TaskTCB;
//�����ջ
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
//led0����
void led0_task(void *p_arg);

//�ֿ��������
//�����������ȼ�
#define FONTUPDATA_TASK_PRIO		7
//�����ջ��С
#define FONTUPDATA_STK_SIZE			128
//������ƿ�
OS_TCB FontupdataTaskTCB;
//�����ջ
CPU_STK FONTUPDATA_TASK_STK[FONTUPDATA_STK_SIZE];
//�ֿ��������
void fontupdata_task(void *p_arg);

//EMWINDEMO����
//�����������ȼ�
#define EMWINDEMO_TASK_PRIO			8
//�����ջ��С
#define EMWINDEMO_STK_SIZE			1024
//������ƿ�
OS_TCB EmwindemoTaskTCB;
//�����ջ
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_task����
void emwindemo_task(void *p_arg);

//USB���U������
//�����������ȼ�
#define USB_TASK_PRIO			6
//�����ջ��С
#define USB_STK_SIZE			512
//������ƿ�
OS_TCB USBTaskTCB;
//�����ջ
CPU_STK USB_TASK_STK[USB_STK_SIZE];
//emwindemo_task����
void usb_task(void *p_arg);

//���ݴ�������
#define MSGTR_TASK_PRIO             10
//�����ջ��С
#define MSGTR_STK_SIZE			256
static  OS_TCB   MSGTRTaskTCB;
static  CPU_STK  MSGTR_TASK_STK[MSGTR_STK_SIZE];
static void msgtr_task(void *p_arg);

//�������
#define MOTOR_TASK_PRIO             9
//�����ջ��С
#define MOTOR_STK_SIZE			256
static  OS_TCB   MOTORTaskTCB;
static  CPU_STK  MOTOR_TASK_STK[MOTOR_STK_SIZE];
static void motor_task(void *p_arg);

/*
*******************************************************************************************************
*                               ����
*******************************************************************************************************
*/
/* USB��ر��� */
USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;

OS_SEM       AppPrintfSemp;  /* ����printf���� */
OS_SEM       SEM_SYNCH;	     /* ����ͬ�� */
OS_SEM       SEM_DRIVER_CTR; /* ���ڵ������ */

extern uint8_t motor_sta;    /* ���ڻ�ȡ��ǰ�����ִ��״̬ */
extern int     mode_sec;     /* ���ڻ�ȡ��ǰ��ģʽ */
extern int     U_Disk_Flag;
//���������ź���
static  void  AppObjCreate (void);


//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//����CRCʱ��
	GUI_Init();  			//STemWin��ʼ��
	WM_SetCreateFlags(WM_CF_MEMDEV); //�������д��ڵĴ洢�豸
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	AppObjCreate();
	//STemWin Demo����	
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
	//����������
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
	//LED0����
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
	//�ֿ��������
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
	//��������
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
	//USB����
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
	/**************����MsgPro����*********************/
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
	/**************����MsgSD����*********************/
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
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
}


/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨѶ
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{
	OS_ERR      err;
	
	/* 
	   �����ź�����ֵΪ1��ʱ�����ʵ�ֻ��⹦�ܣ�Ҳ����ֻ��һ����Դ����ʹ�� 
	   �������ǽ�����1�Ĵ�ӡ������Ϊ��������Դ����ֹ���ڴ�ӡ��ʱ������������ռ
	   ��ɴ��ڴ�ӡ���ҡ�
	*/
	OSSemCreate((OS_SEM    *)&AppPrintfSemp,
				(CPU_CHAR  *)"AppPrintfSemp",
				(OS_SEM_CTR )1,
				(OS_ERR    *)&err);
	
	/* ��������ֵΪ0������ʵ������ͬ������ */
	OSSemCreate((OS_SEM    *)&SEM_SYNCH,
				(CPU_CHAR  *)"SEM_SYNCH",
				(OS_SEM_CTR )0,
				(OS_ERR    *)&err);
	/* ��������ֵΪ0������ʵ��ϵͳ���Ƶ������ */
	OSSemCreate((OS_SEM    *)&SEM_DRIVER_CTR,
				(CPU_CHAR  *)"SEM_DRIVER_CTR",
				(OS_SEM_CTR )0,
				(OS_ERR    *)&err);
}

/*
*********************************************************************************************************
*	�� �� ��: App_Printf
*	����˵��: �̰߳�ȫ��printf��ʽ		  			  
*	��    ��: ͬprintf�Ĳ�����
*             ��C�У����޷��г����ݺ���������ʵ�ε����ͺ���Ŀʱ,������ʡ�Ժ�ָ��������
*	�� �� ֵ: ��
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

	/* ������� */
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


//EMWINDEMO����
void emwindemo_task(void *p_arg)
{
	/* ��ʾ��� */
	GUI_CURSOR_Show();
	/* ��ʾ������ */
	MainTask();
	while(1)
	{
		GUI_Delay(10);
	}
}

//�ֿ��������
//KEY_UP������2s�����ֿ�
void fontupdata_task(void *pdata)
{
	OS_ERR err;
	while(1)
	{
		if(WK_UP == 1)				//KEY_UP������
		{
			OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_PERIODIC,&err);//��ʱ2s
			if(WK_UP == 1)			//����KEY_UP��
			{
				LCD_Clear(WHITE);
				OSSchedLock(&err);		//����������
				LCD_ShowString(10,50,250,30,16,"Font Updataing,Please Wait...");
				update_font(10,70,16,"0:");//�����ֿ�
				LCD_Clear(WHITE);
				POINT_COLOR = RED;
				LCD_ShowString(10,50,280,30,16,"Font Updata finshed,Please Restart!");
				OSSchedUnlock(&err);	//����������
			}
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//��ʱ10ms
	}
}

//������������
void key_task(void *pdata)
{
	u8       key;
	OS_ERR   err;
	double temp_val;
	while(1)
	{
		/* ѭ����ⰴ��ֵ */
		key = KEY_Scan(0);
		switch(key)
		{
			/* KEY0���� */
			case KEY0_PRES:
				//��ȡ��ǰ��ѹֵ
				temp_val = Get_Adc_Average(3,ADC_Channel_4,10);
				//ת����ѹֵ
				temp_val = (double)temp_val*(3.3/4096);		

				App_Printf("%.4f",temp_val);
			break;
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);//��ʱ10ms
	}
}

//TOUCH����
void touch_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		/* ������� */
		GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//��ʱ5ms
	}
}

//LED0����
void led0_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		/* ��ɫLED��˸ */
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);//��ʱ500ms
	}
}

//USB��������
void usb_task(void *p_arg)
{
	OS_ERR err;
	while(1)
	{
		/* ѭ�����USB״̬ */
		USBH_Process(&USB_OTG_Core, &USB_Host);
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_PERIODIC,&err);//��ʱ10ms
	}
}

/*
*********************************************************************************************************
*	�� �� ��: msgtr_task
*	����˵��: Ŀǰ�����ǵȴ����������ť,����ť����ʱ,����һ��������־
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 3
*********************************************************************************************************
*/	
static void msgtr_task(void *p_arg)
{
	OS_ERR      err;
	
	(void)p_arg;
	  
	while(1)
	{	
		 /* �ȴ���ȡ�ź���ͬ����Ϣ�����յ���ִ�д��ڴ�ӡ */
		OSSemPend((OS_SEM *)&SEM_SYNCH,
				  (OS_TICK )0,
				  (OS_OPT  )OS_OPT_PEND_BLOCKING,
				  (CPU_TS  )0,
				  (OS_ERR *)&err);			
		write_log(2);
		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);//��ʱ50ms			
	}   
}

/*
*********************************************************************************************************
*	�� �� ��: motor_task
*	����˵��: ���ڵ�����Ƶ��߳�
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 9
*********************************************************************************************************
*/	
static void motor_task(void *p_arg)
{	
	OS_ERR      err;
	
	(void)p_arg;
	
	while(1)
	{			
		if(motor_sta == 1)       //����״̬
		{
			ctr_driver(mode_sec);
			OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_PERIODIC,&err);
		}
		else if(motor_sta == 0 || motor_sta == 2)  //ֹͣ������״̬
		{
			get_driver_start(); //��ȡ�˹����õĳ�ʼλ��
		}
		else
		{
			ctr_driver(3);
			/* ��ʱ500ms */
			OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);	
		}					  	 	       											  
	}   
}

/**************************************************
	* 
	* @brief ϵͳ��ʼ������
	* 
***************************************************/
void my_system_init(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init(168);       	//��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//�жϷ�������
	uart_init(115200);    	//���ڲ���������
	LCD_Init();			    //��ʼ��LCD
	LCD_Clear(GRAYBLUE);
	W25QXX_Init();			//��ʼ��W25Q128
	LED_Init();   			//LED��ʼ��
	KEY_Init();				//������ʼ��
	ADC1_Init();            //��ص����ɼ�
	ADC3_Init();            //��ѹ��Ȳɼ�
	My_RTC_Init();          //RTC��ʼ��	
	_system_start(20); 

	
	FSMC_SRAM_Init(); 		//SRAM��ʼ��	
	mem_init(SRAMIN); 		//�ڲ�RAM��ʼ��
	mem_init(SRAMEX); 		//�ⲿRAM��ʼ��
	mem_init(SRAMCCM);		//CCM��ʼ��
	
	exfuns_init();			//Ϊfatfs�ļ�ϵͳ�����ڴ�
	f_mount(fs[0],"0:",1);	//����SD��
	f_mount(fs[1],"1:",1);	//����FLASH
	
	_system_start(40);        //ϵͳ������ʾ

	
	while(SD_Init())	    //���SD��
	{
		LCD_ShowString(30,90,200,16,16,"SD Card Failed!");
		delay_ms(200);
		LCD_Fill(30,90,200+30,70+16,WHITE);
		delay_ms(200);		    
	}
	_system_start(60); 
	while(font_init())		//��ʼ���ֿ�
	{
		LCD_ShowString(30,70,200,16,16,"Font Error!");
		update_font(30,90,16,"0:");	//����ֿⲻ���ھ͸����ֿ�
		delay_ms(2000);
		LCD_Clear(WHITE);	//����
		break;
	}
	_system_start(80); 
	TP_Init();			    //��ʼ��������	
	Driver_Init();			//��������ʼ��	
	TIM8_OPM_RCR_Init(999,168-1); //1MHz����Ƶ��  ������+�ظ�����ģʽ 
	OSInit(&err);		    //��ʼ��UCOSIII
	OS_CRITICAL_ENTER();    //�����ٽ���
	rst_driver();
	//��ʼ��USB����
	USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_cb); 
	_system_start(100); 
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
}


