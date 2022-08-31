#include "pwm.h"
	  
/**
 * @brief ��ʼ����ʱ��3
 * @param  arr: ��װ��ֵ
 * @param  psc: ��ʱ����Ƶ
 */
void TIM3_PWM_Init(uint32_t arr,uint32_t psc)
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











