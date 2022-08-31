#include "pwm.h"
	  
/**
 * @brief 初始化定时器3
 * @param  arr: 重装载值
 * @param  psc: 定时器分频
 */
void TIM3_PWM_Init(uint32_t arr,uint32_t psc)
{
	GPIO_InitTypeDef         GPIO_InitStructure;
	NVIC_InitTypeDef         NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef        TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,  ENABLE);  	   //TIM3时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	   //使能PORTA时钟	

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_TIM3);      //GPIOA复用为定时器3

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;                  //GPIOA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;               //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	       //速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;             //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;               //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);                      //初始化PCA7

	TIM_TimeBaseStructure.TIM_Prescaler=psc;                   //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;  //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;                      //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 

	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);             //初始化定时器3

	//初始化TIM3 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;          //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;   //输出极性:TIM输出比较极性低
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);                   //根据T指定的参数初始化外设TIM3 OC2

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);          //使能TIM3在CCR2上的预装载寄存器

	TIM_ARRPreloadConfig(TIM3,ENABLE);                         //ARPE使能 
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);                   //开启相关中断
	
	TIM_Cmd(TIM3, ENABLE);                                     //使能TIM3
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;            //中断服务函数
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //中断通道使能
	NVIC_Init(&NVIC_InitStructure);                            //初始化VIC寄存器
}











