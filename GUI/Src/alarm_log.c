/**************************************************
	* @File Name: alarm_log.c
	* @brief ������־�Ķ�ȡ����ʾ
	* @author ���ָ� (2891854535@qq.com)
	* @Version : 1.0
	* @date 2022-08-31
	*V1.1 �޸���־�����µ�һ����ʼ��ʾ
	* 
***************************************************/
#include "alarm_log.h"
#include "set_menu.H"
#include "string.h"

/***
 * ����־����
 * ϵͳ��ȡSD��������־���ļ�
 * ��ȡ������Ϣͨ���ָ��ӡ��ϵͳGUI����
 * �����Ŀ���ڶ����� :����
 * ��ֻ��ʾ���µĶ����� :����
 * ��ͨ�����������鿴
 ***/


/**************************************************
	* 
	* @brief ��ťID�궨��
	* 
***************************************************/
//���ذ�ť
#define ID_LOG_BACK_BUTTON    (GUI_ID_USER + 0x00)
//���ư�ť
#define ID_LOG_ADD_BUTTON     (GUI_ID_USER + 0x01)
//���ư�ť
#define ID_LOG_DEC_BUTTON     (GUI_ID_USER + 0x02)

/**************************************************
	* 
	* @brief ��������
	* 
***************************************************/
extern const TCHAR* log_pname;  /* ���ڻ�ȡ��־�ļ��洢��·�� */
static FIL log_file;	        /* SD����־�ļ� */
char * log_buf[5];              /* ��־�ļ���ȡ���� */
int total_log = 0;              /* �ܵ���־�ļ���Ŀ */
struct log_msg log_ifo[50];     /* ��־�ļ��洢�ṹ�� */
#define LOG_MAX 200             /* ����ʾ50����־�ļ�(200/4 = 50) */

/*
*********************************************************************************************************
*	�� �� ��: split_log
*	����˵��: ����SD�����������ݽ��зָ�,���ָ�õ����ݴ洢��ָ����������
*	��    �Σ�buf Ҫ�ָ������
*	�� �� ֵ: ֱ�Ӵ�ȫ�ֱ��������ȡ����
*********************************************************************************************************
*/
void split_log_old(char * buf)
{
    char *p;

    int i = 0,j = 0;		

    p = NULL;	
	/* �� @ ������ʼ���ַ������зָ� */
    p = strtok(buf,"@");

    while(p)
    {
		/* ����ָ���з���,������,������һ�ַָ� */
		if(strncmp(p,"\r\n",sizeof("\r\n")) == 0)
		{
			p = strtok(NULL,"@");
		}
		/* �洢��ǰ�ָ���־ */
		else
		{
			log_buf[i] = p;

			++i;			
		
			p = strtok(NULL,"@");
			if((i)%3 ==0)
			{
//				+log_ifo->total_log*3
				log_ifo[j].time_ifo  = log_buf[0];
				log_ifo[j].warn_ifo   = log_buf[1];
				log_ifo[j].warn_grade = (uint8_t)*log_buf[2];
				j ++;
				log_ifo->total_log = j;
				log_ifo[j -1].total_log = j;
				i = 0;
			}
		}
       
    }
	/* �������һ�ηָ�õ������� */
	// log_buf[i] = NULL;
	/* ��ȡ�ܵ���־���� */
	// total_log = (i)/2;
	//���Էָ��Ƿ�����
	//i - 1:ȥ�����һ������
    // for(j = 0;j<i - 1;j++)
    // {
	// 	App_Printf("%s",log_buf[j]);
    // }
}


/*
*********************************************************************************************************
*	�� �� ��: split_log
*	����˵��: ����SD�����������ݽ��зָ�,���ָ�õ����ݴ洢��ָ����������
*	��    �Σ�buf Ҫ�ָ������
*	�� �� ֵ: ֱ�Ӵ�ȫ�ֱ��������ȡ����
*********************************************************************************************************
*/
void split_log(char * buf)
{
    char *p;

    int i = 0,j = 0;		

    p = NULL;	
	/* �� @ ������ʼ���ַ������зָ� */
    p = strtok(buf,"\n");

    while(p)
    {
		/* ����ָ���з���,������,������һ�ַָ� */
		if(strncmp(p,"\r",sizeof("\r")) == 0)
		{
			p = strtok(NULL,"\n");
		}
		/* �洢��ǰ�ָ���־ */
		else
		{
			log_buf[i] = p;

			++i;			
		
			p = strtok(NULL,"\n");
			if((i)%3 ==0)
			{
//				+log_ifo->total_log*3
				log_ifo[j].time_ifo  = log_buf[0];
				log_ifo[j].warn_ifo   = log_buf[1];
				log_ifo[j].warn_grade = (uint8_t)*log_buf[2];
				j ++;
				log_ifo->total_log = j;
				log_ifo[j -1].total_log = j;
				i = 0;
			}
		}
       
    }
}
/**
 * @brief ���ô˺���,��ȡ��־�ļ���������
 * @return int: ��־�ļ���������
 */
static int get_log_line_num(void * p_arg)
{
	u8          res;
	char      * buff;
	int         log_line_num = 0;
	CPU_SR_ALLOC();
	/* ��ֹ���������� */
	(void)p_arg;
	/* ����־�ļ� */
	res = f_open(&log_file,log_pname,FA_READ);
	/* ���ļ�ʧ��,���ô��ڴ�ӡ����λ�� */
	if(res != FR_OK)
	{
		App_Printf("�ڻ�ȡ��־�ļ���������ʱ���ļ���ʧ��\r\n");
		return res;
	}
	/* ��־�ļ��򿪳ɹ� */
	OS_CRITICAL_ENTER();	                //�ٽ���
	for(;;)
	{
		res = f_read(&log_file,buff,1,&br); //��ȡ����
		if(*buff == '\n')log_line_num++;    //��ȡ�����з���,��������һ
		/* ��ɶ�ȡ�ļ� */
		if(br == 0)break;	
	}
    OS_CRITICAL_EXIT();                     //�˳��ٽ���
	/* �ر��ļ� */
    f_close(&log_file);
	/* �����ܵ���־�ļ����� */
	return log_line_num;
}

/*
*********************************************************************************************************
*	�� �� ��: _get_logs
*	����˵��: ��SD����ȡ����
*	��    �Σ�
*	�� �� ֵ: 
*********************************************************************************************************
*/
void _get_logs(void *p_arg)
{
    u8          res;
    u16         filesize;
    char        *buf;
	
	CPU_SR_ALLOC();
	/* ��ֹ���������� */
    (void)p_arg;  
    /* ��ȡ��־�ļ��Ĵ�С */
    res = f_open(&log_file,log_pname,FA_OPEN_EXISTING | FA_READ);
    filesize = f_size(&log_file);
    f_close(&log_file);
	/* �ٴδ��ļ� */
    res = f_open(&log_file,log_pname,FA_READ);
	/* ���ļ�ʧ�� */
    if(res != FR_OK)
    {
        App_Printf("��־��ȡ����\r\n");
        return;
    }
    /* �ļ��򿪳ɹ� */
    buf = mymalloc(SRAMEX,filesize);//�����ڴ�
    OS_CRITICAL_ENTER();	//�ٽ���
    //��ȡ�ļ�������
    for(;;)
	{
		res = f_read(&log_file,buf,4096,&br); //��ȡ����
		/* ��ɶ�ȡ�ļ� */
		if(br == 0)break;	
	}
    OS_CRITICAL_EXIT();//�˳��ٽ���
    f_close(&log_file);
	/* ��ʼ�ָ���־�ļ� */
    split_log(buf);
    /* �ͷ��ڴ� */
    myfree(SRAMEX,buf);	
    
}

/*
*********************************************************************************************************
*	�� �� ��: _get_logs
*	����˵��: ��SD����ȡ����
*	��    �Σ�
*	�� �� ֵ: 
*********************************************************************************************************
*/
void _get_logs1(void *p_arg)
{
    u8          res;
    u16         filesize;
	int         i;
	int         log_start;
	int         log_num = LOG_MAX;
	int         temp_locate = 0;
	/* �ȶ�ȡ50�� */
    char        *buf;
	
	CPU_SR_ALLOC();
	/* ��ֹ���������� */
    (void)p_arg;  
	log_start = get_log_line_num(0);                               //��ȡ�ļ�������
	/* ��������LOG_MAX��,ǰ�������� */
	if(log_start > log_num)
	{
		log_start = log_start - (log_num + 1);                     //��ȥҪ��ʾ��������,�õ���ʼ��ȡ��λ��
	}
	else
	{
		log_start = 0;
	}
    /* ��ȡ��־�ļ��Ĵ�С */
    res = f_open(&log_file,log_pname,FA_OPEN_EXISTING | FA_READ);
    filesize = f_size(&log_file);
    f_close(&log_file);
	/* ��ȡ��־���� */
	
	/* �ٴδ��ļ� */
    res = f_open(&log_file,log_pname,FA_READ);
	/* ���ļ�ʧ�� */
    if(res != FR_OK)
    {
        App_Printf("��־��ȡ����\r\n");
        return;
    }
    /* �ļ��򿪳ɹ� */
	/* ����ǰ�����Ĳ��� */
	OS_CRITICAL_ENTER();	                                       //�ٽ���
	for(i=0;i<log_start;)
	{
		res = f_read(&log_file,buf,1,&br);                         //��ȡ����		
		if(*buf == '\n')                                           //��ȡ����һ������
		{
			temp_locate = f_tell(&log_file);                       //��ȡ��ǰ��ָ��
			f_lseek(&log_file,temp_locate);                        //�ƶ��ļ�ָ��λ��
			i++;
		}
	}
	OS_CRITICAL_EXIT();//�˳��ٽ���
    buf = mymalloc(SRAMEX,filesize - temp_locate);//�����ڴ�
    OS_CRITICAL_ENTER();	//�ٽ���
    //��ȡ�ļ�������
    for(;;)
	{
		res = f_read(&log_file,buf,4096,&br);                      //��ȡ����
		/* ��ɶ�ȡ�ļ� */
		if(br == 0)break;	
	}
    OS_CRITICAL_EXIT();                                            //�˳��ٽ���
    f_close(&log_file);
	/* ��ʼ�ָ���־�ļ� */
    split_log(buf);
    /* �ͷ��ڴ� */
    myfree(SRAMEX,buf);	
    
}

/**
 * @brief ���Ʊ�����־���ı�����ɫ
 * @param  i: ��ǰѡ�е���־������
 * @param  j: ��ǰҪ���ƵĿ��
 */
static void draw_alarm_log_back(int i,int j)
{
	uint8_t mode = log_ifo[i].warn_grade - '0';
	/* ѡ��ڼ��������� */
	switch (j)
	{
		case 0:
			/* ���ݱ����ȼ�ѡ�񱳾���ɫ */
			switch (mode)
			{
				case 1:
					GUI_SetBkColor(GUI_WHITE);
					break;
				case 2:
					GUI_SetBkColor(GUI_YELLOW);
					break;
				case 3:
					GUI_SetBkColor(GUI_RED);
					break;			
				default:
					GUI_SetBkColor(GUI_WHITE);
					break;
			}			
			GUI_ClearRect(0,112,408,168);
			break;
		case 1:
			/* ���ݱ����ȼ�ѡ�񱳾���ɫ */
			switch (mode)
			{
				case 1:
					GUI_SetBkColor(GUI_WHITE);
					break;
				case 2:
					GUI_SetBkColor(GUI_YELLOW);
					break;
				case 3:
					GUI_SetBkColor(GUI_RED);
					break;			
				default:
					GUI_SetBkColor(GUI_WHITE);
					break;
			}
			GUI_ClearRect(0,172,408,228);
			break;
		case 2:
			/* ���ݱ����ȼ�ѡ�񱳾���ɫ */
			switch (mode)
			{
				case 1:
					GUI_SetBkColor(GUI_WHITE);
					break;
				case 2:
					GUI_SetBkColor(GUI_YELLOW);
					break;
				case 3:
					GUI_SetBkColor(GUI_RED);
					break;			
				default:
					GUI_SetBkColor(GUI_WHITE);
					break;
			}
			GUI_ClearRect(0,232,408,288);
			break;
		default:
			App_Printf("���Ʊ�����־������������,����\r\n");
			break;
	}
}


/**
 * @brief ��ʾ������־
 * @param  i: ��ǰ��ѡ��־λ��
 * @return uint8_t: 0:��ʾ�ɹ�
 */
uint8_t show_log(int i)
{
	int     j = 0;
	char    string_buf[10];

	GUI_SetFont(&GUI_FontHZ24);
	GUI_SetColor(GUI_BLACK);
	
	draw_alarm_log_back(i,j);                                                     //���Ƶ�ǰ��ѡ��־�ı����ȼ�������ɫ
	GUI_DispStringHCenterAt(log_ifo[i].time_ifo, 230, 120 - MAIN_TITLE_HEIGHT);
	GUI_DispStringHCenterAt(log_ifo[i].warn_ifo, 230, 150 - MAIN_TITLE_HEIGHT);
	if(i >= 1)
	{
		draw_alarm_log_back(i-1,j+1);
		GUI_DispStringHCenterAt(log_ifo[i -1].time_ifo, 230, 180 - MAIN_TITLE_HEIGHT);
		GUI_DispStringHCenterAt(log_ifo[i -1].warn_ifo, 230, 210 - MAIN_TITLE_HEIGHT);
	}
	
	if(i >=2)
	{
		draw_alarm_log_back(i-2,j+2);
		GUI_DispStringHCenterAt(log_ifo[i -2].time_ifo, 230, 240 - MAIN_TITLE_HEIGHT);
		GUI_DispStringHCenterAt(log_ifo[i -2].warn_ifo, 230, 270 - MAIN_TITLE_HEIGHT);
	}
	
	/* ��ʾ��ǰ��־�ļ�ʱ�ڼ��� */
	sprintf(string_buf,"%d%s%d",log_ifo->total_log-i,"/",log_ifo->total_log);
	GUI_DispStringHCenterAt("������־", 230, 70 - MAIN_TITLE_HEIGHT);
	if(log_ifo[i].total_log) GUI_DispStringHCenterAt(string_buf, 440, 200 - MAIN_TITLE_HEIGHT);
	/* ���û����־�ļ�,����ʾ */
	else GUI_DispStringHCenterAt("0/0", 440, 200- MAIN_TITLE_HEIGHT);

	return 0;
}


/*
*********************************************************************************************************
*	�� �� ��: draw_alarm_log_sheet
*	����˵��: ���Ʊ�����־���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void draw_alarm_log_sheet(void)
{
	//���û�����ɫ
	GUI_SetColor(GUI_BLUE_98);
	//���û��ʴ�С
	GUI_SetPenSize(4);
	//��ʼ���Ʊ��
	GUI_DrawLine(0,  110,480,110);
	GUI_DrawLine(0,  170,480,170);
	GUI_DrawLine(0,  230,480,230);
	GUI_DrawLine(0,  290,480,290);
	GUI_DrawLine(410,110,410,290);
}


/*
*********************************************************************************************************
*	�� �� ��: _cblogs
*	����˵��: ������־��ʾ����
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: 
*********************************************************************************************************
*/
void _cblogs(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
    int     NCode;
	int     Id;
	static  int     i = 0;

	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hItem);
            //������ҳ����������а�ť
            GUI_SetFont(&GUI_FontHZ24);
			_get_logs1(0);
			i = log_ifo->total_log - 1;
			_CreateButton_P(hItem, " ",     ID_LOG_ADD_BUTTON,      430, 125 + MAIN_TITLE_HEIGHT, 20,  20, 0,&bmup,0);
			_CreateButton_P(hItem, " ",     ID_LOG_DEC_BUTTON,      430, 250 + MAIN_TITLE_HEIGHT, 20,  20, 0,&bmdown,0);
			_CreateButton_P(hItem, " ",     ID_LOG_BACK_BUTTON,     40,  60  + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			/* ������ʱ�� */
			WM_CreateTimer(hItem, /* ������Ϣ�Ĵ��ڵľ�� */
						   0, 	                     /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
						   10,                       /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
						   0);
		case WM_PAINT:
			_PaintFrame();	
			draw_alarm_log_sheet();
			_PaintFrameSta();
			show_log(i);
			break;
		/* ��ʱ������ */
		case WM_TIMER:			
			_PaintFrameSta();		
			WM_InvalidateWindow(hItem);
		    WM_RestartTimer(pMsg->Data.v, 1000);
			break;
		case WM_INIT_DIALOG:
				break;		
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
				case ID_LOG_ADD_BUTTON: /* ���²�ѯ��־ */
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: 
							i = i + 1;
							if(i >= log_ifo->total_log)
							{
								i = log_ifo->total_log - 1;
							}
                            WM_InvalidateWindow(hItem);
							break;	
					}
					break;
                    
				case ID_LOG_DEC_BUTTON: /* ���ϲ�ѯ��־ */
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:							
							i = i - 1;
							if(i <= 0)
							{
								i = 0;
							}
                            WM_InvalidateWindow(hItem);
							break;
					}
					break;
                case ID_LOG_BACK_BUTTON: /* �������ò˵����� */
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
							i = 0;//�ָ���ʾ��־��һ��
                            _DeleteFrame();   
                            _CreateFrame(&_cbsystem_menu_1);
							break;
					}
					break;
			}
			break;
		default:
		WM_DefaultProc(pMsg);
	}
}

