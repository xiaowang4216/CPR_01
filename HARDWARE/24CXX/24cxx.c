/**************************************************
	* @File Name: 24cxx.c
	* @brief 24cxx相关操作
	* @author 王现刚 (2891854535@qq.com)
	* @Version : 1.0
	* @date 2022-08-31
	* 
***************************************************/
#include "24cxx.h"
#include "delay.h"


/**************************************************
	* 
	* @brief 初始化IIC接口
	* 
***************************************************/
void AT24CXX_Init(void)
{
	IIC_Init();
}

/**************************************************
	* 
	* @brief 在AT24CXX指定地址读出一个数据
	* @param  ReadAddr: 开始读数的地址
	* @return u8: 读取到的数据
	* 
***************************************************/
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
  IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址	    
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	   
	IIC_Wait_Ack(); 
	IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();//产生一个停止条件	    
	return temp;
}

/**************************************************
	* 
	* @brief 在AT24CXX指定地址写入一个数据
	* @param  WriteAddr: 写入数据的目的地址
	* @param  DataToWrite: 要写入的数据
	* 
***************************************************/
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
	IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址	  
	}else IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	IIC_Wait_Ack();	   
	IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
	IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}

/**************************************************
	* 
	* @brief 在AT24CXX里面的指定地址开始写入长度len的数据
	* @param  WriteAddr: 开始写入数据的地址
	* @param  DataToWrite: 数据的首地址
	* @param  Len: 要写入的数据长度2,4
	* @note  该函数用于写入16Bit或者32Bit的数据
	* 
***************************************************/
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

/**************************************************
	* 
	* @brief 在AT24CXX指定地址开始读取长度为len的数据
	* @param  ReadAddr: 开始读取数据的地址
	* @param  Len: 要读取数据的长度2,4
	* @return u32: 读取到的数据
	* @note  该函数用于读出16Bit或者32Bit的数据
	* 
***************************************************/
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

/**************************************************
	* 
	* @brief 检查AT24CXX是否正常
	* @return u8: 1(检测失败),0(检测成功)
	* @note  这里用AT24CXX的最后一个地址(255)来存储标志字
	* 
***************************************************/
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

/**************************************************
	* 
	* @brief 在AT24CXX里面的指定地址开始读出指定个数的数据
	* @param  ReadAddr: 开始读出数据的地址(0-255)
	* @param  pBuffer: 数据数组首地址
	* @param  NumToRead: 要读出的数据个数
	* 
***************************************************/
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  

/**************************************************
	* 
	* @brief 在AT24CXX的指定地址开始写入指定个数的数据
	* @param  WriteAddr: 开始写入数据的手地址
	* @param  pBuffer: 数据数组首地址
	* @param  NumToWrite: 要写入的数据个数
	* 
***************************************************/
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}








