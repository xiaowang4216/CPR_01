/**************************************************
	* @File Name: usbh_usr.c
	* @brief USB-USR����
	* @author ���ָ� (2891854535@qq.com)
	* @Version : 1.0
	* @date 2022-08-31
	* 
***************************************************/
#include "delay.h"
#include "exfuns.h"
#include "fattester.h"
#include "ff.h" 
#include "includes.h"
#include "led.h"
#include "log.h"
#include "usart.h" 
#include "tftlcd.h"
#include "usbh_usr.h" 
 

/* ��U�̾���ʱ,����flag��һ,����sd���µ���־�ļ���U�� */
/**************************************************
	* 
	* @brief ��������
	* 
***************************************************/
u8                          copy_sd_data_to_u = 0;/* ��U�̾���ʱ,��flag��һ,��ʼ�����ļ���U�� */
static u8                   AppState;             /* ��ȡ��ǰU�̵�״̬ */
extern int                  U_DISK_READY;         /* ��ȡU�̵�״̬ */
extern USB_OTG_CORE_HANDLE  USB_OTG_Core;


/**************************************************
	* 
	* @brief USB OTG�жϷ�����
	* 
***************************************************/
void OTG_FS_IRQHandler(void)
{
	OSIntEnter();
  	USBH_OTG_ISR_Handler(&USB_OTG_Core);
	OSIntExit(); 
} 

/**************************************************
	* 
	* @brief USB HOST �û��ص�����
	* 
***************************************************/
USBH_Usr_cb_TypeDef USR_cb=
{
	USBH_USR_Init,
	USBH_USR_DeInit,
	USBH_USR_DeviceAttached,
	USBH_USR_ResetDevice,
	USBH_USR_DeviceDisconnected,
	USBH_USR_OverCurrentDetected,
	USBH_USR_DeviceSpeedDetected,
	USBH_USR_Device_DescAvailable,
	USBH_USR_DeviceAddressAssigned,
	USBH_USR_Configuration_DescAvailable,
	USBH_USR_Manufacturer_String,
	USBH_USR_Product_String,
	USBH_USR_SerialNum_String,
	USBH_USR_EnumerationDone,
	USBH_USR_UserInput,
	USBH_USR_MSC_Application,
	USBH_USR_DeviceNotSupported,
	USBH_USR_UnrecoveredError
};
/**************************************************
	* 
	* @brief ����Ϊ�ص�����ʵ��
	* 
***************************************************/

/**************************************************
	* 
	* @brief USB HOST ��ʼ��
	* 
***************************************************/
void USBH_USR_Init(void)
{
	printf("USB OTG HS MSC Host\r\n");
	printf("> USB Host library started.\r\n");
	printf("  USB Host Library v2.1.0\r\n\r\n");
	
}

/**************************************************
	* 
	* @brief U�̲�����
	* 
***************************************************/
void USBH_USR_DeviceAttached(void)//U�̲���
{
	U_DISK_READY=1;
	printf("��⵽USB�豸����!\r\n");
}

/**************************************************
	* 
	* @brief U�̰γ����
	* 
***************************************************/
void USBH_USR_DeviceDisconnected (void)//U���Ƴ�
{
	U_DISK_READY=0;
	printf("USB�豸�γ�!\r\n");
}  

/**************************************************
	* 
	* @brief U�̸�λ
	* 
***************************************************/
void USBH_USR_ResetDevice(void)
{
	printf("��λ�豸...\r\n");
}

/**************************************************
	* 
	* @brief ���ӻ��ٶ�
	* @param  DeviceSpeed: �ӻ��ٶ�(0,1,2,����)
	* 
***************************************************/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
	if(DeviceSpeed==HPRT0_PRTSPD_HIGH_SPEED)
	{
		printf("����(HS)USB�豸!\r\n");
 	}  
	else if(DeviceSpeed==HPRT0_PRTSPD_FULL_SPEED)
	{
		printf("ȫ��(FS)USB�豸!\r\n"); 
	}
	else if(DeviceSpeed==HPRT0_PRTSPD_LOW_SPEED)
	{
		printf("����(LS)USB�豸!\r\n");  
	}
	else
	{
		printf("�豸����!\r\n");  
	}
}

/**************************************************
	* 
	* @brief ��⵽�ӻ���������
	* @param  DeviceDesc: �豸������ָ��
	* 
***************************************************/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{ 
	USBH_DevDesc_TypeDef *hs;
	hs=DeviceDesc;   
	printf("VID: %04Xh\r\n" , (uint32_t)(*hs).idVendor); 
	printf("PID: %04Xh\r\n" , (uint32_t)(*hs).idProduct); 
}

/**************************************************
	* 
	* @brief �ӻ������ַ�ɹ�
	* 
***************************************************/
void USBH_USR_DeviceAddressAssigned(void)
{
	printf("�ӻ���ַ����ɹ�!\r\n");   
}

//��������������Ч
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
	USBH_InterfaceDesc_TypeDef *id; 
	id = itfDesc;   
	if((*id).bInterfaceClass==0x08)
	{
		printf("���ƶ��洢���豸!\r\n"); 
	}else if((*id).bInterfaceClass==0x03)
	{
		printf("HID �豸!\r\n"); 
	}    
}
//��ȡ���豸Manufacturer String
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
	printf("Manufacturer: %s\r\n",(char *)ManufacturerString);
}
//��ȡ���豸Product String 
void USBH_USR_Product_String(void *ProductString)
{
	printf("Product: %s\r\n",(char *)ProductString);  
}
//��ȡ���豸SerialNum String 
void USBH_USR_SerialNum_String(void *SerialNumString)
{
	printf("Serial Number: %s\r\n",(char *)SerialNumString);    
} 
//�豸USBö�����
void USBH_USR_EnumerationDone(void)
{ 
	printf("�豸ö�����!\r\n\r\n");    
} 
//�޷�ʶ���USB�豸
void USBH_USR_DeviceNotSupported(void)
{
	printf("�޷�ʶ���USB�豸!\r\n\r\n");    
}  
//�ȴ��û����밴��,ִ����һ������
USBH_USR_Status USBH_USR_UserInput(void)
{ 
	printf("�����û�ȷ�ϲ���!\r\n");
	return USBH_USR_RESP_OK;
} 
//USB�ӿڵ�������
void USBH_USR_OverCurrentDetected (void)
{
	printf("�˿ڵ�������!!!\r\n");
} 

/* extern u8 USH_User_App(void);		//�û����������� */

u8 USH_User_App(void);
//USB HOST MSC���û�Ӧ�ó���
int USBH_USR_MSC_Application(void)
{
	u8 res=0;
  	switch(AppState)
  	{
    	case USH_USR_FS_INIT://��ʼ���ļ�ϵͳ 
			printf("��ʼִ���û�����!!!\r\n");
			AppState=USH_USR_FS_TEST;
      		break;
    	case USH_USR_FS_TEST:	//ִ��USB OTG ����������
			res=USH_User_App(); //�û�������
     		res=0;
			if(res)AppState=USH_USR_FS_INIT;
      		break;
    	default:break;
  	} 
	return res;
}
//�û�Ҫ�����³�ʼ���豸
void USBH_USR_DeInit(void)
{
  	AppState=USH_USR_FS_INIT;
}
//�޷��ָ��Ĵ���!!  
void USBH_USR_UnrecoveredError (void)
{
	printf("�޷��ָ��Ĵ���!!!\r\n\r\n");	
}
////////////////////////////////////////////////////////////////////////////////////////
//�û����庯��,ʵ��fatfs diskio�Ľӿں��� 
extern USBH_HOST              USB_Host;

//��ȡU��״̬
//����ֵ:0,U��δ����
//      1,����
u8 USBH_UDISK_Status(void)
{
	return HCD_IsDeviceConnected(&USB_OTG_Core);//����U��״̬
}

//��U��
//buf:�����ݻ�����
//sector:������ַ
//cnt:��������	
//����ֵ:����״̬;0,����;����,�������;		 
u8 USBH_UDISK_Read(u8* buf,u32 sector,u32 cnt)
{
	u8 res=1;
	if(HCD_IsDeviceConnected(&USB_OTG_Core)&&AppState==USH_USR_FS_TEST)//���ӻ�����,����APP����״̬
	{  		    
		do
		{
			res=USBH_MSC_Read10(&USB_OTG_Core,buf,sector,512*cnt);
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);		      
			if(!HCD_IsDeviceConnected(&USB_OTG_Core))
			{
				res=1;//��д����
				break;
			};   
		}while(res==USBH_MSC_BUSY);
	}else res=1;		  
	if(res==USBH_MSC_OK)res=0;	
	return res;
}

//дU��
//buf:д���ݻ�����
//sector:������ַ
//cnt:��������	
//����ֵ:����״̬;0,����;����,�������;		 
u8 USBH_UDISK_Write(u8* buf,u32 sector,u32 cnt)
{
	u8 res=1;
	if(HCD_IsDeviceConnected(&USB_OTG_Core)&&AppState==USH_USR_FS_TEST)//���ӻ�����,����APP����״̬
	{  		    
		do
		{
			res=USBH_MSC_Write10(&USB_OTG_Core,buf,sector,512*cnt); 
			USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);		      
			if(!HCD_IsDeviceConnected(&USB_OTG_Core))
			{
				res=1;//��д����
				break;
			};   
		}while(res==USBH_MSC_BUSY);
	}else res=1;		  
	if(res==USBH_MSC_OK)res=0;	
	return res;
}

u8 USH_User_App(void)
{  
	u32 total,free;
	u8 res=0;
	OS_ERR err;	
	f_mount(fs[3],"3:",0); 	//���¹���U��
	res=exf_getfree("3:",&total,&free);	
	if(res==0)
	{
		printf("U Disk Total Size:%d MB",total);	 
		printf("U Disk  Free Size:%d MB",free); 
		printf("3");		
	} 
	while(HCD_IsDeviceConnected(&USB_OTG_Core))//�豸���ӳɹ�
	{	
		LED1= !LED1;
		if(copy_sd_data_to_u == 1)
		{
			/* ��Ļ��ʾ��ʼ�����ļ� */
			LCD_ShowString(30,190,200,16,16,"start transferring files");
			/* �����ļ���U�� */
			log_copy(0);
			copy_sd_data_to_u = 0;
		}			
		/* ��ʱ500ms */
		OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_PERIODIC,&err);	
	}
	LED1 = 0;
	delay_ms(500);
	LED1 = 1;
 	f_mount(0,"3:",0); 	//ж��U��
	return res;
}









































