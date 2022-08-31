/**************************************************
	* @File Name: setting.c
	* @brief GUI语言及音量设置界面
	* @author 王现刚 (2891854535@qq.com)
	* @Version : 1.0
	* @date 2022-08-31
	* 
***************************************************/
#include "gui_public.h"
#include "set_menu.h"
#include "setting.h"

/***
 * 设置界面
 * 通过此界面设置语言类型
 * 音量大小
 * 报警方式
 ***/

//语言左变换按钮
#define ID_LANGUAGE_LCHANGE_BUTTON (GUI_ID_USER + 0x00)
//语言右变换按钮
#define ID_LANGUAGE_RCHANGE_BUTTON (GUI_ID_USER + 0x01)
//音量加按钮
#define ID_VOLUME_INCEA_BUTTON     (GUI_ID_USER + 0x02)
//音量减按钮
#define ID_VOLUME_REDUC_BUTTON     (GUI_ID_USER + 0x03)
//设置界面返回按钮
#define ID_SETTING_BACK_BUTTON     (GUI_ID_USER + 0x04)


/**************************************************
	* 
	* @brief 变量定义
	* 
***************************************************/
int LANGUAGE_SELECT = 0;  /* 语言序列号,默认中文 */
int ALARM_VOLUME    = 0; /* 系统提示音量大小 */


/**************************************************
	* 
	* @brief 静态数组,语言种类
	* 
***************************************************/
static const char * _language[][1] = {
    {"中文"},
    {"English"},
    {"Русский"},
};


/*
*********************************************************************************************************
*	函 数 名: _get_language
*	功能说明: 获取当前语言字符串的地址
*	形    参：Index  要显示语言字符串的序号
*	返 回 值: p      返回要显示语言字符串地址
*********************************************************************************************************
*/
static const char * _get_language(void)
{
    const char *p;

    p = NULL;
	/* 获取当前语言 */
    p = * _language[LANGUAGE_SELECT];

    return p;
}

/*
*********************************************************************************************************
*	函 数 名: _cbsetting
*	功能说明: 设置界面,用于语言设置,音量设置,报警方式设置
*	形    参：pMsg  参数指针
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbsetting(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
	/* 创建定时器 */
    int     NCode;
	int     Id;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* 设置聚焦 */
			WM_SetFocus(hItem);
            //创建此页面所需的所有按钮
            GUI_SetFont(&GUI_FontHZ24);
			_CreateButton(hItem, "jie",   ID_VOLUME_REDUC_BUTTON,      230, 170 + MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "jia",   ID_VOLUME_INCEA_BUTTON,      420, 170 + MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "zuo",   ID_LANGUAGE_LCHANGE_BUTTON,  230, 120 + MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton(hItem, "you",   ID_LANGUAGE_RCHANGE_BUTTON,  420, 120 + MAIN_TITLE_HEIGHT, 20,  20, 0);
			_CreateButton_P(hItem, " ",  ID_SETTING_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
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
			GUI_SetFont(&GUI_FontHZ24);
			GUI_DispStringHCenterAt("语言", 70, 120 + MAIN_TITLE_HEIGHT);
            GUI_DispStringHCenterAt("音量", 70, 170 + MAIN_TITLE_HEIGHT);
            GUI_DispStringHCenterAt("语音类型", 70, 220 + MAIN_TITLE_HEIGHT);
            GUI_DispStringHCenterAt(_get_language(), 330, 120 + MAIN_TITLE_HEIGHT);
            GUI_DispDecAt(ALARM_VOLUME,320,170 + MAIN_TITLE_HEIGHT,2);
            GUI_DispStringHCenterAt("嘀嘀", 330, 220 + MAIN_TITLE_HEIGHT);
			break;
		/* 定时器更新 */
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
				case ID_VOLUME_REDUC_BUTTON: /* 减少当前音量 */
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: //按钮被按下并释放
							ALARM_VOLUME --;
							if(ALARM_VOLUME < 0)
							{
								ALARM_VOLUME = 7;
							}
                            WM_InvalidateWindow(hItem);
							break;	
					}
					break;
                    
				case ID_VOLUME_INCEA_BUTTON: /* 增加当前音量 */
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
                case ID_LANGUAGE_RCHANGE_BUTTON: /* 切换当前语言 */
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
				case ID_LANGUAGE_LCHANGE_BUTTON: /* 切换当前语言 */
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
                case ID_SETTING_BACK_BUTTON: /* 返回设置菜单界面 */
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





