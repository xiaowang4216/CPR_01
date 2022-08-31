#include "main_menu.h"
#include "gui_public.h"
#include "set_menu.h"
#include "icon.h"
//深度数值
int     depth_val  = 45;
//模式选择
int     mode_sec   = 0;
//电机控制状态
uint8_t motor_sta  = 0;

extern  u8 first_rst_flag; //是否能成功复位的标志


/*
*********************************************************************************************************
*                                       静态数据
*********************************************************************************************************
*/
static const char * _mode[][1] = {
	{"30:2"},
    {"连续"},   
    {"15:2"},
};


/*
*********************************************************************************************************
*	函 数 名: _get_mode
*	功能说明: 获取当前模式字符串的地址
*	形    参：Index  要显示模式字符串的序号
*	返 回 值: p      返回要显示模式字符串地址
*********************************************************************************************************
*/
static const char * _get_mode(void)
{
    const char *p;
    
    p = NULL;
	/* 获取当前按压模式 */
    p = *_mode[mode_sec];

    return p;
}

/*
*********************************************************************************************************
*	函 数 名: _cbLanguage
*	功能说明: 第一个界面，用于深度的选择以及模式的控制
*	形    参：pMsg  参数指针
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbdepth_modle(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
	/* 创建定时器 */
    int           NCode;
	int           Id;
	int static    min=0,sec=0;
	char *        _acBuffer;
	GUI_LOGPALETTE Palette;
	
	BUTTON_Handle hbutton[7];                

	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* 设置聚焦 */
			WM_SetFocus(hItem);
            //创建此页面所需的所有按钮
			
			
			//绘制停止按钮
			_acBuffer = GUI_ALLOC_h2p(hStopButtonStreamBitmap);
			GUI_CreateBitmapFromStream(&StopBitmap, &Palette, _acBuffer);
			hbutton[6] = _CreateButton(hItem, " ",  ID_STOP_BUTTON, 110, 250 + MAIN_TITLE_HEIGHT, 40,  40, 0);
			BUTTON_SetSkinClassic(hbutton[6]);
			BUTTON_SetBitmapEx(hbutton[6],BUTTON_BI_UNPRESSED,&StopBitmap,0,0);
			if((motor_sta == 0)||(motor_sta == 2))
			{
				//绘制深度增加按钮
				hbutton[0] = _CreateButton_P(hItem, " ",   ID_DEPTH_REDUC_BUTTON, 230, 80 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmzuo1,0);
				//绘制深度减少按钮
				hbutton[1] = _CreateButton_P(hItem, " ",   ID_DEPTH_INCEA_BUTTON, 420, 80 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmyou1,0);
				//绘制模式左变换按钮
				hbutton[2] = _CreateButton_P(hItem, "",   ID_MODE_LCHAGE_BUTTON, 230, 180 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmzuo1,0);
				//绘制模式右变换按钮
				hbutton[3] = _CreateButton_P(hItem, "",   ID_MODE_RCHAGE_BUTTON, 420, 180 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmyou1,0);

				_acBuffer = GUI_ALLOC_h2p(hConButtonStreamBitmap);
				GUI_CreateBitmapFromStream(&ConBitmap, &Palette, _acBuffer);
				hbutton[5] = _CreateButton(hItem, "", ID_PAUSE_RESUME_BUTTON,230, 250 + MAIN_TITLE_HEIGHT, 40,  40, 0);
				BUTTON_SetSkinClassic(hbutton[5]);
				BUTTON_SetBitmapEx(hbutton[5],BUTTON_BI_UNPRESSED,&ConBitmap,0,0);
				
				if(motor_sta == 0)//只有处于停止状态才绘制设置按钮
				{
					//绘制设置按钮
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
				//绘制暂停继续按钮
				_acBuffer = GUI_ALLOC_h2p(hPauseButtonStreamBitmap);
				GUI_CreateBitmapFromStream(&PauseBitmap, &Palette, _acBuffer);
				hbutton[5] = _CreateButton(hItem, "", ID_PAUSE_RESUME_BUTTON,230, 250 + MAIN_TITLE_HEIGHT, 40,  40, 0);
				BUTTON_SetSkinClassic(hbutton[5]);
				BUTTON_SetBitmapEx(hbutton[5],BUTTON_BI_UNPRESSED,&PauseBitmap,0,0);
			}
			
			WM_CreateTimer(hItem, /* 接受信息的窗口的句柄 */
						   0, 	                     /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
						   10,                       /* 周期，此周期过后指定窗口应收到消息*/
						   0);
		case WM_PAINT:
			//重绘窗口
			_PaintFrame();
			_PaintFrameSta();		
			GUI_SetFont(&GUI_FontHZ32);
			GUI_DispStringHCenterAt("深度", 70, 70 + MAIN_TITLE_HEIGHT);			
            GUI_DispStringHCenterAt("模式", 70, 170 + MAIN_TITLE_HEIGHT);			
            GUI_DispDecAt(depth_val,320,80 + MAIN_TITLE_HEIGHT,2);
            GUI_DispStringHCenterAt(_get_mode(), 330, 180 + MAIN_TITLE_HEIGHT);
			if((motor_sta == 1)||(motor_sta == 2))
			{
				//显示时间
				GUI_SetFont(&GUI_FontHZ24);
				GUI_DispStringHCenterAt(":", 362, 250 + MAIN_TITLE_HEIGHT);
				GUI_DispDecAt(min,330,250 + MAIN_TITLE_HEIGHT,2);
				GUI_DispDecAt(sec,370,250 + MAIN_TITLE_HEIGHT,2);
			}
			GUI_SetFont(&GUI_FontHZ24);
			GUI_DispStringHCenterAt("(mm)", 70, 102 + MAIN_TITLE_HEIGHT);
			break;
		/* 定时器更新 */
		case WM_TIMER:	
			//按压时间
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
				case ID_DEPTH_REDUC_BUTTON: //按压深度减少按钮
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //按钮被按下并释放
							depth_val --;
							if(depth_val < 30)
							{
								depth_val = 53;
							}
                            WM_InvalidateWindow(hItem);
							break;	
					}
					break;
                    
				case ID_DEPTH_INCEA_BUTTON: //按压深度增加按钮
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
                case ID_MODE_RCHAGE_BUTTON: //按压模式变换按钮
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
				 case ID_MODE_LCHAGE_BUTTON: //按压模式变换按钮
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
				case ID_PAUSE_RESUME_BUTTON: //暂停继续按钮
				switch(NCode) 
				{
					case WM_NOTIFICATION_CLICKED:
						break;
					case WM_NOTIFICATION_RELEASED:
						if(motor_sta == 0 && first_rst_flag)//如果是在停止状态,并且可以复位成功
						{
							BUTTON_Delete(hbutton[0]);
							BUTTON_Delete(hbutton[1]);
							BUTTON_Delete(hbutton[2]);
							BUTTON_Delete(hbutton[3]);
							BUTTON_Delete(hbutton[4]);
							motor_sta = 1;//开始运行
							_DeleteFrame();   
							_CreateFrame(&_cbdepth_modle);
							change_warn("开始CPR",3);
						}
						else if(motor_sta == 1)//如果是在运行状态
						{
							BUTTON_Delete(hbutton[4]);
							motor_sta = 2;//暂停运行
							_DeleteFrame();   
							_CreateFrame(&_cbdepth_modle);
						}
						else if(motor_sta == 2)//如果是在暂停状态
						{
							motor_sta = 1;//继续运行
							_DeleteFrame();   
							_CreateFrame(&_cbdepth_modle);
						}
						
						break;
				}
				break;
				case ID_STOP_BUTTON: //停止按钮按钮
				switch(NCode) 
				{
					case WM_NOTIFICATION_CLICKED:
						break;
					case WM_NOTIFICATION_RELEASED:
						if(motor_sta)//如果是在运行模式
						{
							motor_sta = 0;//停止运行
							min = 0;
							sec = 0;
							_DeleteFrame();   						
							_CreateFrame(&_cbdepth_modle);
						}						
						break;
				}
				break;
				case ID_SEETING_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
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
