/**************************************************
	* @File Name: myiic.c
	* @brief IIC操作
	* @author 王现刚 (2891854535@qq.com)
	* @Version : 1.0
	* @date 2022-08-31
	* 
***************************************************/
#include "myiic.h"
#include "delay.h"
 	

/**************************************************
	* 
	* @brief 初始化IIC
	* 
***************************************************/
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //开启GPIOB时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9; //PB8 9引脚设置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  //输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure); //GPIOB8 9设置
	
	IIC_SCL = 1;
	IIC_SDA = 1;
}

/**************************************************
	* 
	* @brief 产生IIC起始信号
	* 
***************************************************/
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}

/**************************************************
	* 
	* @brief 产生IIC停止信号
	* 
***************************************************/
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);			
}

/**************************************************
	* 
	* @brief 等待IIC应答信号
	* @return u8: 1(接受应答失败),0(接受应答成功)
	* 
***************************************************/
u8 IIC_Wait_Ack(void)
{
	
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
}

/**************************************************
	* 
	* @brief 产生ACK应答信号
	* 
***************************************************/
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

/**************************************************
	* 
	* @brief 不产生应答信号
	* 
***************************************************/
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答	
/**************************************************
	* 
	* @brief IIC发送一个字节,返回从机有误应答
	* @param  txd: 1(有应答),0(无应答)
	* 
***************************************************/
void IIC_Send_Byte(u8 txd)
{
	u8 t;   
	SDA_OUT(); 	    
  IIC_SCL=0;//拉低时钟开始数据传输
  for(t=0;t<8;t++)
  {              
    IIC_SDA=(txd&0x80)>>7;
    txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
  }	 
}

/**************************************************
	* 
	* @brief 读一个字节
	* @param  ack: 1(发送ACK),0(发送nACK)
	* @return u8: 读到的的字节
	* 
***************************************************/
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}

