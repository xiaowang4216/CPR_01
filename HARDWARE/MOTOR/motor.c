/**************************************************
	* @File Name: motor.c
	* @brief ���������غ�������
	* @author ���ָ� (2891854535@qq.com)
	* @Version : 1.2
	* @date 2022-08-29
	* V1.1 �޸ĸ�λ����,��ֹ�˹���Ԥϵͳ�����ǵ���ĳ�ʼλ��
	* V1.2 ����˹����õ����ʼλ�ù���
***************************************************/
#include "adc.h"
#include "delay.h"
#include "tftlcd.h"
#include "includes.h"
#include "log.h"
#include "motor.h"
#include "os_control.h"
#include "usart.h"


/********** ������ �˿ڶ��� **************
//DRIVER_DIR   PA4 (DCMI_D6)
//DRIVER_OE    PA6 (DCMI_D7)
//STEP_PULSE   PC7 (TIM8_CH2  DCMI_D1)
******************************************/

/**************************************************
	* 
	* @brief ��������
	* 
***************************************************/
u8       first_rst_flag = 1; //�Ƿ��ܳɹ���λ�ı�־
u8       rcr_remainder;      //�ظ�������������
u8       is_rcr_finish  = 1; //�ظ��������Ƿ��������
long     rcr_integer;	     //�ظ�������������
long     target_pos     = 0; //�з��ŷ���
long     current_pos    = 0; //�з��ŷ���
DIR_Type motor_dir      = CW;//˳ʱ��
int      depth_start    = 0; /* ���ڻ�ȡ��ǰ�˹����õ���ʼλ�� */
extern   int      depth_val; /* ���ڻ�ȡ��ǰ�İ�ѹ��� */
extern   uint8_t  motor_sta; /* ���ڻ�ȡ��ǰ�İ�ѹ״̬ */

int count  = 6;              /* �Զ���İ�ѹ��ɱ�־ */

/************** �����������ź��߳�ʼ�� ****************/
void Driver_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //ʹ��GPIOEʱ��
 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4|GPIO_Pin_6; //DRIVER_DIR DRIVER_OE��Ӧ����
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;    //��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;     //����
	GPIO_Init(GPIOA, &GPIO_InitStructure);            //��ʼ��GPIOA4,6
	
	GPIO_SetBits(GPIOA,GPIO_Pin_4);  //PA4����� ˳ʱ�뷽��  DRIVER_DIR
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);//PA6����� ʹ�����  DRIVER_OE
}
/***********************************************
//TIM8_CH2(PC7) ���������+�ظ��������ܳ�ʼ��
//TIM8 ʱ��Ƶ�� 84*2=168MHz
//arr:�Զ���װֵ
//psc:ʱ��Ԥ��Ƶ��
************************************************/
void TIM8_OPM_RCR_Init(u16 arr,u16 psc)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);  	//TIM8ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 	//ʹ��PORTCʱ��	                                                                     	

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8);   //GPIOC7����Ϊ��ʱ��8
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;         //GPIOC7
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;       //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;     //����
	GPIO_Init(GPIOC,&GPIO_InitStructure);               //��ʼ��PC7
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ   
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ClearITPendingBit(TIM8,TIM_IT_Update);

	TIM_UpdateRequestConfig(TIM8,TIM_UpdateSource_Regular); /********* ����ֻ�м��������Ϊ�����ж� ********/
	TIM_SelectOnePulseMode(TIM8,TIM_OPMode_Single);/******* ������ģʽ **********/
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����2ʹ��
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; /****** �Ƚ����2Nʧ�� *******/
	TIM_OCInitStructure.TIM_Pulse = arr>>1; //���ô�װ�벶��ȽϼĴ���������ֵ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);  //����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx

	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH2Ԥװ��ʹ��	 
	TIM_ARRPreloadConfig(TIM8, ENABLE); //ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	
	TIM_ITConfig(TIM8, TIM_IT_Update ,ENABLE);  //TIM8   ʹ�ܻ���ʧ��ָ����TIM�ж�
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;  //TIM8�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	TIM_ClearITPendingBit(TIM8, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ
	TIM_Cmd(TIM8, ENABLE);  //ʹ��TIM8											  
}
/******* TIM8�����жϷ������ *********/
void TIM8_UP_TIM13_IRQHandler(void)
{
	OSIntEnter();                                   //���ж�
	if(TIM_GetITStatus(TIM8,TIM_FLAG_Update)!=RESET)//�����ж�
	{
		TIM_ClearITPendingBit(TIM8,TIM_FLAG_Update);//��������жϱ�־λ		
		if(is_rcr_finish==0)//�ظ�������δ�������
		{
			if(rcr_integer!=0) //�����������廹δ�������
			{
				TIM8->RCR=RCR_VAL;//�����ظ�����ֵ
				rcr_integer--;//����RCR_VAL+1������				
			}else if(rcr_remainder!=0)//������������ ��λ0
			{
				TIM8->RCR=rcr_remainder-1;//������������
				rcr_remainder=0;//����
				is_rcr_finish=1;//�ظ��������������				
			}else goto out;   //rcr_remainder=0��ֱ���˳�			 
			TIM_GenerateEvent(TIM8,TIM_EventSource_Update);//����һ�������¼� ���³�ʼ��������
			TIM_CtrlPWMOutputs(TIM8,ENABLE);	//MOE �����ʹ��	
			TIM_Cmd(TIM8, ENABLE);  //ʹ��TIM8			
			if(motor_dir==CW) //�������Ϊ˳ʱ��   
				current_pos+=(TIM8->RCR+1);//�����ظ�����ֵ
			else          //������Ϊ��ʱ��
				current_pos-=(TIM8->RCR+1);//��ȥ�ظ�����ֵ			
		}else
		{
out:		is_rcr_finish=1;//�ظ��������������
			TIM_CtrlPWMOutputs(TIM8,DISABLE);	//MOE ������ر�
			TIM_Cmd(TIM8, DISABLE);  //�ر�TIM8				
			count = 0;
		}	
	}
	OSIntExit();                     //���ж�
}
/***************** ����TIM8 *****************/
void TIM8_Startup(u32 frequency)   //������ʱ��8
{
	u16 temp_arr=1000000/frequency-1; 
	TIM_SetAutoreload(TIM8,temp_arr);//�趨�Զ���װֵ	
	TIM_SetCompare2(TIM8,temp_arr>>1); //ƥ��ֵ2������װֵһ�룬����ռ�ձ�Ϊ50%	
	TIM_SetCounter(TIM8,0);//����������
	TIM_Cmd(TIM8, ENABLE);  //ʹ��TIM8
}
/********************************************
//��Զ�λ���� 
//num 0��2147483647
//frequency: 20Hz~200KHz
//dir: CW(˳ʱ�뷽��)  CCW(��ʱ�뷽��)
*********************************************/
void Locate_Rle(long num,u32 frequency,DIR_Type dir) //��Զ�λ����
{
	if(num<=0) //��ֵС����0 ��ֱ�ӷ���
	{
		return;
	}
	if(TIM8->CR1&0x01)//��һ�����廹δ�������  ֱ�ӷ���
	{
		return;
	}
	if((frequency<20)||(frequency>200000))//����Ƶ�ʲ��ڷ�Χ�� ֱ�ӷ���
	{
		return;
	}
	motor_dir=dir;//�õ�����	
	DRIVER_DIR=motor_dir;//���÷���
	
	if(motor_dir==CW)//˳ʱ��
		target_pos=current_pos+num;//Ŀ��λ��
	else if(motor_dir==CCW)//��ʱ��
		target_pos=current_pos-num;//Ŀ��λ��
	
	rcr_integer=num/(RCR_VAL+1);//�ظ�������������
	rcr_remainder=num%(RCR_VAL+1);//�ظ�������������
	is_rcr_finish=0;//�ظ�������δ�������
	TIM8_Startup(frequency);//����TIM8
}
/********************************************
//���Զ�λ���� 
//num   -2147483648��2147483647
//frequency: 20Hz~200KHz
*********************************************/
void Locate_Abs(long num,u32 frequency)                       //���Զ�λ����
{
	if(TIM8->CR1&0x01)                                        //��һ�����廹δ������� ֱ�ӷ���
	{
		return;
	}
	if((frequency<20)||(frequency>200000))                    //����Ƶ�ʲ��ڷ�Χ�� ֱ�ӷ���
	{
		return;
	}
	target_pos=num;                                           //����Ŀ��λ��
	if(target_pos!=current_pos)                               //Ŀ��͵�ǰλ�ò�ͬ
	{
		if(target_pos>current_pos)
			motor_dir=CW;                                     //˳ʱ��
		else
			motor_dir=CCW;                                    //��ʱ��
		DRIVER_DIR=motor_dir;                                 //���÷���
		
		rcr_integer=abs(target_pos-current_pos)/(RCR_VAL+1);  //�ظ�������������
		rcr_remainder=abs(target_pos-current_pos)%(RCR_VAL+1);//�ظ�������������
		is_rcr_finish=0;                                      //�ظ�������δ�������
		TIM8_Startup(frequency);                              //����TIM8
	}
}

/**************************************************
	* 
	* @brief �Ե�����и�λ����
	* 
***************************************************/
void rst_driver(void)
{
	static   uint8_t i = 1;
	u16      adcx;
	uint16_t rst_count = 0;                                   //��λʱ�����
	uint32_t frequency = DRIVER_RST_S;                        //��ȡ���õĵ����λ�ٶ�
	adcx=Get_Adc_Average(3,ADC_Channel_4,20);                 //��ȡͨ��4��ת��ֵ��20��ȡƽ��

	while(adcx > 5 && first_rst_flag)
	{
		rst_count ++;                                         
		if(rst_count >= 500)                                  //ʱ�䳬��50s,��Ϊ��λʧ��
		{
			write_log(3);                                     //д�븴λʧ�ܱ�����־
			i = 0;                                            //ϵͳ������λʧ��,���治�ù�
			first_rst_flag = 0;                               //��λʧ�ܱ�־,�ȴ���������
			break;
		}
		adcx=Get_Adc_Average(3,ADC_Channel_4,20);             //��ȡADC3ͨ��4��ת��ֵ��20��ȡƽ��
		target_pos=1;                                         //����Ŀ��λ��
		motor_dir=CCW;                                        //��ʱ��
		DRIVER_DIR=motor_dir;                                 //���÷���
		rcr_integer=abs(target_pos-current_pos)/(RCR_VAL+1);  //�ظ�������������
		rcr_remainder=abs(target_pos-current_pos)%(RCR_VAL+1);//�ظ�������������
		is_rcr_finish=0;                                      //�ظ�������δ�������
		TIM8_Startup(frequency);                              //����TIM8

	}
	TIM_CtrlPWMOutputs(TIM8,DISABLE);	                      //MOE ������ر�
	TIM_Cmd(TIM8, DISABLE);                                   //�ر�TIM8
	adcx=Get_Adc_Average(3,ADC_Channel_4,20);                 //��ȡͨ��4��ת��ֵ��20��ȡƽ��
	
	while(adcx < 5 && i)                                      //��֤ϵͳ������,����ĸ�λλ�ò����˹���Ԥ����
	{
		i = 0;
		count = 7;
		adcx=Get_Adc_Average(3,ADC_Channel_4,10);             //��ȡͨ��4��ת��ֵ��20��ȡƽ��
		Locate_Rle(100,frequency,CW);
		while(count);

	}
	rcr_integer = 0;
	rcr_remainder = 0;
	target_pos = 0;
	current_pos = 0;
	is_rcr_finish = 1;
	count = 7;

	TIM_CtrlPWMOutputs(TIM8,DISABLE);	                      //MOE ������ر�
	TIM_Cmd(TIM8, DISABLE);                                   //�ر�TIM8	
}


/**************************************************
	* 
	* @brief ��ѹһ�β��ص����
	* @param  fre: ��ѹ�ٶ�
	* 
***************************************************/
void press_once(uint32_t fre)
{
	count = 7;	
	Locate_Rle(400 * (depth_val+depth_start),fre,CW); 
	while(count)
	{				
		if(motor_sta != 1)
		{
			break;
		}
	}
	Locate_Abs(0,fre);
}

/**************************************************
	* 
	* @brief 32:2��ѹģʽ
	* 
***************************************************/
void press_32_2(uint32_t fre)
{
	OS_ERR      err;
	int         press_num = 64;
	/* ��ѹ32�� */
	for(;press_num >0;press_num --)
	{
		press_once(fre);
		if(motor_sta != 1)
		{
			break;
		}
	}
	/* ������ε�ʱ�� */
	rst_driver();
	OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_PERIODIC,&err);
}

/**************************************************
	* 
	* @brief 15:2��ѹģʽ
	* @param  fre: 
	* 
***************************************************/
void press_15_2(uint32_t fre)
{
	OS_ERR      err;
	int         press_num = 30;
	/* ��ѹ15�� */
	for(;press_num >0;press_num --)
	{
		press_once(fre);
		if(motor_sta != 1)
		{
			break;
		}
	}
	/* ������ε�ʱ�� */
	rst_driver();
	OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_PERIODIC,&err);
}

/**************************************************
	* 
	* @brief ������ѹģʽ
	* @param  fre: 
	* 
***************************************************/
void press_cont(uint32_t fre)
{
	press_once(fre);
}

/**************************************************
	* 
	* @brief ���Ƶ���Ķ���
	* @param  mode: ��ǰ�İ�ѹģʽ
	* ����:
	* 
***************************************************/
void ctr_driver(uint8_t mode)
{
	//���Ƶ�ǰ�İ�ѹ�ٶ�
	uint32_t fre = DRIVER_PRESS_FRE * (depth_val + depth_start);
//	uint32_t fre = 100 *depth_val;//����ʹ���ٶ�
	switch (mode)
	{
		//30:2ģʽ
		case 0:			
			press_32_2(fre);
			break;
		//15:2ģʽ
		case 1:
			press_cont(fre);
			break;
		//����ģʽ
		case 2:			
			press_15_2(fre);
			break;
		case 3:
		//��λģʽ
			rst_driver();
			break;
		default:
			break;
	}
}

/**************************************************
	* 
	* @brief ���ڼ�鰴ѹ�Ƿ�����
	* @return uint8_t: 0(����),-1(�쳣)
	* 
***************************************************/
int8_t check_press(void)
{
	uint8_t  drive_exception_count = 0;
	uint8_t  i;
	uint16_t adcx;
	uint16_t adcx_temp;
	count = 7;
	POINT_COLOR=RED;                                              //���û�����ɫ
	LCD_ShowString(100,140,380,20,24,"Calibrating, please wait������"); //��ʾϵͳ���ڽ���У׼
	rst_driver(); 
	Locate_Rle(400 * 30,2500,CW);                         //˳ʱ���½�30mm
	while(count);                                         //�ȴ��˶����
	for(i = 31;i < 54;i ++)
	{
		count = 7;
		Locate_Rle(400,1500,CW);                              //˳ʱ���½�1mm
		while(count);                                         //�ȴ��˶����
		adcx = Get_Adc_Average(3,ADC_Channel_4,20);           //��ȡADC3ͨ��4��ת��ֵ��20��ȡƽ��
		adcx_temp = (int)((float)i*26.29f + 9.42f);
		if(adcx < adcx_temp - 50 || adcx > adcx_temp + 50)    //��ֵ�仯�����Ϲ���,�����쳣
		{
			drive_exception_count += 1;
			if(drive_exception_count > 5)                     //����53��,�ۼ��쳣5��,
			{
				rst_driver();                                 //�����쳣,��ֹ����
				return -1;
			}			
		}
	}
	//��������
	rst_driver();                                	
	return 0;
}

/**************************************************
	* 
	* @brief ��鴫���Ƿ�����
	* @return uint8_t: 0(����),-1(�����쳣)
	* 
***************************************************/
int8_t check_driver(void)
{
	uint8_t  drive_exception_count = 0;
	uint8_t  i;
	uint16_t adcx;
	uint16_t adcx_temp;
	count = 7;
	POINT_COLOR=RED;                                              //���û�����ɫ
	LCD_ShowString(100,140,380,20,24,"Calibrating, please wait������"); //��ʾϵͳ���ڽ���У׼
	rst_driver();                                         //��ʼ���ǰ�ȸ�λ  
	for(i= 1;i < 54;i ++)
	{
		count = 7;
		Locate_Rle(400,1500,CW);                              //˳ʱ���½�1mm
		while(count);                                         //�ȴ��˶����
		adcx = Get_Adc_Average(3,ADC_Channel_4,20);           //��ȡADC3ͨ��4��ת��ֵ��20��ȡƽ��
		adcx_temp = (int)((float)i*26.29f + 9.42f);
		if(adcx < adcx_temp - 50 || adcx > adcx_temp + 50)    //��ֵ�仯�����Ϲ���,�����쳣
		{
			drive_exception_count += 1;
			if(drive_exception_count > 8)                     //����53��,�ۼ��쳣5��,
			{
				rst_driver();                             //�����쳣,��ֹ����
				return -1;
			}			
		}
	}   
	//��������
	rst_driver();                                	
	return 0;
}

/**************************************************
	* 
	* @brief ��ȡ�˹����õĵ����ʼλ��
	* @return uint8_t: 
	* 
***************************************************/
uint8_t get_driver_start(void)
{
	u8     start;
	u16    adcx;
	u16    adcx_temp;
	adcx_temp=Get_Adc_Average(3,ADC_Channel_4,20) - 10;//��ȡͨ��5��ת��ֵ��20��ȡƽ��
	if(adcx_temp > 2000)adcx_temp = 0;                 //��ֹ�޷������������ֵ
	if(adcx_temp>15)                                   //��⵽���������
	{
		start  = 15;
		for(;start > 0;start --)                       //ѭ�����15��,�ȴ���ֵ�仯������С
		{
			adcx = Get_Adc_Average(3,ADC_Channel_4,20);//��ȡͨ��4��ת��ֵ��20��ȡƽ��
			if(adcx > adcx_temp)
			{
				adcx = adcx - adcx_temp;
				if(adcx>45)break;
			}
			else
			{
				adcx = adcx_temp -adcx;
				if(adcx>45)break;
			}
			
		}
		if(!start)
		{
			adcx = Get_Adc_Average(3,ADC_Channel_4,20);//��ȡͨ��5��ת��ֵ��20��ȡƽ��
			depth_start = (int)((float)adcx + 9.42f)/26.29f; //������Ϻ�����ȡ��ǰ�ĵ����ʼλ��		
			App_Printf("��ǰ����ʼλ��Ϊ:%d",depth_start);
		}			
	}	
	return depth_start;
}

