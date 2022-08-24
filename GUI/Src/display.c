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
*	�� �� ��: _cbBkWindow
*	����˵��: ���洰�ڻص�����
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbBkWindow(WM_MESSAGE* pMsg) 
{
	switch (pMsg->MsgId) 
	{
		case WM_PAINT: 
			{				
				RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);                            //��ȡ��ǰʱ��
				RTC_GetDate(RTC_Format_BIN,&RTC_DateStructure);                            //��ȡ��ǰ����
				_PaintFrameSta();                                                          //����״̬��
				GUI_DispStringHCenterAt(":", 134, 10 + MAIN_TITLE_HEIGHT);
				GUI_DispDecAt(RTC_TimeStructure.RTC_Hours,100,10 + MAIN_TITLE_HEIGHT,2);   //��ʾСʱ
				GUI_DispDecAt(RTC_TimeStructure.RTC_Minutes,140,10 + MAIN_TITLE_HEIGHT,2); //��ʾ����
			} 
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}




/*
*********************************************************************************************************
*	�� �� ��: MainTask
*	����˵��: ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MainTask(void) 
{
	GUI_Init();
	
	/* ʹ�ܴ���ʹ���ڴ��豸 */
	// WM_SetCreateFlags(WM_CF_MEMDEV);
	WM_MULTIBUF_Enable(1);
	
	/* ʹ�ܿؼ���Ƥ��ɫ */
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
	
	//�������еı���ͼƬ
	hConButtonStreamBitmap = _LoadStreamBitmap("0:APP/contiue.dta");
	hPauseButtonStreamBitmap = _LoadStreamBitmap("0:APP/pause.dta");
	hSetsButtonStreamBitmap = _LoadStreamBitmap("0:APP/setsml.dta");
	hStopButtonStreamBitmap = _LoadStreamBitmap("0:APP/stop.dta");
	/* �������洰�ڵĻص����� */
	WM_SetCallback(WM_HBKWIN, &_cbBkWindow);
	
	/* ���������� */
	_CreateFrame(&_cbdepth_modle);
	
//	CreateSysInfoDlg();
	
	/* ʹ��UTF8���� */
	
	while(1)
	{
		GUI_Exec();
		GUI_Delay(10);
	}
}


/* ϵͳ�������������ʾ */
void _system_start(int prg)
{
	static int i;
	static int j = 100;

	char string_buf[5];
	
	POINT_COLOR=BLUE;                                              //���û�����ɫ
	LCD_ShowString(30,70,320,20,24,"System is starting !");        //��ʾϵͳ��������

	if(prg > 100  || prg < 0)return;                               //���鴫���Ƿ���ȷ
	LCD_DrawRectangle(99,220,404,250);                             //���ƽ��������
	for(i = i;i<=prg;i++)
	{		
		LCD_Fill(j,221,j+3,249,BLACK);                             //���ƽ�����
		
		j += 3;
		
		delay_ms(15);
		
		POINT_COLOR=RED; 
				
		sprintf(string_buf,"%d%%",i);
		
		LCD_Fill(200,200,400,219,GRAYBLUE);                        //�����һ����ʾ������
		
		LCD_ShowString(240,200,200,16,16,(u8 *)string_buf);        //��ʾʵʱ����
	}

}

