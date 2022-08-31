#include "main_menu.h"
#include "gui_public.h"
#include "set_menu.h"
#include "icon.h"
//�����ֵ
int     depth_val  = 45;
//ģʽѡ��
int     mode_sec   = 0;
//�������״̬
uint8_t motor_sta  = 0;

extern  u8 first_rst_flag; //�Ƿ��ܳɹ���λ�ı�־


/*
*********************************************************************************************************
*                                       ��̬����
*********************************************************************************************************
*/
static const char * _mode[][1] = {
	{"30:2"},
    {"����"},   
    {"15:2"},
};


/*
*********************************************************************************************************
*	�� �� ��: _get_mode
*	����˵��: ��ȡ��ǰģʽ�ַ����ĵ�ַ
*	��    �Σ�Index  Ҫ��ʾģʽ�ַ��������
*	�� �� ֵ: p      ����Ҫ��ʾģʽ�ַ�����ַ
*********************************************************************************************************
*/
static const char * _get_mode(void)
{
    const char *p;
    
    p = NULL;
	/* ��ȡ��ǰ��ѹģʽ */
    p = *_mode[mode_sec];

    return p;
}

/*
*********************************************************************************************************
*	�� �� ��: _cbLanguage
*	����˵��: ��һ�����棬������ȵ�ѡ���Լ�ģʽ�Ŀ���
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbdepth_modle(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
	/* ������ʱ�� */
    int           NCode;
	int           Id;
	int static    min=0,sec=0;
	char *        _acBuffer;
	GUI_LOGPALETTE Palette;
	
	BUTTON_Handle hbutton[7];                

	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hItem);
            //������ҳ����������а�ť
			
			
			//����ֹͣ��ť
			_acBuffer = GUI_ALLOC_h2p(hStopButtonStreamBitmap);
			GUI_CreateBitmapFromStream(&StopBitmap, &Palette, _acBuffer);
			hbutton[6] = _CreateButton(hItem, " ",  ID_STOP_BUTTON, 110, 250 + MAIN_TITLE_HEIGHT, 40,  40, 0);
			BUTTON_SetSkinClassic(hbutton[6]);
			BUTTON_SetBitmapEx(hbutton[6],BUTTON_BI_UNPRESSED,&StopBitmap,0,0);
			if((motor_sta == 0)||(motor_sta == 2))
			{
				//����������Ӱ�ť
				hbutton[0] = _CreateButton_P(hItem, " ",   ID_DEPTH_REDUC_BUTTON, 230, 80 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmzuo1,0);
				//������ȼ��ٰ�ť
				hbutton[1] = _CreateButton_P(hItem, " ",   ID_DEPTH_INCEA_BUTTON, 420, 80 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmyou1,0);
				//����ģʽ��任��ť
				hbutton[2] = _CreateButton_P(hItem, "",   ID_MODE_LCHAGE_BUTTON, 230, 180 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmzuo1,0);
				//����ģʽ�ұ任��ť
				hbutton[3] = _CreateButton_P(hItem, "",   ID_MODE_RCHAGE_BUTTON, 420, 180 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmyou1,0);

				_acBuffer = GUI_ALLOC_h2p(hConButtonStreamBitmap);
				GUI_CreateBitmapFromStream(&ConBitmap, &Palette, _acBuffer);
				hbutton[5] = _CreateButton(hItem, "", ID_PAUSE_RESUME_BUTTON,230, 250 + MAIN_TITLE_HEIGHT, 40,  40, 0);
				BUTTON_SetSkinClassic(hbutton[5]);
				BUTTON_SetBitmapEx(hbutton[5],BUTTON_BI_UNPRESSED,&ConBitmap,0,0);
				
				if(motor_sta == 0)//ֻ�д���ֹͣ״̬�Ż������ð�ť
				{
					//�������ð�ť
					_acBuffer = GUI_ALLOC_h2p(hSetsButtonStreamBitmap);
					GUI_CreateBitmapFromStream(&SetsBitmap, &Palette, _acBuffer);
					hbutton[4] = _CreateButton(hItem, "",   ID_SEETING_BUTTON, 350, 250 + MAIN_TITLE_HEIGHT, 40,  40, 0);	
					BUTTON_SetBkColor(hbutton[4],BUTTON_BI_UNPRESSED,FRAME_BKCOLOR);
					BUTTON_SetSkinClassic(hbutton[4]);
					BUTTON_SetBitmapEx(hbutton[4],BUTTON_BI_UNPRESSED,&SetsBitmap,0,0);
//					BUTTON_SetBitmapEx(hbutton[4],BUTTON_BI_UNPRESSED,&bmset,0,0);
				}				
			}
			else if(motor_sta == 1)
			{
				//������ͣ������ť
				_acBuffer = GUI_ALLOC_h2p(hPauseButtonStreamBitmap);
				GUI_CreateBitmapFromStream(&PauseBitmap, &Palette, _acBuffer);
				hbutton[5] = _CreateButton(hItem, "", ID_PAUSE_RESUME_BUTTON,230, 250 + MAIN_TITLE_HEIGHT, 40,  40, 0);
				BUTTON_SetSkinClassic(hbutton[5]);
				BUTTON_SetBitmapEx(hbutton[5],BUTTON_BI_UNPRESSED,&PauseBitmap,0,0);
			}
			
			WM_CreateTimer(hItem, /* ������Ϣ�Ĵ��ڵľ�� */
						   0, 	                     /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
						   10,                       /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
						   0);
		case WM_PAINT:
			//�ػ洰��
			_PaintFrame();
			_PaintFrameSta();		
			GUI_SetFont(&GUI_FontHZ32);
			GUI_DispStringHCenterAt("���", 70, 70 + MAIN_TITLE_HEIGHT);			
            GUI_DispStringHCenterAt("ģʽ", 70, 170 + MAIN_TITLE_HEIGHT);			
            GUI_DispDecAt(depth_val,320,80 + MAIN_TITLE_HEIGHT,2);
            GUI_DispStringHCenterAt(_get_mode(), 330, 180 + MAIN_TITLE_HEIGHT);
			if((motor_sta == 1)||(motor_sta == 2))
			{
				//��ʾʱ��
				GUI_SetFont(&GUI_FontHZ24);
				GUI_DispStringHCenterAt(":", 362, 250 + MAIN_TITLE_HEIGHT);
				GUI_DispDecAt(min,330,250 + MAIN_TITLE_HEIGHT,2);
				GUI_DispDecAt(sec,370,250 + MAIN_TITLE_HEIGHT,2);
			}
			GUI_SetFont(&GUI_FontHZ24);
			GUI_DispStringHCenterAt("(mm)", 70, 102 + MAIN_TITLE_HEIGHT);
			break;
		/* ��ʱ������ */
		case WM_TIMER:	
			//��ѹʱ��
			if(motor_sta == 1)
			{
				sec ++;
				if(sec >= 60)
				{
					sec = 0;
					min ++;
				}
			}
			_PaintFrame();
			_PaintFrameSta();
			WM_InvalidateWindow(hItem);
			WM_InvalidateWindow(WM_HBKWIN);
		    WM_RestartTimer(pMsg->Data.v, 1000);
			break;
		case WM_INIT_DIALOG:
				break;		
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
				case ID_DEPTH_REDUC_BUTTON: //��ѹ��ȼ��ٰ�ť
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //��ť�����²��ͷ�
							depth_val --;
							if(depth_val < 30)
							{
								depth_val = 53;
							}
                            WM_InvalidateWindow(hItem);
							break;	
					}
					break;
                    
				case ID_DEPTH_INCEA_BUTTON: //��ѹ������Ӱ�ť
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							depth_val ++;
							if(depth_val > 53)
							{
								depth_val = 30;
							}
                            WM_InvalidateWindow(hItem);
							break;
					}
					break;
                case ID_MODE_RCHAGE_BUTTON: //��ѹģʽ�任��ť
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							mode_sec ++;
							if(mode_sec >= 3)
							{
								mode_sec = 0;
							}
							WM_InvalidateWindow(hItem);
							break;
					}
					break;
				 case ID_MODE_LCHAGE_BUTTON: //��ѹģʽ�任��ť
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							mode_sec --;
							if(mode_sec < 0)
							{
								mode_sec = 2;
							}
							WM_InvalidateWindow(hItem);
							break;
					}
					break;
				case ID_PAUSE_RESUME_BUTTON: //��ͣ������ť
				switch(NCode) 
				{
					case WM_NOTIFICATION_CLICKED:
						break;
					case WM_NOTIFICATION_RELEASED:
						if(motor_sta == 0 && first_rst_flag)//�������ֹͣ״̬,���ҿ��Ը�λ�ɹ�
						{
							BUTTON_Delete(hbutton[0]);
							BUTTON_Delete(hbutton[1]);
							BUTTON_Delete(hbutton[2]);
							BUTTON_Delete(hbutton[3]);
							BUTTON_Delete(hbutton[4]);
							motor_sta = 1;//��ʼ����
							_DeleteFrame();   
							_CreateFrame(&_cbdepth_modle);
							change_warn("��ʼCPR",3);
						}
						else if(motor_sta == 1)//�����������״̬
						{
							BUTTON_Delete(hbutton[4]);
							motor_sta = 2;//��ͣ����
							_DeleteFrame();   
							_CreateFrame(&_cbdepth_modle);
						}
						else if(motor_sta == 2)//���������ͣ״̬
						{
							motor_sta = 1;//��������
							_DeleteFrame();   
							_CreateFrame(&_cbdepth_modle);
						}
						
						break;
				}
				break;
				case ID_STOP_BUTTON: //ֹͣ��ť��ť
				switch(NCode) 
				{
					case WM_NOTIFICATION_CLICKED:
						break;
					case WM_NOTIFICATION_RELEASED:
						if(motor_sta)//�����������ģʽ
						{
							motor_sta = 0;//ֹͣ����
							min = 0;
							sec = 0;
							_DeleteFrame();   						
							_CreateFrame(&_cbdepth_modle);
						}						
						break;
				}
				break;
				case ID_SEETING_BUTTON: //���ð�ť��֪ͨ����,�����ý���
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
