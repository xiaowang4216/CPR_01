/**************************************************
	* @File Name: 24cxx.c
	* @brief 24cxx��ز���
	* @author ���ָ� (2891854535@qq.com)
	* @Version : 1.0
	* @date 2022-08-31
	* 
***************************************************/
#include "24cxx.h"
#include "delay.h"


/**************************************************
	* 
	* @brief ��ʼ��IIC�ӿ�
	* 
***************************************************/
void AT24CXX_Init(void)
{
	IIC_Init();
}

/**************************************************
	* 
	* @brief ��AT24CXXָ����ַ����һ������
	* @param  ReadAddr: ��ʼ�����ĵ�ַ
	* @return u8: ��ȡ��������
	* 
***************************************************/
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
  IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ	    
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	   
	IIC_Wait_Ack(); 
	IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}

/**************************************************
	* 
	* @brief ��AT24CXXָ����ַд��һ������
	* @param  WriteAddr: д�����ݵ�Ŀ�ĵ�ַ
	* @param  DataToWrite: Ҫд�������
	* 
***************************************************/
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
	IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ	  
	}else IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	IIC_Wait_Ack();	   
	IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
	IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}

/**************************************************
	* 
	* @brief ��AT24CXX�����ָ����ַ��ʼд�볤��len������
	* @param  WriteAddr: ��ʼд�����ݵĵ�ַ
	* @param  DataToWrite: ���ݵ��׵�ַ
	* @param  Len: Ҫд������ݳ���2,4
	* @note  �ú�������д��16Bit����32Bit������
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
	* @brief ��AT24CXXָ����ַ��ʼ��ȡ����Ϊlen������
	* @param  ReadAddr: ��ʼ��ȡ���ݵĵ�ַ
	* @param  Len: Ҫ��ȡ���ݵĳ���2,4
	* @return u32: ��ȡ��������
	* @note  �ú������ڶ���16Bit����32Bit������
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
	* @brief ���AT24CXX�Ƿ�����
	* @return u8: 1(���ʧ��),0(���ɹ�)
	* @note  ������AT24CXX�����һ����ַ(255)���洢��־��
	* 
***************************************************/
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

/**************************************************
	* 
	* @brief ��AT24CXX�����ָ����ַ��ʼ����ָ������������
	* @param  ReadAddr: ��ʼ�������ݵĵ�ַ(0-255)
	* @param  pBuffer: ���������׵�ַ
	* @param  NumToRead: Ҫ���������ݸ���
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
	* @brief ��AT24CXX��ָ����ַ��ʼд��ָ������������
	* @param  WriteAddr: ��ʼд�����ݵ��ֵ�ַ
	* @param  pBuffer: ���������׵�ַ
	* @param  NumToWrite: Ҫд������ݸ���
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








