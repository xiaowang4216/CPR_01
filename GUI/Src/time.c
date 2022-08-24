#include "time.h"
#include "rtc.h"
#include "set_menu.h"
#include "EmWinHZFont.h"

#define ID_YEAR_ADD_BUTTON    (GUI_ID_USER + 0x00)
#define ID_YEAR_DEC_BUTTON    (GUI_ID_USER + 0x01)
#define ID_MONTH_ADD_BUTTON   (GUI_ID_USER + 0x02)
#define ID_MONTH_DEC_BUTTON   (GUI_ID_USER + 0x03)
#define ID_DAY_ADD_BUTTON     (GUI_ID_USER + 0x04)
#define ID_DAY_DEC_BUTTON     (GUI_ID_USER + 0x05)
#define ID_HOUR_ADD_BUTTON    (GUI_ID_USER + 0x06)
#define ID_HOUR_DEC_BUTTON    (GUI_ID_USER + 0x07)
#define ID_MINUTE_ADD_BUTTON  (GUI_ID_USER + 0x08)
#define ID_MINUTE_DEC_BUTTON  (GUI_ID_USER + 0x09)
#define ID_TIME_BACK_BUTTON   (GUI_ID_USER + 0x10)
#define ID_TIME_SAVE_BUTTON   (GUI_ID_USER + 0x11)


RTC_TimeTypeDef  RTC_TimeStructure;
RTC_DateTypeDef  RTC_DateStructure;

RTC_TimeTypeDef  RTC_TimeStructure_Temp;
RTC_DateTypeDef  RTC_DateStructure_Temp;

static int month_day [12] = {31,28,31,30,31,30,31,31,30,31,30,31};

/*
*********************************************************************************************************
*	�� �� ��: judge_leap
*	����˵��: �ж��Ƿ�������
*	��    �Σ�RTC_DateStructure  ʱ��ṹ��ָ��
*	�� �� ֵ: �����򷵻�1,�����򷵻�0
*********************************************************************************************************
*/
int judge_leap(RTC_DateTypeDef  RTC_DateStructure_t)
{
	/* ��ȡ��ǰ��� */
	int year = RTC_DateStructure_t.RTC_Year;
	/* �жϵ�ǰ�Ƿ������� */
	if(year % 4 == 0||year % 100 == 0||year % 400 == 0)
	{
		return 1;
	}
	return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: _cbLanguage
*	����˵��: ��һ�����棬������ȵ�ѡ���Լ�ģʽ�Ŀ���
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _cbtime_set(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
    int     NCode;
	int     Id;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hItem);
			_PaintFrameSta();
            //������ҳ����������а�ť
			GUI_SetFont(&GUI_FontHZ24);
			_CreateButton(hItem, "jia",   ID_YEAR_ADD_BUTTON, 400, 140 - MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "jia",   ID_YEAR_DEC_BUTTON, 400, 280 - MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "jia",   ID_MONTH_ADD_BUTTON, 308, 140 - MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "jia",   ID_MONTH_DEC_BUTTON, 308, 280 - MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "jia",   ID_DAY_ADD_BUTTON,208, 140 - MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "jie",   ID_DAY_DEC_BUTTON, 208, 280 - MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "jie",   ID_HOUR_ADD_BUTTON, 20, 140 - MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "jie",   ID_HOUR_DEC_BUTTON, 20, 280 - MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "jie",   ID_MINUTE_ADD_BUTTON, 112, 140 - MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "jie",   ID_MINUTE_DEC_BUTTON,112, 280 - MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton_P(hItem, " ",  ID_TIME_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton(hItem, "����",  ID_TIME_SAVE_BUTTON, 350, 70 - MAIN_TITLE_HEIGHT, 40,  40, 0);
			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure_Temp);
			RTC_GetDate(RTC_Format_BIN,&RTC_DateStructure_Temp);

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
			GUI_DispStringHCenterAt("ʱ��", 230, 70 - MAIN_TITLE_HEIGHT);	
			GUI_DispStringHCenterAt(":", 80, 200 - MAIN_TITLE_HEIGHT);		
            GUI_DispDecAt(RTC_TimeStructure_Temp.RTC_Hours,20,200 - MAIN_TITLE_HEIGHT,2);
			GUI_DispDecAt(RTC_TimeStructure_Temp.RTC_Minutes,112,200 - MAIN_TITLE_HEIGHT,2);
			GUI_DispDecAt(RTC_DateStructure_Temp.RTC_Date,208,200 - MAIN_TITLE_HEIGHT,2);
			GUI_DispDecAt(RTC_DateStructure_Temp.RTC_Month,304,200 - MAIN_TITLE_HEIGHT,2);
			GUI_DispDecAt(RTC_DateStructure_Temp.RTC_Year,400,200 - MAIN_TITLE_HEIGHT,4);
			break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
				case ID_YEAR_ADD_BUTTON: //BUTTON_0��֪ͨ���룬����LED1
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //��ť�����²��ͷ�
							RTC_DateStructure_Temp.RTC_Year += 1;
                            WM_InvalidateWindow(hItem);
							break;	
					}
					break;
                    
				case ID_YEAR_DEC_BUTTON: //BUTTON_1��֪ͨ���룬����BEEP
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							RTC_DateStructure_Temp.RTC_Year -= 1;
                            WM_InvalidateWindow(hItem);
							break;
					}
					break;
					case ID_MONTH_ADD_BUTTON: //BUTTON_0��֪ͨ���룬����LED1
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //��ť�����²��ͷ�
							RTC_DateStructure_Temp.RTC_Month += 1;
							if(RTC_DateStructure_Temp.RTC_Month >12)
							{
								RTC_DateStructure_Temp.RTC_Month = 1;
							}
                            WM_InvalidateWindow(hItem);
							break;	
					}
					break;
                    
				case ID_MONTH_DEC_BUTTON: //BUTTON_1��֪ͨ���룬����BEEP
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							RTC_DateStructure_Temp.RTC_Month -= 1;
							if(RTC_DateStructure_Temp.RTC_Month <= 0)
							{
								RTC_DateStructure_Temp.RTC_Month = 12;
							}
                            WM_InvalidateWindow(hItem);
							break;
					}
					break;
					case ID_DAY_ADD_BUTTON: //BUTTON_0��֪ͨ���룬����LED1
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //��ť�����²��ͷ�
							RTC_DateStructure_Temp.RTC_Date += 1;
							if(judge_leap(RTC_DateStructure_Temp))
							{
								if(RTC_DateStructure_Temp.RTC_Month == 2)
								{
									if(RTC_DateStructure_Temp.RTC_Date > 29)
									{
										RTC_DateStructure_Temp.RTC_Date = 1;
									}
								}								
							}
							else
							{
								if(RTC_DateStructure_Temp.RTC_Date > month_day[RTC_DateStructure_Temp.RTC_Month - 1])
								{
									RTC_DateStructure_Temp.RTC_Date = 1;
								}
							}
                            WM_InvalidateWindow(hItem);
							break;	
					}
					break;
                    
				case ID_DAY_DEC_BUTTON: //BUTTON_1��֪ͨ���룬����BEEP
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							RTC_DateStructure_Temp.RTC_Date -= 1;
							if(judge_leap(RTC_DateStructure_Temp))
							{
								if(RTC_DateStructure_Temp.RTC_Month == 2)
								{
									if(RTC_DateStructure_Temp.RTC_Date <= 0)
									{
										RTC_DateStructure_Temp.RTC_Date = 29;
									}
								}								
							}
							else
							{
								if(RTC_DateStructure_Temp.RTC_Date <= 0)
								{
									RTC_DateStructure_Temp.RTC_Date = month_day[RTC_DateStructure_Temp.RTC_Month - 1];
								}
							}
                            WM_InvalidateWindow(hItem);
							break;
					}
					break;
					case ID_HOUR_ADD_BUTTON: //BUTTON_0��֪ͨ���룬����LED1
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //��ť�����²��ͷ�
							RTC_TimeStructure_Temp.RTC_Hours += 1;		
							if(RTC_TimeStructure_Temp.RTC_Hours >= 24)
							{
								RTC_TimeStructure_Temp.RTC_Hours = 0;
							}				
                            WM_InvalidateWindow(hItem);
							break;	
					}
					break;
                    
				case ID_HOUR_DEC_BUTTON: //BUTTON_1��֪ͨ���룬����BEEP
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							RTC_TimeStructure_Temp.RTC_Hours -= 1;
							if(RTC_TimeStructure_Temp.RTC_Hours > 24)
							{
								RTC_TimeStructure_Temp.RTC_Hours = 23;
							}	
                            WM_InvalidateWindow(hItem);
							break;
					}
					break;
					case ID_MINUTE_ADD_BUTTON: //BUTTON_0��֪ͨ���룬����LED1
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //��ť�����²��ͷ�
							RTC_TimeStructure_Temp.RTC_Minutes += 1;
							if(RTC_TimeStructure_Temp.RTC_Minutes >= 60)
							{
								RTC_TimeStructure_Temp.RTC_Minutes = 0;
							}	
                            WM_InvalidateWindow(hItem);
							break;	
					}
					break;
                    
				case ID_MINUTE_DEC_BUTTON: //BUTTON_1��֪ͨ���룬����BEEP
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							RTC_TimeStructure_Temp.RTC_Minutes -= 1;
							if(RTC_TimeStructure_Temp.RTC_Minutes > 60)
							{
								RTC_TimeStructure_Temp.RTC_Minutes = 59;
							}
                            WM_InvalidateWindow(hItem);
							break;
					}
					break;
                case ID_TIME_SAVE_BUTTON: //BUTTON_1��֪ͨ���룬����BEEP
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							RTC_Set_Date(RTC_DateStructure_Temp.RTC_Year,RTC_DateStructure_Temp.RTC_Month,RTC_DateStructure_Temp.RTC_Date,RTC_DateStructure_Temp.RTC_WeekDay);
							RTC_Set_Time(RTC_TimeStructure_Temp.RTC_Hours,RTC_TimeStructure_Temp.RTC_Minutes,RTC_TimeStructure_Temp.RTC_Seconds,RTC_H12_AM);
							WM_InvalidateWindow(hItem);
							break;
					}
					break;
				case ID_TIME_BACK_BUTTON: //�ر�ʱ�����ý���,���ض����˵�
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
                            _DeleteFrame();                      
                            _CreateFrame(&_cbsystem_menu_2);
							break;
					}
					break;
			}
			break;
		default:
		WM_DefaultProc(pMsg);
	}
}


