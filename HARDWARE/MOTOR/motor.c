/**************************************************
	* @File Name: motor.c
	* @brief 电机控制相关函数定义
	* @author 王现刚 (2891854535@qq.com)
	* @Version : 1.2
	* @date 2022-08-29
	* V1.1 修改复位函数,防止人工干预系统启动是电机的初始位置
	* V1.2 添加人工设置电机初始位置功能
***************************************************/
#include "adc.h"
#include "delay.h"
#include "tftlcd.h"
#include "includes.h"
#include "log.h"
#include "motor.h"
#include "os_control.h"
#include "usart.h"


/********** 驱动器 端口定义 **************
//DRIVER_DIR   PA4 (DCMI_D6)
//DRIVER_OE    PA6 (DCMI_D7)
//STEP_PULSE   PC7 (TIM8_CH2  DCMI_D1)
******************************************/

/**************************************************
	* 
	* @brief 变量定义
	* 
***************************************************/
u8       first_rst_flag = 1; //是否能成功复位的标志
u8       rcr_remainder;      //重复计数余数部分
u8       is_rcr_finish  = 1; //重复计数器是否设置完成
long     rcr_integer;	     //重复计数整数部分
long     target_pos     = 0; //有符号方向
long     current_pos    = 0; //有符号方向
DIR_Type motor_dir      = CW;//顺时针
int      depth_start    = 0; /* 用于获取当前人工设置的起始位置 */
extern   int      depth_val; /* 用于获取当前的按压深度 */
extern   uint8_t  motor_sta; /* 用于获取当前的按压状态 */

int count  = 6;              /* 自定义的按压完成标志 */

/************** 驱动器控制信号线初始化 ****************/
void Driver_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //使能GPIOE时钟
 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4|GPIO_Pin_6; //DRIVER_DIR DRIVER_OE对应引脚
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;    //普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;     //上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);            //初始化GPIOA4,6
	
	GPIO_SetBits(GPIOA,GPIO_Pin_4);  //PA4输出高 顺时针方向  DRIVER_DIR
	GPIO_ResetBits(GPIOA,GPIO_Pin_6);//PA6输出低 使能输出  DRIVER_OE
}
/***********************************************
//TIM8_CH2(PC7) 单脉冲输出+重复计数功能初始化
//TIM8 时钟频率 84*2=168MHz
//arr:自动重装值
//psc:时钟预分频数
************************************************/
void TIM8_OPM_RCR_Init(u16 arr,u16 psc)
{		 					 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);  	//TIM8时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 	//使能PORTC时钟	                                                                     	

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8);   //GPIOC7复用为定时器8
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;         //GPIOC7
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;       //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;     //下拉
	GPIO_Init(GPIOC,&GPIO_InitStructure);               //初始化PC7
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值   
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	TIM_ClearITPendingBit(TIM8,TIM_IT_Update);

	TIM_UpdateRequestConfig(TIM8,TIM_UpdateSource_Regular); /********* 设置只有计数溢出作为更新中断 ********/
	TIM_SelectOnePulseMode(TIM8,TIM_OPMode_Single);/******* 单脉冲模式 **********/
 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出2使能
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable; /****** 比较输出2N失能 *******/
	TIM_OCInitStructure.TIM_Pulse = arr>>1; //设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH2预装载使能	 
	TIM_ARRPreloadConfig(TIM8, ENABLE); //使能TIMx在ARR上的预装载寄存器
	
	TIM_ITConfig(TIM8, TIM_IT_Update ,ENABLE);  //TIM8   使能或者失能指定的TIM中断
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;  //TIM8中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	TIM_ClearITPendingBit(TIM8, TIM_IT_Update);  //清除TIMx的中断待处理位:TIM 中断源
	TIM_Cmd(TIM8, ENABLE);  //使能TIM8											  
}
/******* TIM8更新中断服务程序 *********/
void TIM8_UP_TIM13_IRQHandler(void)
{
	OSIntEnter();                                   //开中断
	if(TIM_GetITStatus(TIM8,TIM_FLAG_Update)!=RESET)//更新中断
	{
		TIM_ClearITPendingBit(TIM8,TIM_FLAG_Update);//清除更新中断标志位		
		if(is_rcr_finish==0)//重复计数器未设置完成
		{
			if(rcr_integer!=0) //整数部分脉冲还未发送完成
			{
				TIM8->RCR=RCR_VAL;//设置重复计数值
				rcr_integer--;//减少RCR_VAL+1个脉冲				
			}else if(rcr_remainder!=0)//余数部分脉冲 不位0
			{
				TIM8->RCR=rcr_remainder-1;//设置余数部分
				rcr_remainder=0;//清零
				is_rcr_finish=1;//重复计数器设置完成				
			}else goto out;   //rcr_remainder=0，直接退出			 
			TIM_GenerateEvent(TIM8,TIM_EventSource_Update);//产生一个更新事件 重新初始化计数器
			TIM_CtrlPWMOutputs(TIM8,ENABLE);	//MOE 主输出使能	
			TIM_Cmd(TIM8, ENABLE);  //使能TIM8			
			if(motor_dir==CW) //如果方向为顺时针   
				current_pos+=(TIM8->RCR+1);//加上重复计数值
			else          //否则方向为逆时针
				current_pos-=(TIM8->RCR+1);//减去重复计数值			
		}else
		{
out:		is_rcr_finish=1;//重复计数器设置完成
			TIM_CtrlPWMOutputs(TIM8,DISABLE);	//MOE 主输出关闭
			TIM_Cmd(TIM8, DISABLE);  //关闭TIM8				
			count = 0;
		}	
	}
	OSIntExit();                     //关中断
}
/***************** 启动TIM8 *****************/
void TIM8_Startup(u32 frequency)   //启动定时器8
{
	u16 temp_arr=1000000/frequency-1; 
	TIM_SetAutoreload(TIM8,temp_arr);//设定自动重装值	
	TIM_SetCompare2(TIM8,temp_arr>>1); //匹配值2等于重装值一半，是以占空比为50%	
	TIM_SetCounter(TIM8,0);//计数器清零
	TIM_Cmd(TIM8, ENABLE);  //使能TIM8
}
/********************************************
//相对定位函数 
//num 0～2147483647
//frequency: 20Hz~200KHz
//dir: CW(顺时针方向)  CCW(逆时针方向)
*********************************************/
void Locate_Rle(long num,u32 frequency,DIR_Type dir) //相对定位函数
{
	if(num<=0) //数值小等于0 则直接返回
	{
		return;
	}
	if(TIM8->CR1&0x01)//上一次脉冲还未发送完成  直接返回
	{
		return;
	}
	if((frequency<20)||(frequency>200000))//脉冲频率不在范围内 直接返回
	{
		return;
	}
	motor_dir=dir;//得到方向	
	DRIVER_DIR=motor_dir;//设置方向
	
	if(motor_dir==CW)//顺时针
		target_pos=current_pos+num;//目标位置
	else if(motor_dir==CCW)//逆时针
		target_pos=current_pos-num;//目标位置
	
	rcr_integer=num/(RCR_VAL+1);//重复计数整数部分
	rcr_remainder=num%(RCR_VAL+1);//重复计数余数部分
	is_rcr_finish=0;//重复计数器未设置完成
	TIM8_Startup(frequency);//开启TIM8
}
/********************************************
//绝对定位函数 
//num   -2147483648～2147483647
//frequency: 20Hz~200KHz
*********************************************/
void Locate_Abs(long num,u32 frequency)                       //绝对定位函数
{
	if(TIM8->CR1&0x01)                                        //上一次脉冲还未发送完成 直接返回
	{
		return;
	}
	if((frequency<20)||(frequency>200000))                    //脉冲频率不在范围内 直接返回
	{
		return;
	}
	target_pos=num;                                           //设置目标位置
	if(target_pos!=current_pos)                               //目标和当前位置不同
	{
		if(target_pos>current_pos)
			motor_dir=CW;                                     //顺时针
		else
			motor_dir=CCW;                                    //逆时针
		DRIVER_DIR=motor_dir;                                 //设置方向
		
		rcr_integer=abs(target_pos-current_pos)/(RCR_VAL+1);  //重复计数整数部分
		rcr_remainder=abs(target_pos-current_pos)%(RCR_VAL+1);//重复计数余数部分
		is_rcr_finish=0;                                      //重复计数器未设置完成
		TIM8_Startup(frequency);                              //开启TIM8
	}
}

/**************************************************
	* 
	* @brief 对电机进行复位设置
	* 
***************************************************/
void rst_driver(void)
{
	static   uint8_t i = 1;
	u16      adcx;
	uint16_t rst_count = 0;                                   //复位时间计数
	uint32_t frequency = DRIVER_RST_S;                        //获取设置的电机复位速度
	adcx=Get_Adc_Average(3,ADC_Channel_4,20);                 //获取通道4的转换值，20次取平均

	while(adcx > 5 && first_rst_flag)
	{
		rst_count ++;                                         
		if(rst_count >= 500)                                  //时间超过50s,认为复位失败
		{
			write_log(3);                                     //写入复位失败报警日志
			i = 0;                                            //系统启动复位失败,后面不用管
			first_rst_flag = 0;                               //复位失败标志,等待机器检修
			break;
		}
		adcx=Get_Adc_Average(3,ADC_Channel_4,20);             //获取ADC3通道4的转换值，20次取平均
		target_pos=1;                                         //设置目标位置
		motor_dir=CCW;                                        //逆时针
		DRIVER_DIR=motor_dir;                                 //设置方向
		rcr_integer=abs(target_pos-current_pos)/(RCR_VAL+1);  //重复计数整数部分
		rcr_remainder=abs(target_pos-current_pos)%(RCR_VAL+1);//重复计数余数部分
		is_rcr_finish=0;                                      //重复计数器未设置完成
		TIM8_Startup(frequency);                              //开启TIM8

	}
	TIM_CtrlPWMOutputs(TIM8,DISABLE);	                      //MOE 主输出关闭
	TIM_Cmd(TIM8, DISABLE);                                   //关闭TIM8
	adcx=Get_Adc_Average(3,ADC_Channel_4,20);                 //获取通道4的转换值，20次取平均
	
	while(adcx < 5 && i)                                      //保证系统启动是,电机的复位位置不是人工干预过的
	{
		i = 0;
		count = 7;
		adcx=Get_Adc_Average(3,ADC_Channel_4,10);             //获取通道4的转换值，20次取平均
		Locate_Rle(100,frequency,CW);
		while(count);

	}
	rcr_integer = 0;
	rcr_remainder = 0;
	target_pos = 0;
	current_pos = 0;
	is_rcr_finish = 1;
	count = 7;

	TIM_CtrlPWMOutputs(TIM8,DISABLE);	                      //MOE 主输出关闭
	TIM_Cmd(TIM8, DISABLE);                                   //关闭TIM8	
}


/**************************************************
	* 
	* @brief 按压一次并回到起点
	* @param  fre: 按压速度
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
	* @brief 32:2按压模式
	* 
***************************************************/
void press_32_2(uint32_t fre)
{
	OS_ERR      err;
	int         press_num = 64;
	/* 按压32次 */
	for(;press_num >0;press_num --)
	{
		press_once(fre);
		if(motor_sta != 1)
		{
			break;
		}
	}
	/* 间隔两次的时间 */
	rst_driver();
	OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_PERIODIC,&err);
}

/**************************************************
	* 
	* @brief 15:2按压模式
	* @param  fre: 
	* 
***************************************************/
void press_15_2(uint32_t fre)
{
	OS_ERR      err;
	int         press_num = 30;
	/* 按压15次 */
	for(;press_num >0;press_num --)
	{
		press_once(fre);
		if(motor_sta != 1)
		{
			break;
		}
	}
	/* 间隔两次的时间 */
	rst_driver();
	OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_PERIODIC,&err);
}

/**************************************************
	* 
	* @brief 连续按压模式
	* @param  fre: 
	* 
***************************************************/
void press_cont(uint32_t fre)
{
	press_once(fre);
}

/**************************************************
	* 
	* @brief 控制电机的动作
	* @param  mode: 当前的按压模式
	* 测试:
	* 
***************************************************/
void ctr_driver(uint8_t mode)
{
	//控制当前的按压速度
	uint32_t fre = DRIVER_PRESS_FRE * (depth_val + depth_start);
//	uint32_t fre = 100 *depth_val;//调试使用速度
	switch (mode)
	{
		//30:2模式
		case 0:			
			press_32_2(fre);
			break;
		//15:2模式
		case 1:
			press_cont(fre);
			break;
		//连续模式
		case 2:			
			press_15_2(fre);
			break;
		case 3:
		//复位模式
			rst_driver();
			break;
		default:
			break;
	}
}

/**************************************************
	* 
	* @brief 用于检查按压是否正常
	* @return uint8_t: 0(正常),-1(异常)
	* 
***************************************************/
int8_t check_press(void)
{
	uint8_t  drive_exception_count = 0;
	uint8_t  i;
	uint16_t adcx;
	uint16_t adcx_temp;
	count = 7;
	POINT_COLOR=RED;                                              //设置画笔颜色
	LCD_ShowString(100,140,380,20,24,"Calibrating, please wait···"); //显示系统正在进行校准
	rst_driver(); 
	Locate_Rle(400 * 30,2500,CW);                         //顺时针下降30mm
	while(count);                                         //等待运动完成
	for(i = 31;i < 54;i ++)
	{
		count = 7;
		Locate_Rle(400,1500,CW);                              //顺时针下降1mm
		while(count);                                         //等待运动完成
		adcx = Get_Adc_Average(3,ADC_Channel_4,20);           //获取ADC3通道4的转换值，20次取平均
		adcx_temp = (int)((float)i*26.29f + 9.42f);
		if(adcx < adcx_temp - 50 || adcx > adcx_temp + 50)    //差值变化不符合规律,传动异常
		{
			drive_exception_count += 1;
			if(drive_exception_count > 5)                     //测试53次,累计异常5次,
			{
				rst_driver();                                 //传动异常,终止测试
				return -1;
			}			
		}
	}
	//传动正常
	rst_driver();                                	
	return 0;
}

/**************************************************
	* 
	* @brief 检查传动是否正常
	* @return uint8_t: 0(正常),-1(传动异常)
	* 
***************************************************/
int8_t check_driver(void)
{
	uint8_t  drive_exception_count = 0;
	uint8_t  i;
	uint16_t adcx;
	uint16_t adcx_temp;
	count = 7;
	POINT_COLOR=RED;                                              //设置画笔颜色
	LCD_ShowString(100,140,380,20,24,"Calibrating, please wait···"); //显示系统正在进行校准
	rst_driver();                                         //开始检查前先复位  
	for(i= 1;i < 54;i ++)
	{
		count = 7;
		Locate_Rle(400,1500,CW);                              //顺时针下降1mm
		while(count);                                         //等待运动完成
		adcx = Get_Adc_Average(3,ADC_Channel_4,20);           //获取ADC3通道4的转换值，20次取平均
		adcx_temp = (int)((float)i*26.29f + 9.42f);
		if(adcx < adcx_temp - 50 || adcx > adcx_temp + 50)    //差值变化不符合规律,传动异常
		{
			drive_exception_count += 1;
			if(drive_exception_count > 8)                     //测试53次,累计异常5次,
			{
				rst_driver();                             //传动异常,终止测试
				return -1;
			}			
		}
	}   
	//传动正常
	rst_driver();                                	
	return 0;
}

/**************************************************
	* 
	* @brief 获取人工设置的电机起始位置
	* @return uint8_t: 
	* 
***************************************************/
uint8_t get_driver_start(void)
{
	u8     start;
	u16    adcx;
	u16    adcx_temp;
	adcx_temp=Get_Adc_Average(3,ADC_Channel_4,20) - 10;//获取通道5的转换值，20次取平均
	if(adcx_temp > 2000)adcx_temp = 0;                 //防止无符号数跳到最大值
	if(adcx_temp>15)                                   //检测到电机被拉动
	{
		start  = 15;
		for(;start > 0;start --)                       //循环检测15次,等待数值变化波动变小
		{
			adcx = Get_Adc_Average(3,ADC_Channel_4,20);//获取通道4的转换值，20次取平均
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
			adcx = Get_Adc_Average(3,ADC_Channel_4,20);//获取通道5的转换值，20次取平均
			depth_start = (int)((float)adcx + 9.42f)/26.29f; //调用拟合函数获取当前的电机起始位置		
			App_Printf("当前的起始位置为:%d",depth_start);
		}			
	}	
	return depth_start;
}

