#include "log.h"
#include "alarm_log.h"
#include "os_control.h"
#include "malloc.h"
/***
 * ��ϵͳ��������ʱ
 * ��SD���ļ���,����ļ��Ƿ����
 * ����ļ�������,�����ļ�
 * ����ļ�����
 * ���ļ�
 * ���ļ�д�������־
 ***/

/***
 * ���ļ���������
 * ���U���Ƿ����
 * ���U���Ѿ�����
 * �����ʼ���䰴ť
 * ��ʼ�����ݴ�SD����U�̱���
 ***/

//Ĭ����־��ӡ·��
const TCHAR* log_pathn = "0:LOG";
//Ĭ����־��ӡ·���ļ�
const TCHAR* log_pname = "0:LOG/log.txt";
//���ݸ���U��·��
u8  ulog_pname[20];
//sd���ļ�
static FIL sd_file;	  	
//U���ļ�
static FIL ud_file;	
//ʱ��ṹ��
extern RTC_TimeTypeDef  RTC_TimeStructure;
extern RTC_DateTypeDef  RTC_DateStructure;
//��ѹ���
extern int depth_val;
/**********************************************************
*�������ܣ����ļ��У�����ļ��в����ھʹ����ļ��У����ھ͹ر�
*���룺	pathn���ļ�������
*˵����
***********************************************************/
u8 mkdir(const TCHAR* pathn)
{
	u8 res;
	res = f_opendir(&dir, pathn);   // ���Դ�Ŀ¼
	if(res != FR_OK)
	{
		res = f_mkdir(pathn);       // �����ʧ�ܣ����½��ļ���
	}
	else
	{
		res = f_closedir(&dir);     // ���Ŀ¼���ڣ���ر�
	}
	return res;
}

char string_buf[50] = {0};		    //�����洢д���ļ�������

/**********************************************************
*�������ܣ���SD��д�뱨����־
*���룺	   ��
*˵����   ��ʱû�����ô���,����Ӧ�����ò�����ѡ��
* д��ı�����־,Ŀǰ�ı�����־����������
*����ֵ:  ��
***********************************************************/
void write_data_sd(void)
{
    u8          res;               //��ȡ�ļ���д״̬
	u16         filesize;          //��ȡ��ǰ�ļ���С
	OS_ERR      err;               //��ȡϵͳ����ִ��״̬

    res = mkdir(log_pathn);        //����ļ���״̬
    if(res != FR_OK)
    {
        App_Printf("�ļ��д���ʧ��\r\n");
    }
	
	memset(string_buf,0,sizeof(string_buf));//�������	

	//�ϲ�������־
	sprintf(string_buf,"��ʼʱ�䣺%02d-%02d-%02d %02d:%02d:%02d@\r\n@",\
	RTC_DateStructure.RTC_Year,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,\
	RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
	sprintf(string_buf,"%s��ѹ��ȣ�%d mm@\r\n@",string_buf,depth_val);

	//��ȡҪд����ļ������Ѿ�д����ļ���С
    res = f_open(&sd_file,log_pname,FA_OPEN_EXISTING | FA_READ);
    filesize = f_size(&sd_file);
    f_close(&sd_file);
    
	//���ļ�,��ʼ׷��д������
    res=f_open(&sd_file,log_pname, FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
    res=f_lseek(&sd_file,filesize); 
    res = f_write(&sd_file,string_buf,strlen(string_buf),&bw);
    f_close(&sd_file);
	//��ʱһ��,��װд��ܾ�
    OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);//��ʱһ����
}

/**
 * @brief ���ô˺���,��SD��д�������־
 * @param  mode: Ҫд�����־
 * 1:��ص�����
 * 2:��ѹ��ȵ���
 * @return uint8_t: 
 * 0:д��ɹ�
 * 1:�ļ��д���ʧ��
 * 2:����Ĳ���������
 * 3:�����ļ�ʧ��
 */
uint8_t write_log_old(uint8_t mode)
{
	u8          res;               //��ȡ�ļ���д״̬
	u16         filesize;          //��ȡ��ǰ�ļ���С
	res = mkdir(log_pathn);        //����ļ���״̬
    if(res != FR_OK)
    {
        App_Printf("�ļ��д���ʧ��\r\n");
		return 1;
    }
	/* �ļ��д��� */
	memset(string_buf,0,sizeof(string_buf));//�������
	//��ȡ��ǰʱ��
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN,&RTC_DateStructure);
	//�ϲ�������־��������
	sprintf(string_buf,"��ʼʱ�䣺%02d-%02d-%02d %02d:%02d:%02d@\r\n@",\
	RTC_DateStructure.RTC_Year,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,\
	RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
	/* �ϲ�������־ */
	switch (mode)
	{
		/* ��ص������� */
		case 1:
			sprintf(string_buf,"%s��ص�������@2@\r\n@",string_buf);
			break;
		/* ��ѹ��������� */
		case 2:
			sprintf(string_buf,"%s��ѹ��ȣ�%d mm@1@\r\n@",string_buf,depth_val);
			break;
		default:
			App_Printf("д����־�Ĳ�������,�����\r\n");
			return 2;
	}
	//��ȡҪд����ļ������Ѿ�д����ļ���С
    res = f_open(&sd_file,log_pname,FA_OPEN_EXISTING | FA_READ);
    filesize = f_size(&sd_file);
    f_close(&sd_file);

	//���ļ�,��ʼ׷��д������
    res=f_open(&sd_file,log_pname, FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
	if(res != FR_OK)
	{
		App_Printf("��־�ļ���ʧ��\r\n");
		return 3;
	}
    res=f_lseek(&sd_file,filesize); 
    res = f_write(&sd_file,string_buf,strlen(string_buf),&bw);
    f_close(&sd_file);
	/* д������ */
	return 0;
}

/**
 * @brief ���ô˺���,��SD��д�������־
 * @param  mode: Ҫд�����־
 * 1:��ص�����
 * 2:��ѹ��ȵ���
 * 3:��λʧ��
 * @return uint8_t: 
 * 0:д��ɹ�
 * 1:�ļ��д���ʧ��
 * 2:����Ĳ���������
 * 3:�����ļ�ʧ��
 */
uint8_t write_log(uint8_t mode)
{
	u8          res;               //��ȡ�ļ���д״̬
	u16         filesize;          //��ȡ��ǰ�ļ���С
	res = mkdir(log_pathn);        //����ļ���״̬
    if(res != FR_OK)
    {
        App_Printf("�ļ��д���ʧ��\r\n");
		return 1;
    }
	/* �ļ��д��� */
	memset(string_buf,0,sizeof(string_buf));//�������
	//��ȡ��ǰʱ��
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN,&RTC_DateStructure);
	//�ϲ�������־��������
	sprintf(string_buf,"��ʼʱ�䣺%02d-%02d-%02d %02d:%02d:%02d\r\n",\
	RTC_DateStructure.RTC_Year,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,\
	RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
	/* �ϲ�������־ */
	switch (mode)
	{
		/* ��ص������� */
		case 1:
			sprintf(string_buf,"%s��ص�������\r\n2\r\n\r\n",string_buf);
			break;
		/* ��ѹ��������� */
		case 2:
			sprintf(string_buf,"%s��ѹ��ȣ�%d mm\r\n1\r\n\r\n",string_buf,depth_val);
			break;
		/* ��λʧ�� */
		case 3:
			sprintf(string_buf,"%s��λʧ��\r\n1\r\n\r\n",string_buf);
			break;
		default:
			App_Printf("д����־�Ĳ�������,�����\r\n");
			return 2;
	}
	//��ȡҪд����ļ������Ѿ�д����ļ���С
    res = f_open(&sd_file,log_pname,FA_OPEN_EXISTING | FA_READ);
    filesize = f_size(&sd_file);
    f_close(&sd_file);

	//���ļ�,��ʼ׷��д������
    res=f_open(&sd_file,log_pname, FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
	if(res != FR_OK)
	{
		App_Printf("��־�ļ���ʧ��\r\n");
		return 3;
	}
    res=f_lseek(&sd_file,filesize); 
    res = f_write(&sd_file,string_buf,strlen(string_buf),&bw);
    f_close(&sd_file);
	/* д������ */
	return 0;
}
/**********************************************************
*�������ܣ��ļ�����
*���룺	psrc,pdst:Դ�ļ���Ŀ���ļ�
				fwmode:�ļ�д��ģʽ
				0:������ԭ�е��ļ�	
				1:����ԭ�е��ļ�
*˵������psrc�ļ�,copy��pdst.
***********************************************************/
u8 log_copy(void *p_arg)
{
	u8          res;
	u16         filesize;
	u16         index=0;
	char        *buf = NULL;	
	
	(void)p_arg;
	
	//��ȡ��־�ļ��Ĵ�С
    res = f_open(&sd_file,log_pname,FA_OPEN_EXISTING | FA_READ);
    filesize = f_size(&sd_file);
    f_close(&sd_file);
	//���´��ļ�
	res = f_open(&sd_file,log_pname,FA_READ);
    if(res != FR_OK)
    {
        App_Printf("SD ��ȡ����\r\n");
        return res;
    }//SD�� �ļ���ȡ�ɹ�
	else
	{	
		//�鿴U��LOG�ļ����Ƿ����	
		res = mkdir("3:LOG");
		while(index<0XFFFF)
		{
			sprintf((char*)ulog_pname,"3:LOG/log%05d.txt",index);
			res=f_open(&ud_file,(const TCHAR*)ulog_pname,FA_READ);//���Դ�����ļ�
			if(res!=FR_OK)break;   //���ļ���������
			else if(res==FR_OK) f_close(&ud_file);//����򿪳ɹ����͹رյ�
			index++;
		}
		res = f_open(&ud_file,(const TCHAR*)ulog_pname,7);//�½��ļ�		
		if(res != FR_OK)
		{
			App_Printf("U�� ��ȡ����\r\n");
			return res;
		}
	}
	
	//�ļ��򿪳ɹ�
    buf = mymalloc(SRAMEX,filesize);//�����ڴ�
	
	//��ȡ�ļ�������
	App_Printf("��ʼ�����ļ�\r\n");
    for(;;)
	{
		res = f_read(&sd_file,buf,512,&br); //��ȡ����
		if(res||br == 0)break;              //û�пɶ�ȡ��������,�������ζ�ȡ
		res = f_write(&ud_file,buf,(UINT)br,&bw); //��ʼд������
		if(res||bw<br)break;
	}
		
    f_close(&sd_file);
	f_close(&ud_file);
	
	myfree(SRAMEX,buf);		//�ͷ��ڴ�
	
	return res;
}






