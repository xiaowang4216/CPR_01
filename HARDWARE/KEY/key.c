/**************************************************
	* @File Name: key.c
	* @brief 按键操作
	* @author 王现刚 (2891854535@qq.com)
	* @Version : 1.0
	* @date 2022-08-31
	* 
***************************************************/
#include "key.h"
#include "delay.h"


/**************************************************
	* 
	* @brief 按键初始化
	* 
***************************************************/
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOE,ENABLE);  //使能GPIOA GPIOE时钟
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;  //PE2,3,4引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;  //输入
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  //上拉输入
	GPIO_Init(GPIOE,&GPIO_InitStructure); 			//初始化GPIOE
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;  //PA0引脚
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;  //下拉输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);     //初始化GPIOA
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
/**************************************************
	* 
	* @brief 按键处理函数
	* @param  mode: 0(不支持长按),1(支持长按)
	* @return u8: 1,key0按下 
	*             2,key1按下
	*             3,key2按下
	*             4,wkup按下
	*             0,没有任何按键按下
	* @note 此函数有优先响应级,1>2>3>4
	* 
***************************************************/
u8 KEY_Scan(u8 mode)
{
	static u8 key_up=1;   //按键松开标志
	if(mode) key_up=1;  //支持连按
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0||WK_UP==1)) //有按键按下
	{
		delay_ms(10);  //按键去抖
		key_up=0;
		if(KEY0==0) return 1;
		else if(KEY1==0) return 2;
		else if(KEY2==0) return 3;
		else if(WK_UP==1) return 4;
	}else if(KEY0==1&&KEY1==1&&KEY2==1&&WK_UP==0)key_up=1; 
	return 0; //无按键按下
}

