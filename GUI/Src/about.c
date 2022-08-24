#include "about.h"
#include "set_menu.h"

/***
 * �򿪴�ҳ��,��ʾӲ���������Ϣ
 ***/

//���ذ�ť
#define ID_ABOUT_BACK_BUTTON    (GUI_ID_USER + 0x00)
/* ��ʾ��ǰӲ������汾 */
const char * PB_hardware_version = "PB-Ӳ���汾:V1.0_20220812";
const char * PB_software_version = "PB-����汾:V1.0.0_20220812";
const char * CB_hardware_version = "CB-Ӳ���汾:V1.0_20220812";
const char * CB_software_version = "CB-����汾:V1.0.0_20220812";

/*
*********************************************************************************************************
*	�� �� ��: draw_about_sheet
*	����˵��: ���ƹ��ڱ���������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void draw_about_sheet(void)
{
	//���û�����ɫ
	GUI_SetColor(GUI_BLUE_98);
	//���û��ʴ�С
	GUI_SetPenSize(4);
	//��ʼ���Ʊ��
	GUI_DrawLine(0,  120,480,120);
	GUI_DrawLine(0,  200,480,200);
	GUI_DrawLine(0,  280,480,280);
}


/*
*********************************************************************************************************
*	�� �� ��: _cbabout
*	����˵��: ������־��ʾ����
*	��    �Σ�pMsg  ����ָ��
*	�� �� ֵ: 
*********************************************************************************************************
*/
void _cbabout(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
	/* ������ʱ�� */
    int     NCode;
	int     Id;
	// char    string_buf[10];

	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* ���þ۽� */
			WM_SetFocus(hItem);
            //������ҳ����������а�ť
            GUI_SetFont(&GUI_FontHZ24);
			_CreateButton_P(hItem, " ",  ID_ABOUT_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			WM_CreateTimer(hItem, /* ������Ϣ�Ĵ��ڵľ�� */
						   0, 	                     /* �û������Id���������ͬһ����ʹ�ö����ʱ������ֵ��������Ϊ�㡣 */
						   10,                       /* ���ڣ������ڹ���ָ������Ӧ�յ���Ϣ*/
						   0);
		case WM_PAINT:
			_PaintFrame();	
			draw_about_sheet();
			_PaintFrameSta();
			
			GUI_SetFont(&GUI_FontHZ24);

			GUI_DispStringHCenterAt("���ڱ���", 230, 70 - MAIN_TITLE_HEIGHT);
			//��ʾ������Ϣ
			GUI_DispStringHCenterAt(PB_hardware_version, 240, 130 - MAIN_TITLE_HEIGHT);
			GUI_DispStringHCenterAt(PB_software_version, 240, 160 - MAIN_TITLE_HEIGHT);
			GUI_DispStringHCenterAt(CB_hardware_version, 240, 210 - MAIN_TITLE_HEIGHT);
			GUI_DispStringHCenterAt(CB_software_version, 240, 240 - MAIN_TITLE_HEIGHT);
			break;
		/* ��ʱ������ */
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
                case ID_ABOUT_BACK_BUTTON: //���ð�ť��֪ͨ����,�����ý���
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* ɾ����������Ľ��� */
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



