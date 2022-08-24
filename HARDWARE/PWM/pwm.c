#include "pwm.h"
#include "led.h"
#include "usart.h"
#include "adc.h"
#include "includes.h"	 
#include "os_control.h"
  
/**
 * @brief ��ʼ����ʱ��3
 * @param  arr: ��װ��ֵ
 * @param  psc: ��ʱ����Ƶ
 */
static void TIM3_PWM_Init(uint32_t arr,uint32_t psc)
{
	GPIO_InitTypeDef         GPIO_InitStructure;
	NVIC_InitTypeDef         NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef        TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,  ENABLE);  	   //TIM3ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	   //ʹ��PORTAʱ��	

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_TIM3);      //GPIOA����Ϊ��ʱ��3

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;                  //GPIOA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;               //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	       //�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;             //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;               //����
	GPIO_Init(GPIOA,&GPIO_InitStructure);                      //��ʼ��PCA7

	TIM_TimeBaseStructure.TIM_Prescaler=psc;                   //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;  //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;                      //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 

	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);             //��ʼ����ʱ��3

	//��ʼ��TIM3 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;   //�������:TIM����Ƚϼ��Ե�
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);                   //����Tָ���Ĳ�����ʼ������TIM3 OC2

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);          //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���

	TIM_ARRPreloadConfig(TIM3,ENABLE);                         //ARPEʹ�� 
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                   //��������ж�
	
	TIM_Cmd(TIM3, ENABLE);                                     //ʹ��TIM3
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;            //�жϷ�����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //�ж�ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);                            //��ʼ��VIC�Ĵ���
}

/**
 * @brief ������Ƴ�ʼ��
 * @param  arr: ��װ��ֵ
 * @param  psc: ��ʱ����Ƶ
 * ˵��:���ù������ӷ�,PA1����PA8��������,����ߵ�ƽ
 * PA7��PWm���,ΪPUL-
 * PA4Ϊ���ʹ�ܿ�������
 * PA6Ϊ�����ת�����������
 */
void motor_init(uint32_t arr,uint32_t psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	TIM3_PWM_Init(arr,psc);                                 //��ʼ��PWM���

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//ʹ��PORTAʱ��	

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_8;           //GPIOA
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //���츴�����
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;           //����
	GPIO_Init(GPIOA,&GPIO_InitStructure);                   //��ʼ��PA1,4,6,8
		
	MOTOR_A   = ENABLE;                                     //���߹�����
	MOTOR_DIR = CW;                                         //��������ת,������
	MOTOR_EN  = ENABLE;                                     //�����תʹ��
}


/**
 * @brief ��ʱ��3�жϷ�����,�����߾��ȿ��Ƶ����ת
 */
void TIM3_IRQHandler(void)   
{	
	static int count = 0;
	OSIntEnter(); 	                                   //���ж�
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		count++;                                       //���Դ���
		if(count > 200)
		{
			// printf("������ʱ��3�жϷ�����\r\n");
			MOTOR_DIR = ~MOTOR_DIR;
			MOTOR_EN = ~MOTOR_EN;
			count = 0;
		}
	}
	OSIntExit();    	                              //�˳��ж�
}

/***
 * ͨ��GUI����ѡ��ѹģʽ
 * ʵʱ��ⰴѹģʽ�Ƿ�ı�
 * ��ͬ�İ�ѹģʽ�����ͬƵ�ʵ�PWM
 * ͨ������ADCģ��ĵ�ѹ���
 * ʵʱ��ⰴѹ���
 * ͨ����������Ĵ���,����һ���ӵİ�ѹ����
 ***/


/***
 * ��������:motor_control
 * ����:order:��ǰ�İ�ѹģʽ
 * ����ֵ:��
 * ��������:ͨ����ⰴѹģʽ�Ĳ�ͬ
 * ���Ƶ�������Ƶ��
 ***/
void motor_control(int order)
{
	int order_id = order;

	switch (order_id)
	{
	case 0:
		/* code */
		break;
	case 1:
		/* code */
		break;
	case 2:
		/* code */
		break;
	
	default:
		break;
	}

}

/***
 * ��������:motor_reset
 * ����:��
 * ����ֵ:��
 * ��������:��λ����ص���ʼ״̬
 * ˵�� :����������ѹֵ������,����ѹֵС��ĳ��ֵ��ʱ��,
 * 	��Ϊ��λ�ɹ�
 ***/
void motor_reset(void)
{
	float temp_val;
	//ѭ����ȡ��ǰ����,ֱ������С�ڶ���
	//�㸴λ�ɹ�
	while(1)
	{
		//��ȡ��ǰ��ѹֵ
		temp_val = Get_Adc_Average(3,ADC_Channel_4,10);
		//ת����ѹֵ
		temp_val = (float)temp_val*(3.3/4096);
		//��λ�ɹ�
		if(temp_val < 0.5)
		{
			break;
		}
		//����ִ�и�λָ��
		else
		{

		}
	}
}

