#include "status.h"


/*
*********************************************************************************************************
*                                       ����
*********************************************************************************************************
*/
uint8_t g_ucNETDetect = 0;
WM_HWIN hWinStatus;



/*
*********************************************************************************************************
*				                         �Ի����ʼ��ѡ��
*********************************************************************************************************
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateSysInfo[] = 
{
    { WINDOW_CreateIndirect,  "",   0,   0,  444,  800,36,0},
	{ TEXT_CreateIndirect,    "",   GUI_ID_TEXT0, 715,2, 85, 16, 0,0},
    { TEXT_CreateIndirect,    "",   GUI_ID_TEXT1, 715,18,85, 16, 0,0},
};

/*
*********************************************************************************************************
*	�� �� ��: Caculate_RTC
*	����˵��: ��ʾRTCʱ��
*	��    ��: pMsg  WM_MESSAGE����ָ�����           
*	�� �� ֵ: ��
*********************************************************************************************************
*/
extern RTC_TimeTypeDef  RTC_TimeStructure;
extern RTC_DateTypeDef  RTC_DateStructure;

// static void Caculate_RTC(WM_MESSAGE * pMsg)
// {
//	  char buf[30];
//	  WM_HWIN hWin = pMsg->hWin;

	//   RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
	//   RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

//	  sprintf(buf, 
//	          "%0.2d:%0.2d:%0.2d %0.3s", 
//			  RTC_TimeStructure.RTC_Hours, 
//			  RTC_TimeStructure.RTC_Minutes,
//			  RTC_TimeStructure.RTC_Seconds, 
//			  ucWeekDay[RTC_DateStructure.RTC_WeekDay-1]);
//	 TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT1), buf);
// 
//	  sprintf(buf, 
//	          "20%0.2d/%0.2d/%0.2d", 
//			  RTC_DateStructure.RTC_Year, 
//			  RTC_DateStructure.RTC_Month, 
//			  RTC_DateStructure.RTC_Date);
//	  TEXT_SetText(WM_GetDialogItem(hWin,GUI_ID_TEXT0), buf); 	

// }

/*
*********************************************************************************************************
*	�� �� ��: PaintDialogSysInfo
*	����˵��: �ػغ��� 
*	��    ��: pMsg  WM_MESSAGE����ָ�����  
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PaintDialogSysInfo(WM_MESSAGE * pMsg)
{

	int        xSize;
	int        ySize;
	static int Reception = 3; 
	int        xPos;
	int        i;

    xSize = WM_GetWindowSizeX(pMsg->hWin);
    ySize = WM_GetWindowSizeY(pMsg->hWin);	
	
	/* ��ر�־ */
	GUI_SetColor(0x303030);
    GUI_FillRect(0, 2, xSize - 1, ySize - 1);
	
    GUI_SetColor(0x808080);
    GUI_DrawHLine(0, 0, xSize - 1);
	
    GUI_SetColor(0x404040);
    GUI_DrawHLine(1, 0, xSize - 1);
	
	// GUI_DrawBitmap(&_bmBatteryEmpty_27x14, 680,10);
    // GUI_DrawBitmap(_apbmCharge[4], 680, 10);
	
	/* WIFI�źŻ��� */
	GUI_SetColor(GUI_WHITE);
	for (xPos = 650, i = 0; i < Reception; i++) 
	{
		GUI_DrawVLine(xPos + i * 4 + 0, 18 - i * 2, 20);
		GUI_DrawVLine(xPos + i * 4 + 1, 18 - i * 2, 20);
	}
	
	GUI_SetColor(GUI_GRAY);
	for (; i < 5; i++) {
		GUI_DrawVLine(xPos + i * 4 + 0, 18 - i * 2, 20);
		GUI_DrawVLine(xPos + i * 4 + 1, 18 - i * 2, 20);
	}
	
	/* ���ӻ��� */
	// GUI_DrawBitmap(&_bmAlarm_16x16, 620, 10);
	
	// /* ���߲�α�־ */
	// if(g_ucNETDetect == 1)
	// {
	// 	GUI_DrawBitmap(&bmNetStatus, 3, 2);		
	// }
	// else
	// {
	// 	GUI_DrawBitmap(&bmNetStatus, 3, 2);
	// 	GUI_DrawBitmap(&bmLinkError, 3+16, 2+16);		
	// }

	// /* SD����α�־ */
	// if(s_ucSDDetect == 1)
	// {
	// 	GUI_DrawBitmap(&bmSdcard, 40, 2);		
	// }
	// else
	// {
	// 	GUI_DrawBitmap(&bmSdcard, 40, 2);	
	// 	GUI_DrawBitmap(&bmLinkError, 40+16, 2+16);	
	// }

	// /* U�̲�α�־ */
	// GUI_DrawBitmap(&bmusbdisk, 77, 2);
	// GUI_DrawBitmap(&bmLinkError, 77+16, 2+16);

	// /* USB�����߲�α�־ */
	// GUI_DrawBitmap(&bmusbtrans, 77+37, 2);
	// GUI_DrawBitmap(&bmLinkError, 77+37+16, 2+16);	
}

/*
*********************************************************************************************************
*	�� �� ��: InitDialogSysInfo
*	����˵��: �Ի����ʼ��
*	��    ��: pMsg  WM_MESSAGE����ָ�����  
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void InitDialogSysInfo(WM_MESSAGE * pMsg)
{
	WM_HWIN hItem;
	WM_HWIN hWin = pMsg->hWin; 

	/* ������ʱ�� */
	WM_CreateTimer(hWin, 0, 1000, 0);	        

	//
    //��ʼ���ı�GUI_ID_TEXT0
    //
	hItem = WM_GetDialogItem(hWin,GUI_ID_TEXT0);
    TEXT_SetBkColor(hItem, 0x303030);
	TEXT_SetTextColor(hItem, GUI_WHITE);
    TEXT_SetFont(hItem, &GUI_Font16B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_VCENTER|GUI_TA_CENTER);
	
    //
    //��ʼ���ı�GUI_ID_TEXT1
    //
	hItem = WM_GetDialogItem(hWin,GUI_ID_TEXT1);	
    TEXT_SetBkColor(hItem, 0x303030);
	TEXT_SetTextColor(hItem, GUI_WHITE);
    TEXT_SetFont(hItem, &GUI_Font16B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_VCENTER|GUI_TA_CENTER);

	Caculate_RTC(pMsg);
}

/*
*********************************************************************************************************
*	�� �� ��: _cbCallbackSysInfo
*	����˵��: �ص����� 
*	��    ��: pMsg  WM_MESSAGE����ָ�����  
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void _cbCallbackSysInfo(WM_MESSAGE * pMsg) 
{
//	WM_HWIN hWin = pMsg->hWin;	
	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			InitDialogSysInfo(pMsg);
			break;
		
		/* ��ʱ������ */
		case WM_TIMER:
			Caculate_RTC(pMsg);
			WM_RestartTimer(pMsg->Data.v, 1000);
			break;

		case WM_PAINT:
			PaintDialogSysInfo(pMsg);
			break;

		/* SD������ʱ��������� */
		// case MSG_SDPlugIn:
		// 	s_ucSDDetect = 1;
		// 	WM_InvalidateWindow(hWin);
		// 	break;

		// /* SD���γ��󣬽������ */
		// case MSG_SDPlugOut:
		// 	s_ucSDDetect = 0;
		// 	WM_InvalidateWindow(hWin);
		// 	break;
		
		// /* ���߲���ʱ��������� */
		// case MSG_NETPlugIn:
		// 	g_ucNETDetect = 1;
		// 	WM_InvalidateWindow(hWin);
		// 	break;

		// /* ���߰γ��󣬽������ */
		// case MSG_NETPlugOut:
		// 	g_ucNETDetect = 0;
		// 	WM_InvalidateWindow(hWin);
		// 	break;

		default:
			WM_DefaultProc(pMsg);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: CreateSysInfoDlg
*	����˵��: ϵͳ��Ϣ�Ի���
*	��    ��: ��   	
*	�� �� ֵ: ���ھ��
*********************************************************************************************************
*/
void CreateSysInfoDlg(void) 
{
	
	/* ����һ���Ի���, ����XPϵͳ�������� */
	hWinStatus = GUI_CreateDialogBox(_aDialogCreateSysInfo, 
	                                 GUI_COUNTOF(_aDialogCreateSysInfo), 
								     &_cbCallbackSysInfo, 
								     0, 
								     0, 
								     0);
	
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/

