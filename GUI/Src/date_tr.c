#include "date_tr.h"
#include "set_menu.h"
#include "alarm_log.h"
#include "time.h"

/***
 * 平常产生的数据存储到SD卡
 * 当打开数据传输界面并选择U盘传输的时候
 * 循环检测U盘是否插入,当检测到U盘插入的时候,更新专状态栏图标
 * 显示U盘插入图标
 * 点击开始传输数据按钮,开始传输数据
 ***/



/***
 * 两个界面,一个界面选择传输方式
 * 第一个传输方式为蓝牙传输
 * 第二个传输方式为U盘传输
 ***/

/***
 * 蓝牙传输方式
 ***/

/***
 * U盘传输方式
 * 打开U盘传输界面
 * 等待U盘连接
 * U盘已连接
 * 点击传输按钮
 * 开始传输数据
 ***/
/***
 * 实现数据传输所需要的按钮
 ***/
//蓝牙选择按钮
#define ID_BULETOOTH_SELECT_BUTTON (GUI_ID_USER + 0x00)
//U盘选择按钮
#define ID_U_DISK_SELECT_BOTTON    (GUI_ID_USER + 0x01)
//向上移动列表
//向下移动列表
//确认连接
//开始传输数据
#define ID_STSRT_TRANS_BOTTON      (GUI_ID_USER + 0x05) 
//返回上一级菜单
#define ID_DATE_BACK_BUTTON        (GUI_ID_USER + 0x06)


//U盘是否插入
int U_DISK_READY = 0;


extern u8           copy_sd_data_to_u;
extern OS_SEM       AppPrintfSemp;	/* 用于printf互斥 */
extern OS_SEM       SEM_SYNCH;	    /* 用于同步 */

void _cbtrmethod_menu(WM_MESSAGE* pMsg);
void _cbtrdate(WM_MESSAGE* pMsg);

int U_Disk_Flag = 0;
/*
*********************************************************************************************************
*	函 数 名: _cbtrmethod_menu
*	功能说明: 数据传输方式选择菜单界面
*	形    参：pMsg  参数指针
*	返 回 值: 无
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
			/* 设置聚焦 */
			WM_SetFocus(hItem);
            //创建此页面所需的所有按钮
			GUI_SetFont(&GUI_FontHZ24);
			_CreateButton_P(hItem, " ",  ID_DATE_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton(hItem, "蓝牙",   ID_BULETOOTH_SELECT_BUTTON, 50, 120 + MAIN_TITLE_HEIGHT, 190,  160, 0);
			_CreateButton(hItem, "U盘",      ID_U_DISK_SELECT_BOTTON, 240, 120 + MAIN_TITLE_HEIGHT, 190,  160, 0);
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
            GUI_SetFont(&GUI_FontHZ32);
            GUI_DispStringHCenterAt("数据传输", 220, 70 - MAIN_TITLE_HEIGHT);		           
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
                case ID_DATE_BACK_BUTTON: //返回设置菜单
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
                case ID_U_DISK_SELECT_BOTTON: //打开U盘传输界面
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
				case ID_BULETOOTH_SELECT_BUTTON: //点击蓝牙按钮
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
*	函 数 名: _cbtrdate
*	功能说明: 数据传输界面
*	形    参：pMsg  参数指针
*	返 回 值: 无
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
			/* 设置聚焦 */
			WM_SetFocus(hItem);
            //创建此页面所需的所有按钮
			GUI_SetFont(&GUI_FontHZ24);
            _CreateButton_P(hItem, " ",  ID_DATE_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton(hItem, "开始",      ID_STSRT_TRANS_BOTTON, 240, 120 + MAIN_TITLE_HEIGHT, 190,  160, 0);
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
            GUI_DispStringHCenterAt("数据传输", 220, 70 - MAIN_TITLE_HEIGHT);		
            if(U_DISK_READY)//如果U盘已经插入
            {
                GUI_DispStringHCenterAt("U盘 插入", 100, 120 - MAIN_TITLE_HEIGHT);	
            }
            else//提示插入U盘
            {
                GUI_DispStringHCenterAt("U盘 拔出", 100, 120 - MAIN_TITLE_HEIGHT);	
            }
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
                case ID_DATE_BACK_BUTTON: //返回设置菜单
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
				case ID_STSRT_TRANS_BOTTON: //开始传输数据
                switch(NCode) 
                {
                    case WM_NOTIFICATION_CLICKED:
                        break;
                    case WM_NOTIFICATION_RELEASED:
						/* 开始复制文件到U盘 */
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

