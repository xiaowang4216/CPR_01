#include "about.h"
#include "set_menu.h"

/***
 * 打开此页面,显示硬件及软件信息
 ***/

//返回按钮
#define ID_ABOUT_BACK_BUTTON    (GUI_ID_USER + 0x00)
/* 显示当前硬件软件版本 */
const char * PB_hardware_version = "PB-硬件版本:V1.0_20220812";
const char * PB_software_version = "PB-软件版本:V1.0.0_20220812";
const char * CB_hardware_version = "CB-硬件版本:V1.0_20220812";
const char * CB_software_version = "CB-软件版本:V1.0.0_20220812";

/*
*********************************************************************************************************
*	函 数 名: draw_about_sheet
*	功能说明: 绘制关于本机界面表格
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void draw_about_sheet(void)
{
	//设置画笔颜色
	GUI_SetColor(GUI_BLUE_98);
	//设置画笔大小
	GUI_SetPenSize(4);
	//开始绘制表格
	GUI_DrawLine(0,  120,480,120);
	GUI_DrawLine(0,  200,480,200);
	GUI_DrawLine(0,  280,480,280);
}


/*
*********************************************************************************************************
*	函 数 名: _cbabout
*	功能说明: 警告日志显示界面
*	形    参：pMsg  参数指针
*	返 回 值: 
*********************************************************************************************************
*/
void _cbabout(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
	/* 创建定时器 */
    int     NCode;
	int     Id;
	// char    string_buf[10];

	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* 设置聚焦 */
			WM_SetFocus(hItem);
            //创建此页面所需的所有按钮
            GUI_SetFont(&GUI_FontHZ24);
			_CreateButton_P(hItem, " ",  ID_ABOUT_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			WM_CreateTimer(hItem, /* 接受信息的窗口的句柄 */
						   0, 	                     /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
						   10,                       /* 周期，此周期过后指定窗口应收到消息*/
						   0);
		case WM_PAINT:
			_PaintFrame();	
			draw_about_sheet();
			_PaintFrameSta();
			
			GUI_SetFont(&GUI_FontHZ24);

			GUI_DispStringHCenterAt("关于本机", 230, 70 - MAIN_TITLE_HEIGHT);
			//显示本机信息
			GUI_DispStringHCenterAt(PB_hardware_version, 240, 130 - MAIN_TITLE_HEIGHT);
			GUI_DispStringHCenterAt(PB_software_version, 240, 160 - MAIN_TITLE_HEIGHT);
			GUI_DispStringHCenterAt(CB_hardware_version, 240, 210 - MAIN_TITLE_HEIGHT);
			GUI_DispStringHCenterAt(CB_software_version, 240, 240 - MAIN_TITLE_HEIGHT);
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
                case ID_ABOUT_BACK_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            _DeleteFrame();   
							_PaintFrameSta();
                            _CreateFrame(&_cbsystem_menu_3);
							break;
					}
					break;
			}
			break;
		default:
		WM_DefaultProc(pMsg);
	}
}



