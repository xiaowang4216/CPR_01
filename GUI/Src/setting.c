/**************************************************
	* @File Name: setting.c
	* @brief GUI���Լ��������ý���
	* @author ���ָ� (2891854535@qq.com)
	* @Version : 1.0
	* @date 2022-08-31
	* 
***************************************************/
#include "gui_public.h"
#include "set_menu.h"
#include "setting.h"

/***
 * ���ý���
 * ͨ���˽���������������
 * ������С
 * ������ʽ
 ***/

//������任��ť
#define ID_LANGUAGE_LCHANGE_BUTTON (GUI_ID_USER + 0x00)
//�����ұ任��ť
#define ID_LANGUAGE_RCHANGE_BUTTON (GUI_ID_USER + 0x01)
//�����Ӱ�ť
#define ID_VOLUME_INCEA_BUTTON     (GUI_ID_USER + 0x02)
//��������ť
#define ID_VOLUME_REDUC_BUTTON     (GUI_ID_USER + 0x03)
//���ý��淵�ذ�ť
#define ID_SETTING_BACK_BUTTON     (GUI_ID_USER + 0x04)


/**************************************************
	* 
	* @brief ��������
	* 
***************************************************/
int LANGUAGE_SELECT = 0;  /* �������к�,Ĭ������ */
int ALARM_VOLUME    = 0; /* ϵͳ��ʾ������С */


/**************************************************
	* 
	* @brief ��̬����,��������
	* 
***************************************************/
static const char * _language[][1] = {
    {"����"},
    {"English"},
    {"������ܧڧ�"},
};


/*
*********************************************************************************************************
*	�� �� ��: _get_language
*	����˵��: ��ȡ��ǰ�����ַ����ĵ�ַ
*	��    �Σ�Index  Ҫ��ʾ�����ַ��������
*	�� �� ֵ: p      ����Ҫ��ʾ�����ַ�����ַ
*********************************************************************************************************
*/
static const char * _get_language(void)
{
    const char *p;

    p = NULL;
	/* ��ȡ��ǰ���� */
    p = * _language[LANGUAGE_SELECT];

    return p;
}

/*
*********************************************************************************************************
*	�� �� ��: _cbsetting
*	����˵��: ���ý���,������������,��������,������ʽ����
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbsetting(WM_MESSAGE* pMsg) 
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
			_CreateButton(hItem, "jie",   ID_VOLUME_REDUC_BUTTON,      230, 170 + MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "jia",   ID_VOLUME_INCEA_BUTTON,      420, 170 + MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "zuo",   ID_LANGUAGE_LCHANGE_BUTTON,  230, 120 + MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "you",   ID_LANGUAGE_RCHANGE_BUTTON,  420, 120 + MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton_P(hItem, " ",  ID_SETTING_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			WM_CreateTimer(hItem, /* ������Ϣ�Ĵ��ڵľ�� */
						   0, 	                     /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
						   10,                       /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
						   0);
//		 case WM_KEY:
//            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
//            { 
//				case GUI_KEY_ESCAPE:
//                    GUI_EndDialog(hItem, 1);
//                    break;
//				case GUI_KEY_TAB:
//					WM_SetFocusOnNextChild(hItem);
//					break;
//            }
//            break;
		case WM_PAINT:
			_PaintFrame();		
			_PaintFrameSta();
			GUI_SetFont(&GUI_FontHZ24);
			GUI_DispStringHCenterAt("����", 70, 120 + MAIN_TITLE_HEIGHT);
            GUI_DispStringHCenterAt("����", 70, 170 + MAIN_TITLE_HEIGHT);
            GUI_DispStringHCenterAt("��������", 70, 220 + MAIN_TITLE_HEIGHT);
            GUI_DispStringHCenterAt(_get_language(), 330, 120 + MAIN_TITLE_HEIGHT);
            GUI_DispDecAt(ALARM_VOLUME,320,170 + MAIN_TITLE_HEIGHT,2);
            GUI_DispStringHCenterAt("����", 330, 220 + MAIN_TITLE_HEIGHT);
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
				case ID_VOLUME_REDUC_BUTTON: /* ���ٵ�ǰ���� */
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //��ť�����²��ͷ�
							ALARM_VOLUME --;
							if(ALARM_VOLUME < 0)
							{
								ALARM_VOLUME = 7;
							}
                            WM_InvalidateWindow(hItem);
							break;	
					}
					break;
                    
				case ID_VOLUME_INCEA_BUTTON: /* ���ӵ�ǰ���� */
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							ALARM_VOLUME ++;
							if(ALARM_VOLUME >= 8)
							{
								ALARM_VOLUME = 0;
							}
                            WM_InvalidateWindow(hItem);
							break;
					}
					break;
                case ID_LANGUAGE_RCHANGE_BUTTON: /* �л���ǰ���� */
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							LANGUAGE_SELECT ++;
							if(LANGUAGE_SELECT >= 3)
							{
								LANGUAGE_SELECT = 0;
							}
							WM_InvalidateWindow(hItem);
							break;
					}
					break;
				case ID_LANGUAGE_LCHANGE_BUTTON: /* �л���ǰ���� */
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							LANGUAGE_SELECT --;
							if(LANGUAGE_SELECT < 0)
							{
								LANGUAGE_SELECT = 2;
							}
							WM_InvalidateWindow(hItem);
							break;
					}
					break;
                case ID_SETTING_BACK_BUTTON: /* �������ò˵����� */
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
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





