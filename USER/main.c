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

//�������ȼ�
#define START_TASK_PRIO				3
//�����ջ��С	
#define START_STK_SIZE 				128
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
extern void start_task(void *p_arg);



USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;



int main(void)
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
	
	motor_init(500-1,84-1);     //���2kHZ��PWM��,�����������
	TIM_SetCompare2(TIM3,100);  //�͵�ƽʱ��ռ���֮һ

	
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
	OSInit(&err);		    //��ʼ��UCOSIII
	OS_CRITICAL_ENTER();    //�����ٽ���
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
	while(1);
}


