#include "display.h"
#include "gui_public.h"
#include "main_menu.h"
#include "status.h"
#include "time.h"
#include "rtc.h"
#include "tftlcd.h"
#include "delay.h"
#include "includes.h"


extern RTC_TimeTypeDef  RTC_TimeStructure;
extern RTC_DateTypeDef  RTC_DateStructure;
/*
*********************************************************************************************************
*	函 数 名: _cbBkWindow
*	功能说明: 桌面窗口回调函数
*	形    参：pMsg  参数指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbBkWindow(WM_MESSAGE* pMsg) 
{
	switch (pMsg->MsgId) 
	{
		case WM_PAINT: 
			{				
				RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);                            //获取当前时间
				RTC_GetDate(RTC_Format_BIN,&RTC_DateStructure);                            //获取当前日期
				_PaintFrameSta();                                                          //绘制状态栏
				GUI_DispStringHCenterAt(":", 134, 10 + MAIN_TITLE_HEIGHT);
				GUI_DispDecAt(RTC_TimeStructure.RTC_Hours,100,10 + MAIN_TITLE_HEIGHT,2);   //显示小时
				GUI_DispDecAt(RTC_TimeStructure.RTC_Minutes,140,10 + MAIN_TITLE_HEIGHT,2); //显示分钟
			} 
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}




/*
*********************************************************************************************************
*	函 数 名: MainTask
*	功能说明: 主函数
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void MainTask(void) 
{
	GUI_Init();
	
	/* 使能窗口使用内存设备 */
	// WM_SetCreateFlags(WM_CF_MEMDEV);
	WM_MULTIBUF_Enable(1);
	
	/* 使能控件的皮肤色 */
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	FRAMEWIN_SetDefaultSkin(FRAMEWIN_SKIN_FLEX);
	PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	CHECKBOX_SetDefaultSkin(CHECKBOX_SKIN_FLEX);
	DROPDOWN_SetDefaultSkin(DROPDOWN_SKIN_FLEX);
	SCROLLBAR_SetDefaultSkin(SCROLLBAR_SKIN_FLEX);
	SLIDER_SetDefaultSkin(SLIDER_SKIN_FLEX);
	HEADER_SetDefaultSkin(HEADER_SKIN_FLEX);
	RADIO_SetDefaultSkin(RADIO_SKIN_FLEX);
	
	//加载所有的背景图片
	hConButtonStreamBitmap = _LoadStreamBitmap("0:APP/contiue.dta");
	hPauseButtonStreamBitmap = _LoadStreamBitmap("0:APP/pause.dta");
	hSetsButtonStreamBitmap = _LoadStreamBitmap("0:APP/setsml.dta");
	hStopButtonStreamBitmap = _LoadStreamBitmap("0:APP/stop.dta");
	/* 设置桌面窗口的回调函数 */
	WM_SetCallback(WM_HBKWIN, &_cbBkWindow);
	
	/* 进入主界面 */
	_CreateFrame(&_cbdepth_modle);
	
//	CreateSysInfoDlg();
	
	/* 使能UTF8解码 */
	
	while(1)
	{
		GUI_Exec();
		GUI_Delay(10);
	}
}


/* 系统启动界面进度提示 */
void _system_start(int prg)
{
	static int i;
	static int j = 100;

	char string_buf[5];
	
	POINT_COLOR=BLUE;                                              //设置画笔颜色
	LCD_ShowString(30,70,320,20,24,"System is starting !");        //显示系统正在启动

	if(prg > 100  || prg < 0)return;                               //检验传参是否正确
	LCD_DrawRectangle(99,220,404,250);                             //绘制进度条外框
	for(i = i;i<=prg;i++)
	{		
		LCD_Fill(j,221,j+3,249,BLACK);                             //绘制进度条
		
		j += 3;
		
		delay_ms(15);
		
		POINT_COLOR=RED; 
				
		sprintf(string_buf,"%d%%",i);
		
		LCD_Fill(200,200,400,219,GRAYBLUE);                        //清除上一次显示的数字
		
		LCD_ShowString(240,200,200,16,16,(u8 *)string_buf);        //显示实时进度
	}

}

