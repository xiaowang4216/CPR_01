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
*	函 数 名: _cbsystem_menu
*	功能说明: 设置窗口回调函数
*	形    参：pMsg  参数指针
*	返 回 值: 无
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
			/* 设置聚焦 */
			WM_SetFocus(hItem);
            //创建此页面所需的所有按钮
			GUI_SetFont(&GUI_FontHZ24);
			GUI_DrawLine(0,125,480,125);
			_CreateButton_P(hItem, " ",  ID_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton_P(hItem, "报警日志",   ID_DEPTH_REDUC_BUTTON, 240, 120 + MAIN_TITLE_HEIGHT, 190,  160, 0,&bmlog,1);//日志按钮
			_CreateButton_P(hItem, "系统设置",   ID_DEPTH_INCEA_BUTTON, 50, 120 + MAIN_TITLE_HEIGHT, 190,  160, 0,&bmsetting,1); //系统设置按钮			
			_CreateButton_P(hItem, " ",   ID_MODE_LCHAGE_BUTTON, 0, 120 + MAIN_TITLE_HEIGHT, 50,  160, 0,&bmzuo2,2);
			_CreateButton_P(hItem, " ",   ID_MODE_RCHAGE_BUTTON, 430, 120 + MAIN_TITLE_HEIGHT, 50,  160, 0,&bmyou2,2);
			WM_CreateTimer(hItem, /* 接受信息的窗口的句柄 */
						   0, 	                     /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
						   10,                       /* 周期，此周期过后指定窗口应收到消息*/
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
			//GUI_DispStringHCenterAt("请选择语言", FRAME_WIDTH >> 1, 5);
			//GUI_DispStringHCenterAt("Depth", 70, 70);
            //GUI_DispStringHCenterAt("Mode", 70, 170);
                                   
            // GUI_DispDecAt(LCD_GetXSize(),120,10,4);
            // GUI_DispDecAt(LCD_GetYSize(),120,60,4);
            // GUI_DispStringHCenterAt(_get_mode(), 320, 170);
			break;
		/* 定时器更新 */
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
				case ID_DEPTH_REDUC_BUTTON: /* 打开日志界面按钮 */
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //打开系统报警日志界面
							_DeleteFrame();                    
                            _CreateFrame(&_cblogs);
							break;	
					}
					break;
                    
				case ID_DEPTH_INCEA_BUTTON: //打开系统设置界面
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
                case ID_MODE_LCHAGE_BUTTON: //往左切换界面
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
                case ID_MODE_RCHAGE_BUTTON: //往右切换界面
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
				case ID_BACK_BUTTON: //返回主界面
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
*	函 数 名: _cbsystem_menu_2
*	功能说明: 设置窗口回调函数
*	形    参：pMsg  参数指针
*	返 回 值: 无
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
			/* 设置聚焦 */
			WM_SetFocus(hItem);
            //创建此页面所需的所有按钮
			GUI_SetFont(&GUI_FontHZ24);
            _CreateButton_P(hItem, " ",  ID_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton_P(hItem, "日期",   ID_DEPTH_REDUC_BUTTON, 240, 120 +MAIN_TITLE_HEIGHT, 190,  160, 0,&bmdate,1);//时间设置按钮
			_CreateButton_P(hItem, "数据传输",   ID_DEPTH_INCEA_BUTTON, 50, 120 +MAIN_TITLE_HEIGHT, 190,  160, 0,&bmdata,1);//数据传输按钮
			_CreateButton_P(hItem, " ",   ID_MODE_LCHAGE_BUTTON, 0, 120 + MAIN_TITLE_HEIGHT, 50,  160, 0,&bmzuo2,2);
			_CreateButton_P(hItem, " ",   ID_MODE_RCHAGE_BUTTON, 430, 120 + MAIN_TITLE_HEIGHT, 50,  160, 0,&bmyou2,2);
			WM_CreateTimer(hItem, /* 接受信息的窗口的句柄 */
						   0, 	                     /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
						   10,                       /* 周期，此周期过后指定窗口应收到消息*/
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
			//GUI_DispStringHCenterAt("请选择语言", FRAME_WIDTH >> 1, 5);
			//GUI_DispStringHCenterAt("Depth", 70, 70);
            //GUI_DispStringHCenterAt("Mode", 70, 170);
                                   
            // GUI_DispDecAt(LCD_GetXSize(),120,10,4);
            // GUI_DispDecAt(LCD_GetYSize(),120,60,4);
            // GUI_DispStringHCenterAt(_get_mode(), 320, 170);
			break;
		/* 定时器更新 */
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
				case ID_DEPTH_REDUC_BUTTON: //BUTTON_0的通知代码，控制LED1
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //按钮被按下并释放
							 _DeleteFrame();                    
                            _CreateFrame(&_cbtime_set);
							break;	
					}
					break;
                    
				case ID_DEPTH_INCEA_BUTTON: //打开数据传输界面
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
                case ID_MODE_LCHAGE_BUTTON: //BUTTON_1的通知代码，控制BEEP
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
                case ID_MODE_RCHAGE_BUTTON: //BUTTON_1的通知代码，控制BEEP
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
				case ID_BACK_BUTTON: //设置按钮的通知代码,打开设置界面
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
*	函 数 名: _cbsystem_menu
*	功能说明: 设置窗口回调函数
*	形    参：pMsg  参数指针
*	返 回 值: 无
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
			/* 设置聚焦 */
			WM_SetFocus(hItem);
            //创建此页面所需的所有按钮
			GUI_SetFont(&GUI_FontHZ24);
			_CreateButton_P(hItem, " ",  ID_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton_P(hItem, "关于本机",   ID_DEPTH_REDUC_BUTTON, 240, 120 + MAIN_TITLE_HEIGHT, 190,  160, 0,&bmabout,1);//关于按钮
			_CreateButton_P(hItem, "维护",   ID_DEPTH_INCEA_BUTTON, 50, 120 + MAIN_TITLE_HEIGHT, 190,  160, 0,&bmservic,1);//维护按钮
			_CreateButton_P(hItem, " ",   ID_MODE_LCHAGE_BUTTON, 0, 120 + MAIN_TITLE_HEIGHT, 50,  160, 0,&bmzuo2,2);
			_CreateButton_P(hItem, " ",   ID_MODE_RCHAGE_BUTTON, 430, 120 + MAIN_TITLE_HEIGHT, 50,  160, 0,&bmyou2,2);
			WM_CreateTimer(hItem, /* 接受信息的窗口的句柄 */
						   0, 	                     /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
						   10,                       /* 周期，此周期过后指定窗口应收到消息*/
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
			//GUI_DispStringHCenterAt("请选择语言", FRAME_WIDTH >> 1, 5);
			//GUI_DispStringHCenterAt("Depth", 70, 70);
            //GUI_DispStringHCenterAt("Mode", 70, 170);
                                   
            // GUI_DispDecAt(LCD_GetXSize(),120,10,4);
            // GUI_DispDecAt(LCD_GetYSize(),120,60,4);
            // GUI_DispStringHCenterAt(_get_mode(), 320, 170);
			break;
		/* 定时器更新 */
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
				case ID_DEPTH_REDUC_BUTTON: //BUTTON_0的通知代码，控制LED1
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //按钮被按下并释放
							_DeleteFrame();                   
                            _CreateFrame(&_cbabout);
							break;	
					}
					break;
                    
				case ID_DEPTH_INCEA_BUTTON: //BUTTON_1的通知代码，控制BEEP
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
                case ID_MODE_LCHAGE_BUTTON: //BUTTON_1的通知代码，控制BEEP
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
                case ID_MODE_RCHAGE_BUTTON: //BUTTON_1的通知代码，控制BEEP
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
				case ID_BACK_BUTTON: //设置按钮的通知代码,打开设置界面
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

