/**************************************************
	* @File Name: servic.c
	* @brief GUI���������
	* @author ���ָ� (2891854535@qq.com)
	* @Version : 1.0
	* @date 2022-08-31
	* 
***************************************************/
#include "exfuns.h"
#include "fattester.h"
#include "ff.h"
#include "malloc.h"
#include "os_control.h"
#include "servic.h"
#include "set_menu.h"

/***
 * �򿪴�ҳ��,��ʾ�����
 * ��ʾ����
 * ��ʾ��������
 * ��֤�����Ƿ���ȷ
 * �������,��������
 * ������ȷ,������һҳ��
 ***/




//�����������ȡ����ť
#define ID_SERVIC_BACK_BUTTON    (GUI_ID_USER + 0x00)
//�����������ȷ�ϰ�ť
#define ID_SERVIC_VERIFY_BUTTON    (GUI_ID_USER + 0x01)
//�����������ȡ����ť
#define ID_SERVIC_CANCEL_BUTTON    (GUI_ID_USER + 0x02)
#define ID_SERVIC_0_BUTTON    (GUI_ID_USER + 0x03)
#define ID_SERVIC_1_BUTTON    (GUI_ID_USER + 0x04)
#define ID_SERVIC_2_BUTTON    (GUI_ID_USER + 0x05)
#define ID_SERVIC_3_BUTTON    (GUI_ID_USER + 0x06)
#define ID_SERVIC_4_BUTTON    (GUI_ID_USER + 0x07)
#define ID_SERVIC_5_BUTTON    (GUI_ID_USER + 0x08)
#define ID_SERVIC_6_BUTTON    (GUI_ID_USER + 0x09)
#define ID_SERVIC_7_BUTTON    (GUI_ID_USER + 0x0A)
#define ID_SERVIC_8_BUTTON    (GUI_ID_USER + 0x0B)
#define ID_SERVIC_9_BUTTON    (GUI_ID_USER + 0x0C)

#define NUMPAD_BKCOLOR            GUI_LIGHTGRAY
#define NUMPAD_EFFECT             (&WIDGET_Effect_3D2L)
#define NUMPAD_BORDER_X             160
#define NUMPAD_BORDER_Y             140
#define NUMPAD_PIN                1685

#define NUMPAD_BUTTON_BKCOLOR0    0xB8B8B8              /* Unpressed */
#define NUMPAD_BUTTON_BKCOLOR1    0xE0E0E0              /* Pressed   */
#define NUMPAD_BUTTON_BKCOLOR2    0xC0C0C0              /* Disabled  */
#define NUMPAD_BUTTON_COLOR0      0x700000              /* Unpressed */
#define NUMPAD_BUTTON_COLOR1      0x700000              /* Pressed   */
#define NUMPAD_BUTTON_RED0        0x0060FF              /* Unpressed */
#define NUMPAD_BUTTON_RED1        0x0030E8              /* Pressed   */
#define NUMPAD_BUTTON_BLUE0       0x00FF00              /* Unpressed */
#define NUMPAD_BUTTON_BLUE1       0x00FF7F              /* Pressed   */
#define NUMPAD_BUTTON_GREEN0      0x00B040              /* Unpressed */
#define NUMPAD_BUTTON_GREEN1      0x00D000              /* Pressed   */
#define NUMPAD_BUTTON_FONT        (&GUI_FontHZ16)
#define NUMPAD_BUTTON_EFFECT      (&WIDGET_Effect_3D1L)
#define NUMPAD_BUTTON_WIDTH       80
#define NUMPAD_BUTTON_HEIGHT      36
#define NUMPAD_BUTTON_DIST_X      7
#define NUMPAD_BUTTON_DIST_Y      7


/**************************************************
	* 
	* @brief �������� 
	* 
***************************************************/
int          _PIN_Digits;             /* ��ǰ���������λ�� */
char         password[6] = "123456";  /* Ĭ������ */
char         in_password[6];          /* ���̲ɼ����� */
char         temp_password[6];        /* �޸�������ʱ���� */
int          PWD_SELECT = 0;          /* ����������ʾ���� */
static FIL   pwd_file;
/* �����뱣�浽�ⲿFLASH,�ñ��������ļ�·�����ļ��� */
const TCHAR* pwd_pname = "1:pwd.txt";
/**************************************************
	* 
	* @brief ��̬����
	* 
***************************************************/
static const char *_pwd_hint[][1] = {
	{"������"},
	{"������"},
	{"����һ��"}
};

//ά���˵�����
void _cbservic_menu(WM_MESSAGE* pMsg) ;
//�޸��������
void _cbpwd_change(WM_MESSAGE* pMsg) ;
//���У׼����
void _cbcalib_depth(WM_MESSAGE* pMsg) ;
//������У׼����
void _cbcalib_lvdt(WM_MESSAGE* pMsg) ;

/*
*********************************************************************************************************
*	�� �� ��: _get_password
*	����˵��: ��FLASH��ȡ����
*	��    �Σ�
*	�� �� ֵ: 
*********************************************************************************************************
*/

void _get_password(void *p_arg)
{
    u8 res;
	char        string_buf[20];
	int i;
	
	char bufer[20];
	
	CPU_SR_ALLOC();
		
    (void)p_arg;
	
	
       

    //��ȡ��־�ļ��Ĵ�С
    res = f_open(&pwd_file,pwd_pname,FA_OPEN_EXISTING | FA_READ);
	//���ļ�������,���½�һ��,����д��Ĭ������
    if(res != FR_OK)
    {
		OS_CRITICAL_ENTER();	//�ٽ���
		memset(string_buf,0,sizeof(string_buf));
		sprintf(string_buf,"%s\r\n",password);
        res = f_open(&pwd_file,pwd_pname, FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
		res = f_write(&pwd_file,string_buf, strlen(string_buf), &bw);
		if(res != FR_OK)
		{
			App_Printf("д��ʧ��\r\n");
		}
		else
		{
			App_Printf("д��ɹ�\r\n");
		}
		OS_CRITICAL_EXIT();//�˳��ٽ���
		f_close(&pwd_file);
    }			
    f_close(&pwd_file);
	
	res = f_open(&pwd_file,pwd_pname,FA_READ);
    //�ļ��򿪳ɹ�

    OS_CRITICAL_ENTER();	//�ٽ���
    //��ȡ�ļ�������
	res = f_read(&pwd_file,bufer,22,&br); //��ȡ����
    for(i=0;i<6;i++)
	{
		password[i] = bufer[i];	
	}
    OS_CRITICAL_EXIT();//�˳��ٽ���
    f_close(&pwd_file);
    
    
}

/*
*********************************************************************************************************
*	�� �� ��: _change_password
*	����˵��: ���޸ĺ������洢��flash
*	��    �Σ�
*	�� �� ֵ: 
*********************************************************************************************************
*/
void _change_password(void *p_arg)
{
    u8 res;
    //��ֹ����������
	
	CPU_SR_ALLOC();
		
    (void)p_arg;
	
    //��ȡ��־�ļ��Ĵ�С
    res = f_open(&pwd_file,pwd_pname,FA_OPEN_EXISTING | FA_READ);
	//���ļ�������,���½�һ��,����д��Ĭ������
    if(res == FR_NO_FILE)
    {
		OS_CRITICAL_ENTER();	//�ٽ���
		_change_password(0);
		OS_CRITICAL_EXIT();//�˳��ٽ���
		f_close(&pwd_file);
    }			
    f_close(&pwd_file);
	
	res = f_open(&pwd_file,pwd_pname,FA_WRITE);
    //�ļ��򿪳ɹ�
    OS_CRITICAL_ENTER();	//�ٽ���
    //������д���ļ�
	res = f_write(&pwd_file,password,strlen(password),&br); //��ȡ����

    OS_CRITICAL_EXIT();//�˳��ٽ���
    f_close(&pwd_file);
    
}

/*
*********************************************************************************************************
*	�� �� ��: _get_pwd_hint
*	����˵��: ��ȡ��ǰ����������ʾ
*	��    �Σ�
*	�� �� ֵ: p      ���ص�ǰ������ʾ
*********************************************************************************************************
*/
static const char * _get_pwd_hint(void)
{
    const char *p;

    p = NULL;
	/* ��ȡ��ǰӦ�ý��е�������� */
    p = * _pwd_hint[PWD_SELECT];

    return p;
}


/*
*********************************************************************************************************
*	�� �� ��: _CreateNumPadButton
*	����˵��: ����С��������Ҫ�İ�ť
*	��    �Σ�hParent  ������
*             pText    ��������ʾ���ı�
*             Id       ��ťId         
*             x        x������
*             y        y������
*             w        ��ť��
*             h        ��ť��
*             Color    ��ť��ɫ
*             PressedColor  ��ť�����º����ɫ
*             Enable   ʹ�ܻ��߽�ֹ״̬
*             TextId   �ı���ID          
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static WM_HWIN _CreateNumPadButton(WM_HWIN hParent, const char* pText, int Id,
                                   int x, int y, int w, int h,
                                   GUI_COLOR Color, GUI_COLOR PressedColor, int Enable, unsigned TextId)
{
	WM_HWIN hButton;
	
	
	hButton = BUTTON_CreateEx(x, y, w, h, hParent, WM_CF_SHOW, 0, Id);
	if (Enable) 
	{
		if (TextId) 
		{
			pText = _GetLang(TextId);
		}
		
		BUTTON_SetText(hButton, pText);
	} 
	else 
	{
		WM_DisableWindow(hButton);
	}
	
	BUTTON_SetFont      (hButton,    NUMPAD_BUTTON_FONT);
	BUTTON_SetBkColor   (hButton, 0, NUMPAD_BUTTON_BKCOLOR0);
	BUTTON_SetBkColor   (hButton, 1, NUMPAD_BUTTON_BKCOLOR1);
	BUTTON_SetBkColor   (hButton, 2, NUMPAD_BUTTON_BKCOLOR2);
	BUTTON_SetTextColor (hButton, 0, Color);
	BUTTON_SetTextColor (hButton, 1, PressedColor);
	BUTTON_SetTextColor (hButton, 2, Color);
	BUTTON_SetFocussable(hButton, 1);
	WIDGET_SetEffect    (hButton, NUMPAD_BUTTON_EFFECT);
	
	return hButton;
}



/*
*********************************************************************************************************
*	�� �� ��: _cbNumPad
*	����˵��: С���̵Ļص�����
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbNumPad(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
	/* ������ʱ�� */
    int     NCode;
	int     Id;
    int     i;
    char    string_buf[6];
    int     bx, by, bw, bh, dx, dy;
    /* λ�úʹ�С */
	bx = NUMPAD_BORDER_X;
	by = NUMPAD_BORDER_Y;
	bw = NUMPAD_BUTTON_WIDTH;
	bh = NUMPAD_BUTTON_HEIGHT;
	dx = NUMPAD_BUTTON_DIST_X;
	dy = NUMPAD_BUTTON_DIST_Y;

	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hItem);
            //������ҳ����������а�ť
            GUI_SetFont(&GUI_FontHZ24);
			_CreateButton_P(hItem, " ",  ID_SERVIC_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
            _PIN_Digits = 0;
			_get_password(0);
            /* Ϊ���̴��ڴ�����ť */
            _CreateNumPadButton(hItem, "1",    ID_SERVIC_1_BUTTON,      bx + 0*(bw+dx),   by + 0*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "2",    ID_SERVIC_2_BUTTON,      bx + 1*(bw+dx),   by + 0*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "3",    ID_SERVIC_3_BUTTON,      bx + 2*(bw+dx),   by + 0*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "4",    ID_SERVIC_4_BUTTON,      bx + 0*(bw+dx),   by + 1*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "5",    ID_SERVIC_5_BUTTON,      bx + 1*(bw+dx),   by + 1*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "6",    ID_SERVIC_6_BUTTON,      bx + 2*(bw+dx),   by + 1*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "7",    ID_SERVIC_7_BUTTON,      bx + 0*(bw+dx),   by + 2*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "8",    ID_SERVIC_8_BUTTON,      bx + 1*(bw+dx),   by + 2*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "9",    ID_SERVIC_9_BUTTON,      bx + 2*(bw+dx),   by + 2*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "ȡ��", ID_SERVIC_CANCEL_BUTTON, bx + 0*(bw+dx),   by + 3*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0) ;
            _CreateNumPadButton(hItem, "0",    ID_SERVIC_0_BUTTON,      bx + 1*(bw+dx),   by + 3*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "ȷ��", ID_SERVIC_VERIFY_BUTTON, bx + 2*(bw+dx),   by + 3*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);

			WM_CreateTimer(hItem, /* ������Ϣ�Ĵ��ڵľ�� */
						   0, 	                     /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
						   10,                       /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
						   0);
		case WM_PAINT:
			_PaintFrame();
			_PaintFrameSta();		
			GUI_SetFont(&GUI_FontHZ24);
			GUI_DispStringHCenterAt("ά��", 230, 70 - MAIN_TITLE_HEIGHT);
            GUI_DispStringHCenterAt("����", 70, 160 - MAIN_TITLE_HEIGHT);
			
			memset(string_buf,0,sizeof(string_buf));
            for(i = 0;i<_PIN_Digits;i++)
            {
                string_buf[i] = '*';
            }
            GUI_DispStringHCenterAt(string_buf, 70, 220 - MAIN_TITLE_HEIGHT);
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
                case ID_SERVIC_BACK_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            _DeleteFrame();   
                            _CreateFrame(&_cbsystem_menu_3);
							break;
					}
					break;
                case ID_SERVIC_1_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '1';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_2_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '2';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_3_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '3';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_4_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '4';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_5_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '5';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_6_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '6';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_7_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '7';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_8_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '8';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_9_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '9';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_0_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '0';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_CANCEL_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ��һλ����*/
                            if(_PIN_Digits>=0)
                            {
                                in_password[_PIN_Digits] = '*';
                                _PIN_Digits --;
                                if(_PIN_Digits<0)
                                {
                                    _PIN_Digits = 0;
                                }
                            }
							break;
					}
					break;
                case ID_SERVIC_VERIFY_BUTTON: //ȷ��,����������֤
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
                            if(strncmp(password,in_password,sizeof(in_password)) == 0)
                            {
                                /* ɾ����������Ľ��� */
                                _PIN_Digits = 0;
                                for(i=0;i<6;i++)
                                {
                                    in_password[i] = '*';
                                }
                                _DeleteFrame();   
                                _CreateFrame(&_cbservic_menu);
                            }//������������
							else
							{
								_PIN_Digits = 0;
                                for(i=0;i<6;i++)
                                {
                                    in_password[i] = '*';
                                }
							}

							break;
					}
					break;
			}
			break;
		default:
		WM_DefaultProc(pMsg);
	}
}




/*
*********************************************************************************************************
*	�� �� ��: _cbservic_menu
*	����˵��: ������־��ʾ����
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: 
*********************************************************************************************************
*/
void _cbservic_menu(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
	/* ������ʱ�� */
    int     NCode;
	int     Id;

	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hItem);
            //������ҳ����������а�ť
            GUI_SetFont(&GUI_FontHZ24);
			_CreateButton_P(hItem, " ",  ID_SERVIC_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton(hItem, "�޸�����",    ID_SERVIC_1_BUTTON,      0,   122 + MAIN_TITLE_HEIGHT, 400,  66, 0);
            _CreateButton(hItem, "λ�ƴ�����У׼",    ID_SERVIC_2_BUTTON,      0,   188 + MAIN_TITLE_HEIGHT, 400,  66, 0);
            _CreateButton(hItem, "����", ID_SERVIC_CANCEL_BUTTON, 400,   122 + MAIN_TITLE_HEIGHT,   80,  66,0) ;
            _CreateButton(hItem, "��ѹ���У׼",    ID_SERVIC_0_BUTTON,      0,   254 + MAIN_TITLE_HEIGHT,    400,  66, 0);
            _CreateButton(hItem, "����", ID_SERVIC_VERIFY_BUTTON, 400,   254 + MAIN_TITLE_HEIGHT, 80,  66, 0);
			WM_CreateTimer(hItem, /* ������Ϣ�Ĵ��ڵľ�� */
						   0, 	                     /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
						   10,                       /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
						   0);
		case WM_PAINT:
			_PaintFrame();	
			_PaintFrameSta();		
			GUI_SetFont(&GUI_FontHZ24);
		
			GUI_DispStringHCenterAt("ά��", 230, 70 - MAIN_TITLE_HEIGHT);
			GUI_DispStringHCenterAt("1/1", 440, 200 + MAIN_TITLE_HEIGHT);
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
                case ID_SERVIC_BACK_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            _DeleteFrame();   
                            _CreateFrame(&_cbsystem_menu_3);
							break;
					}
					break;
				case ID_SERVIC_1_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            _DeleteFrame();   
                            _CreateFrame(&_cbpwd_change);
							break;
					}
					break;
				case ID_SERVIC_2_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            _DeleteFrame();   
                            _CreateFrame(&_cbcalib_lvdt);
							break;
					}
					break;
				case ID_SERVIC_0_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            _DeleteFrame();   
                            _CreateFrame(&_cbcalib_depth);
							break;
					}
					break;
			}
			break;
		default:
		WM_DefaultProc(pMsg);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: _cbpwd_change
*	����˵��: �޸�����ص�����
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: 
*********************************************************************************************************
*/
void _cbpwd_change(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
	/* ������ʱ�� */
    int     NCode;
	int     Id;
	int     i;
    char    string_buf[6];
	int     bx, by, bw, bh, dx, dy;
    /* λ�úʹ�С */
	bx = NUMPAD_BORDER_X;
	by = NUMPAD_BORDER_Y;
	bw = NUMPAD_BUTTON_WIDTH;
	bh = NUMPAD_BUTTON_HEIGHT;
	dx = NUMPAD_BUTTON_DIST_X;
	dy = NUMPAD_BUTTON_DIST_Y;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hItem);
            //������ҳ����������а�ť
            GUI_SetFont(&GUI_FontHZ24);
			_CreateButton_P(hItem, " ",  ID_SERVIC_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
            _PIN_Digits = 0;
            /* Ϊ���̴��ڴ�����ť */
            _CreateNumPadButton(hItem, "1", ID_SERVIC_1_BUTTON,         bx + 0*(bw+dx),   by + 0*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "2", ID_SERVIC_2_BUTTON,         bx + 1*(bw+dx),   by + 0*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "3", ID_SERVIC_3_BUTTON,         bx + 2*(bw+dx),   by + 0*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "4", ID_SERVIC_4_BUTTON,         bx + 0*(bw+dx),   by + 1*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "5", ID_SERVIC_5_BUTTON,         bx + 1*(bw+dx),   by + 1*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "6", ID_SERVIC_6_BUTTON,         bx + 2*(bw+dx),   by + 1*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "7", ID_SERVIC_7_BUTTON,         bx + 0*(bw+dx),   by + 2*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "8", ID_SERVIC_8_BUTTON,         bx + 1*(bw+dx),   by + 2*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "9", ID_SERVIC_9_BUTTON,         bx + 2*(bw+dx),   by + 2*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "ȡ��", ID_SERVIC_CANCEL_BUTTON, bx + 0*(bw+dx),   by + 3*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0) ;
            _CreateNumPadButton(hItem, "0", ID_SERVIC_0_BUTTON,         bx + 1*(bw+dx),   by + 3*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "ȷ��", ID_SERVIC_VERIFY_BUTTON, bx + 2*(bw+dx),   by + 3*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
			WM_CreateTimer(hItem, /* ������Ϣ�Ĵ��ڵľ�� */
						   0, 	                     /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
						   10,                       /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
						   0);
		case WM_PAINT:
			_PaintFrame();	
			_PaintFrameSta();		
			GUI_SetFont(&GUI_FontHZ24);
		
			GUI_DispStringHCenterAt("�޸�����", 230, 70 - MAIN_TITLE_HEIGHT);
			GUI_DispStringHCenterAt(_get_pwd_hint(), 70, 160 - MAIN_TITLE_HEIGHT);
			memset(string_buf,0,sizeof(string_buf));
            for(i = 0;i<_PIN_Digits;i++)
            {
                string_buf[i] = in_password[i];
            }
            GUI_DispStringHCenterAt(string_buf, 70, 220 - MAIN_TITLE_HEIGHT);
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
                case ID_SERVIC_BACK_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            _DeleteFrame();   
                            _CreateFrame(&_cbservic_menu);
							break;
					}
					break;
				 case ID_SERVIC_1_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '1';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_2_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '2';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_3_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '3';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_4_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '4';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_5_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '5';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_6_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '6';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_7_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '7';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_8_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '8';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_9_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '9';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_0_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '0';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_CANCEL_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ��һλ����*/
                            if(_PIN_Digits>=0)
                            {
                                in_password[_PIN_Digits] = '*';
                                _PIN_Digits --;
                                if(_PIN_Digits<0)
                                {
                                    _PIN_Digits = 0;
                                }
                            }
							break;
					}
					break;
                case ID_SERVIC_VERIFY_BUTTON: //ȷ��,����������֤
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							switch (PWD_SELECT)
							{
								case 0://��֤����Ա����
									if(strncmp(password,in_password,sizeof(in_password)) == 0)
									{
										_PIN_Digits = 0;
										for(i=0;i<6;i++)
										{
											in_password[i] = '*';
										}
										PWD_SELECT = 1;
									}//������������
									else
									{
										_PIN_Digits = 0;
										for(i=0;i<6;i++)
										{
											in_password[i] = '*';
										}
									}
									break;
								//��һ����������
								case 1:
									if(_PIN_Digits == 6)
									{
										for(i=0;i<6;i++)
										{
											temp_password[i] = in_password[i];
										}
										PWD_SELECT = 2;
										_PIN_Digits = 0;
										memset(in_password,0,sizeof(in_password));
									}
									else
									{
										App_Printf("��������λ����");
									}
									break;
								//��֤���������Ƿ�һ��
								case 2:
									if(_PIN_Digits == 6)
									{
										//��������һ��
										if(strncmp(temp_password,in_password,sizeof(in_password)) == 0)
										{
											for(i=0;i<6;i++)
											{
												password[i] = in_password[i];
											}
											_change_password(0);
											PWD_SELECT = 0;
											_PIN_Digits = 0;
											memset(in_password,0,sizeof(in_password));
											_DeleteFrame();   
											_PaintFrameSta();
											_CreateFrame(&_cbservic_menu);

										}
										else
										{
											App_Printf("��������ͬ����");
										}
									}
									else
									{
										App_Printf("��������λ����");
									}
									break;
								default:
									break;
							}
                            

							break;
					}
					break;
			}
			break;
		default:
		WM_DefaultProc(pMsg);
	}
}


/*
*********************************************************************************************************
*	�� �� ��: _cbcalib_lvdt
*	����˵��: λ�ƴ�����У׼����
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: 
*********************************************************************************************************
*/
void _cbcalib_lvdt(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
	/* ������ʱ�� */
    int     NCode;
	int     Id;

	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hItem);
            //������ҳ����������а�ť
            GUI_SetFont(&GUI_FontHZ24);
			_CreateButton_P(hItem, " ",  ID_SERVIC_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
            _CreateButton(hItem, "��ʾ:��\"��ʼ\"����У׼", ID_SERVIC_CANCEL_BUTTON, 0,   188 + MAIN_TITLE_HEIGHT,   480,  66,0) ;
            _CreateButton(hItem, "��ʼ", ID_SERVIC_VERIFY_BUTTON, 0,   254 + MAIN_TITLE_HEIGHT, 480,  66, 0);
			WM_CreateTimer(hItem, /* ������Ϣ�Ĵ��ڵľ�� */
						   0, 	                     /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
						   10,                       /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
						   0);
		case WM_PAINT:
			_PaintFrame();
			_PaintFrameSta();		
			GUI_SetFont(&GUI_FontHZ24);
		
			GUI_DispStringHCenterAt("λ�ƴ�����У׼", 230, 70 - MAIN_TITLE_HEIGHT);
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
                case ID_SERVIC_BACK_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            _DeleteFrame();   
                            _CreateFrame(&_cbservic_menu);
							break;
					}
					break;
				case ID_SERVIC_1_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            _DeleteFrame();   
                            _CreateFrame(&_cbpwd_change);
							break;
					}
					break;
			}
			break;
		default:
		WM_DefaultProc(pMsg);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: _cbcalib_depth
*	����˵��: ��ѹ���У׼����
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: 
*********************************************************************************************************
*/
void _cbcalib_depth(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
	/* ������ʱ�� */
    int     NCode;
	int     Id;

	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hItem);
            //������ҳ����������а�ť
            GUI_SetFont(&GUI_FontHZ24);
			_CreateButton_P(hItem, " ",  ID_SERVIC_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton(hItem, "�޸�����",    ID_SERVIC_1_BUTTON,      0,   122 + MAIN_TITLE_HEIGHT, 400,  66, 0);
            _CreateButton(hItem, "λ�ƴ�����У׼",    ID_SERVIC_2_BUTTON,      0,   188 + MAIN_TITLE_HEIGHT, 400,  66, 0);
            _CreateButton(hItem, "ȡ��", ID_SERVIC_CANCEL_BUTTON, 400,   122 + MAIN_TITLE_HEIGHT,   80,  66,0) ;
            _CreateButton(hItem, "��ѹ���У׼",    ID_SERVIC_0_BUTTON,      0,   254 + MAIN_TITLE_HEIGHT,    400,  66, 0);
            _CreateButton(hItem, "ȷ��", ID_SERVIC_VERIFY_BUTTON, 400,   254 + MAIN_TITLE_HEIGHT, 80,  66, 0);
			WM_CreateTimer(hItem, /* ������Ϣ�Ĵ��ڵľ�� */
						   0, 	                     /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
						   10,                       /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
						   0);
		case WM_PAINT:
			_PaintFrame();	
			_PaintFrameSta();
			GUI_SetFont(&GUI_FontHZ24);
		
			GUI_DispStringHCenterAt("��ѹ���У׼", 230, 70 - MAIN_TITLE_HEIGHT);
			GUI_DispStringHCenterAt("1/1", 440, 200 + MAIN_TITLE_HEIGHT);
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
                case ID_SERVIC_BACK_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            _DeleteFrame();   
                            _CreateFrame(&_cbservic_menu);
							break;
					}
					break;
				case ID_SERVIC_1_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            _DeleteFrame();   
                            _CreateFrame(&_cbpwd_change);
							break;
					}
					break;
			}
			break;
		default:
		WM_DefaultProc(pMsg);
	}
}



