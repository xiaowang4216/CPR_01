#include "pwm.h"
#include "led.h"
#include "usart.h"
#include "adc.h"
#include "includes.h"	 
#include "os_control.h"
  
/**
 * @brief 初始化定时器3
 * @param  arr: 重装载值
 * @param  psc: 定时器分频
 */
static void TIM3_PWM_Init(uint32_t arr,uint32_t psc)
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

/**
 * @brief 电机控制初始化
 * @param  arr: 重装载值
 * @param  psc: 定时器分频
 * 说明:采用共阳极接法,PA1或者PA8做共阳极,输出高电平
 * PA7做PWm输出,为PUL-
 * PA4为电机使能控制引脚
 * PA6为电机旋转方向控制引脚
 */
void motor_init(uint32_t arr,uint32_t psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	TIM3_PWM_Init(arr,psc);                                 //初始化PWM输出

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能PORTA时钟	

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_8;           //GPIOA
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;           //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);                   //初始化PA1,4,6,8
		
	MOTOR_A   = ENABLE;                                     //拉高共阳极
	MOTOR_DIR = CW;                                         //正反向旋转,即向下
	MOTOR_EN  = ENABLE;                                     //电机旋转使能
}


/**
 * @brief 定时器3中断服务函数,辅助高精度控制电机旋转
 */
void TIM3_IRQHandler(void)   
{	
	static int count = 0;
	OSIntEnter(); 	                                   //开中断
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx的中断待处理位:TIM 中断源 
		count++;                                       //测试代码
		if(count > 200)
		{
			// printf("触发定时器3中断服务函数\r\n");
			MOTOR_DIR = ~MOTOR_DIR;
			MOTOR_EN = ~MOTOR_EN;
			count = 0;
		}
	}
	OSIntExit();    	                              //退出中断
}

/***
 * 通过GUI界面选择按压模式
 * 实时检测按压模式是否改变
 * 不同的按压模式输出不同频率的PWM
 * 通过调用ADC模块的电压检测
 * 实时检测按压深度
 * 通过检测往返的次数,计算一分钟的按压次数
 ***/


/***
 * 函数名称:motor_control
 * 变量:order:当前的按压模式
 * 返回值:无
 * 函数作用:通过检测按压模式的不同
 * 控制电机的输出频率
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
 * 函数名称:motor_reset
 * 参数:无
 * 返回值:无
 * 函数作用:复位电机回到初始状态
 * 说明 :伸出长度与电压值成正比,当电压值小于某个值的时候,
 * 	认为复位成功
 ***/
void motor_reset(void)
{
	float temp_val;
	//循环获取当前距离,直到距离小于多少
	//算复位成功
	while(1)
	{
		//获取当前电压值
		temp_val = Get_Adc_Average(3,ADC_Channel_4,10);
		//转换电压值
		temp_val = (float)temp_val*(3.3/4096);
		//复位成功
		if(temp_val < 0.5)
		{
			break;
		}
		//继续执行复位指令
		else
		{

		}
	}
}

