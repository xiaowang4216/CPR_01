#include "set_menu.h"

#include "main_menu.h"

#include "time.h"

#include "date_tr.h"

#include "setting.h"

#include "alarm_log.h"

#include "about.h"

#include "servic.h"

#include "icon.h"
/*
*********************************************************************************************************
*	�� �� ��: _cbsystem_menu
*	����˵��: ���ô��ڻص�����
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbsystem_menu_1(WM_MESSAGE* pMsg)
{
    WM_HWIN hItem = pMsg->hWin;
    int     NCode;
	int     Id;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hItem);
            //������ҳ����������а�ť
			GUI_SetFont(&GUI_FontHZ24);
			GUI_DrawLine(0,125,480,125);
			_CreateButton_P(hItem, " ",  ID_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton_P(hItem, "������־",   ID_DEPTH_REDUC_BUTTON, 240, 120 + MAIN_TITLE_HEIGHT, 190,  160, 0,&bmlog,1);//��־��ť
			_CreateButton_P(hItem, "ϵͳ����",   ID_DEPTH_INCEA_BUTTON, 50, 120 + MAIN_TITLE_HEIGHT, 190,  160, 0,&bmsetting,1); //ϵͳ���ð�ť			
			_CreateButton_P(hItem, " ",   ID_MODE_LCHAGE_BUTTON, 0, 120 + MAIN_TITLE_HEIGHT, 50,  160, 0,&bmzuo2,2);
			_CreateButton_P(hItem, " ",   ID_MODE_RCHAGE_BUTTON, 430, 120 + MAIN_TITLE_HEIGHT, 50,  160, 0,&bmyou2,2);
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
            //GUI_SetFont(GUI_FONT_24B_ASCII);
			//GUI_DispStringHCenterAt("��ѡ������", FRAME_WIDTH >> 1, 5);
			//GUI_DispStringHCenterAt("Depth", 70, 70);
            //GUI_DispStringHCenterAt("Mode", 70, 170);
                                   
            // GUI_DispDecAt(LCD_GetXSize(),120,10,4);
            // GUI_DispDecAt(LCD_GetYSize(),120,60,4);
            // GUI_DispStringHCenterAt(_get_mode(), 320, 170);
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
				case ID_DEPTH_REDUC_BUTTON: /* ����־���水ť */
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //��ϵͳ������־����
							_DeleteFrame();                    
                            _CreateFrame(&_cblogs);
							break;	
					}
					break;
                    
				case ID_DEPTH_INCEA_BUTTON: //��ϵͳ���ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							_DeleteFrame();                    
                            _CreateFrame(&_cbsetting);
							break;
					}
					break;
                case ID_MODE_LCHAGE_BUTTON: //�����л�����
                    switch(NCode) 
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            _DeleteFrame();                    
                            _CreateFrame(&_cbsystem_menu_3);
                            break;
                    }
                    break;
                case ID_MODE_RCHAGE_BUTTON: //�����л�����
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
				case ID_BACK_BUTTON: //����������
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
                            _DeleteFrame();                
                            _CreateFrame(&_cbdepth_modle);
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
*	�� �� ��: _cbsystem_menu_2
*	����˵��: ���ô��ڻص�����
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbsystem_menu_2(WM_MESSAGE* pMsg)
{
    WM_HWIN       hItem = pMsg->hWin;
    int           NCode;
	int           Id;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hItem);
            //������ҳ����������а�ť
			GUI_SetFont(&GUI_FontHZ24);
            _CreateButton_P(hItem, " ",  ID_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton_P(hItem, "����",   ID_DEPTH_REDUC_BUTTON, 240, 120 +MAIN_TITLE_HEIGHT, 190,  160, 0,&bmdate,1);//ʱ�����ð�ť
			_CreateButton_P(hItem, "���ݴ���",   ID_DEPTH_INCEA_BUTTON, 50, 120 +MAIN_TITLE_HEIGHT, 190,  160, 0,&bmdata,1);//���ݴ��䰴ť
			_CreateButton_P(hItem, " ",   ID_MODE_LCHAGE_BUTTON, 0, 120 + MAIN_TITLE_HEIGHT, 50,  160, 0,&bmzuo2,2);
			_CreateButton_P(hItem, " ",   ID_MODE_RCHAGE_BUTTON, 430, 120 + MAIN_TITLE_HEIGHT, 50,  160, 0,&bmyou2,2);
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
            //GUI_SetFont(GUI_FONT_24B_ASCII);
			//GUI_DispStringHCenterAt("��ѡ������", FRAME_WIDTH >> 1, 5);
			//GUI_DispStringHCenterAt("Depth", 70, 70);
            //GUI_DispStringHCenterAt("Mode", 70, 170);
                                   
            // GUI_DispDecAt(LCD_GetXSize(),120,10,4);
            // GUI_DispDecAt(LCD_GetYSize(),120,60,4);
            // GUI_DispStringHCenterAt(_get_mode(), 320, 170);
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
				case ID_DEPTH_REDUC_BUTTON: //BUTTON_0��֪ͨ���룬����LED1
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //��ť�����²��ͷ�
							 _DeleteFrame();                    
                            _CreateFrame(&_cbtime_set);
							break;	
					}
					break;
                    
				case ID_DEPTH_INCEA_BUTTON: //�����ݴ������
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
                case ID_MODE_LCHAGE_BUTTON: //BUTTON_1��֪ͨ���룬����BEEP
                    switch(NCode) 
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            _DeleteFrame();                   
                            _CreateFrame(&_cbsystem_menu_1);
                            break;
                    }
                    break;
                case ID_MODE_RCHAGE_BUTTON: //BUTTON_1��֪ͨ���룬����BEEP
                    switch(NCode) 
                    {
                        case WM_NOTIFICATION_CLICKED:
                            break;
                        case WM_NOTIFICATION_RELEASED:
                            _DeleteFrame();                    
                            _CreateFrame(&_cbsystem_menu_3);
                            break;
                    }
                    break;
				case ID_BACK_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
                            _DeleteFrame();                     
                            _CreateFrame(&_cbdepth_modle);
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
*	�� �� ��: _cbsystem_menu
*	����˵��: ���ô��ڻص�����
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbsystem_menu_3(WM_MESSAGE* pMsg)
{
    WM_HWIN       hItem = pMsg->hWin;
    int           NCode;
	int           Id;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hItem);
            //������ҳ����������а�ť
			GUI_SetFont(&GUI_FontHZ24);
			_CreateButton_P(hItem, " ",  ID_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton_P(hItem, "���ڱ���",   ID_DEPTH_REDUC_BUTTON, 240, 120 + MAIN_TITLE_HEIGHT, 190,  160, 0,&bmabout,1);//���ڰ�ť
			_CreateButton_P(hItem, "ά��",   ID_DEPTH_INCEA_BUTTON, 50, 120 + MAIN_TITLE_HEIGHT, 190,  160, 0,&bmservic,1);//ά����ť
			_CreateButton_P(hItem, " ",   ID_MODE_LCHAGE_BUTTON, 0, 120 + MAIN_TITLE_HEIGHT, 50,  160, 0,&bmzuo2,2);
			_CreateButton_P(hItem, " ",   ID_MODE_RCHAGE_BUTTON, 430, 120 + MAIN_TITLE_HEIGHT, 50,  160, 0,&bmyou2,2);
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
            //GUI_SetFont(GUI_FONT_24B_ASCII);
			//GUI_DispStringHCenterAt("��ѡ������", FRAME_WIDTH >> 1, 5);
			//GUI_DispStringHCenterAt("Depth", 70, 70);
            //GUI_DispStringHCenterAt("Mode", 70, 170);
                                   
            // GUI_DispDecAt(LCD_GetXSize(),120,10,4);
            // GUI_DispDecAt(LCD_GetYSize(),120,60,4);
            // GUI_DispStringHCenterAt(_get_mode(), 320, 170);
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
				case ID_DEPTH_REDUC_BUTTON: //BUTTON_0��֪ͨ���룬����LED1
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //��ť�����²��ͷ�
							_DeleteFrame();                   
                            _CreateFrame(&_cbabout);
							break;	
					}
					break;
                    
				case ID_DEPTH_INCEA_BUTTON: //BUTTON_1��֪ͨ���룬����BEEP
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							_DeleteFrame();                     
                            _CreateFrame(&_cbNumPad);
							break;
					}
					break;
                case ID_MODE_LCHAGE_BUTTON: //BUTTON_1��֪ͨ���룬����BEEP
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
                case ID_MODE_RCHAGE_BUTTON: //BUTTON_1��֪ͨ���룬����BEEP
                switch(NCode) 
                {
                    case WM_NOTIFICATION_CLICKED:
                        break;
                    case WM_NOTIFICATION_RELEASED:
                        _DeleteFrame();                     
                        _CreateFrame(&_cbsystem_menu_1);
                        break;
                }
                break;
				case ID_BACK_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
                            _DeleteFrame();                     
                            _CreateFrame(&_cbdepth_modle);
							break;
					}
					break;
			}
			break;
		default:
		WM_DefaultProc(pMsg);
	}

}

