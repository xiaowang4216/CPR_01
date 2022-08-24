#include "date_tr.h"
#include "set_menu.h"
#include "alarm_log.h"
#include "time.h"

/***
 * ƽ�����������ݴ洢��SD��
 * �������ݴ�����沢ѡ��U�̴����ʱ��
 * ѭ�����U���Ƿ����,����⵽U�̲����ʱ��,����ר״̬��ͼ��
 * ��ʾU�̲���ͼ��
 * �����ʼ�������ݰ�ť,��ʼ��������
 ***/



/***
 * ��������,һ������ѡ���䷽ʽ
 * ��һ�����䷽ʽΪ��������
 * �ڶ������䷽ʽΪU�̴���
 ***/

/***
 * �������䷽ʽ
 ***/

/***
 * U�̴��䷽ʽ
 * ��U�̴������
 * �ȴ�U������
 * U��������
 * ������䰴ť
 * ��ʼ��������
 ***/
/***
 * ʵ�����ݴ�������Ҫ�İ�ť
 ***/
//����ѡ��ť
#define ID_BULETOOTH_SELECT_BUTTON (GUI_ID_USER + 0x00)
//U��ѡ��ť
#define ID_U_DISK_SELECT_BOTTON    (GUI_ID_USER + 0x01)
//�����ƶ��б�
//�����ƶ��б�
//ȷ������
//��ʼ��������
#define ID_STSRT_TRANS_BOTTON      (GUI_ID_USER + 0x05) 
//������һ���˵�
#define ID_DATE_BACK_BUTTON        (GUI_ID_USER + 0x06)


//U���Ƿ����
int U_DISK_READY = 0;


extern u8           copy_sd_data_to_u;
extern OS_SEM       AppPrintfSemp;	/* ����printf���� */
extern OS_SEM       SEM_SYNCH;	    /* ����ͬ�� */

void _cbtrmethod_menu(WM_MESSAGE* pMsg);
void _cbtrdate(WM_MESSAGE* pMsg);

int U_Disk_Flag = 0;
/*
*********************************************************************************************************
*	�� �� ��: _cbtrmethod_menu
*	����˵��: ���ݴ��䷽ʽѡ��˵�����
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbtrmethod_menu(WM_MESSAGE* pMsg)
{
    WM_HWIN hItem = pMsg->hWin;
    int         NCode;
	int         Id;
	OS_ERR      err;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hItem);
            //������ҳ����������а�ť
			GUI_SetFont(&GUI_FontHZ24);
			_CreateButton_P(hItem, " ",  ID_DATE_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton(hItem, "����",   ID_BULETOOTH_SELECT_BUTTON, 50, 120 + MAIN_TITLE_HEIGHT, 190,  160, 0);
			_CreateButton(hItem, "U��",      ID_U_DISK_SELECT_BOTTON, 240, 120 + MAIN_TITLE_HEIGHT, 190,  160, 0);
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
            GUI_SetFont(&GUI_FontHZ32);
            GUI_DispStringHCenterAt("���ݴ���", 220, 70 - MAIN_TITLE_HEIGHT);		           
			break;
		/* ��ʱ������ */
		case WM_TIMER:			
			_PaintFrameSta();		
			WM_InvalidateWindow(hItem);
		    WM_RestartTimer(pMsg->Data.v, 1000);
			break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
                case ID_DATE_BACK_BUTTON: //�������ò˵�
                switch(NCode) 
                {
                    case WM_NOTIFICATION_CLICKED:
                        break;
                    case WM_NOTIFICATION_RELEASED:
                        _DeleteFrame();                    
                        _CreateFrame(&_cbsystem_menu_2);
                        break;
                }
                break;
                case ID_U_DISK_SELECT_BOTTON: //��U�̴������
                switch(NCode) 
                {
                    case WM_NOTIFICATION_CLICKED:
                        break;
                    case WM_NOTIFICATION_RELEASED:
                        _DeleteFrame();                    
                        _CreateFrame(&_cbtrdate);
                        break;
                }
                break;
				case ID_BULETOOTH_SELECT_BUTTON: //���������ť
                switch(NCode) 
                {
                    case WM_NOTIFICATION_CLICKED:
                        break;
                    case WM_NOTIFICATION_RELEASED:
                        OSSemPost((OS_SEM *)&SEM_SYNCH,  
								  (OS_OPT  )OS_OPT_POST_1,
								  (OS_ERR *)&err); 
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
*	�� �� ��: _cbtrdate
*	����˵��: ���ݴ������
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbtrdate(WM_MESSAGE* pMsg)
{
    WM_HWIN hItem = pMsg->hWin;
    int         NCode;
	int         Id;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hItem);
            //������ҳ����������а�ť
			GUI_SetFont(&GUI_FontHZ24);
            _CreateButton_P(hItem, " ",  ID_DATE_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton(hItem, "��ʼ",      ID_STSRT_TRANS_BOTTON, 240, 120 + MAIN_TITLE_HEIGHT, 190,  160, 0);
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
            GUI_DispStringHCenterAt("���ݴ���", 220, 70 - MAIN_TITLE_HEIGHT);		
            if(U_DISK_READY)//���U���Ѿ�����
            {
                GUI_DispStringHCenterAt("U�� ����", 100, 120 - MAIN_TITLE_HEIGHT);	
            }
            else//��ʾ����U��
            {
                GUI_DispStringHCenterAt("U�� �γ�", 100, 120 - MAIN_TITLE_HEIGHT);	
            }
			break;
		/* ��ʱ������ */
		case WM_TIMER:			
			_PaintFrameSta();		
			WM_InvalidateWindow(hItem);
		    WM_RestartTimer(pMsg->Data.v, 1000);
			break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
                case ID_DATE_BACK_BUTTON: //�������ò˵�
                switch(NCode) 
                {
                    case WM_NOTIFICATION_CLICKED:
                        break;
                    case WM_NOTIFICATION_RELEASED:
                        _DeleteFrame();                    
                        _CreateFrame(&_cbtrmethod_menu);
                        break;
                }
                break;
				case ID_STSRT_TRANS_BOTTON: //��ʼ��������
                switch(NCode) 
                {
                    case WM_NOTIFICATION_CLICKED:
                        break;
                    case WM_NOTIFICATION_RELEASED:
						/* ��ʼ�����ļ���U�� */
						copy_sd_data_to_u = 1;
                        break;
                }
                break;
			}
			break;
		default:
		WM_DefaultProc(pMsg);
	}

}

