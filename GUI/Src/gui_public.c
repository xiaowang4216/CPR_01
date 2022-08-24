#include "gui_public.h"
#include "rtc.h"
#include "timer.h"
#include "adc.h"

/* ���´��ھ�� */
WM_HWIN    _hLastFrame;
/* �������ھ�� */
WM_HWIN    _hWarnFrame;


unsigned   _Language;
WM_HWIN    _hTitle;

//ֹͣ��ť����ͼƬ
GUI_HMEM hStopButtonStreamBitmap;
GUI_BITMAP StopBitmap;
//��ͣ��ť����ͼƬ
GUI_HMEM hPauseButtonStreamBitmap;
GUI_BITMAP PauseBitmap;
//������ť����ͼƬ
GUI_HMEM hConButtonStreamBitmap;
GUI_BITMAP ConBitmap;
//����С��ť����ͼƬ
GUI_HMEM hSetsButtonStreamBitmap;
GUI_BITMAP SetsBitmap;

//ʱ��ṹ��
extern RTC_TimeTypeDef  RTC_TimeStructure;
extern RTC_DateTypeDef  RTC_DateStructure;

//logo
extern GUI_CONST_STORAGE GUI_BITMAP bmLogo_armflySmall;
/*
*********************************************************************************************************
*                                       ��̬����
*********************************************************************************************************
*/
const char * _aLang[][2] = {
  { "ȡ��",
    "Cancel"
  },
  { "����",
    "Correction"
  },
  { "ȷ��",
    "Confirm"
  },
  { "���������룺",
    "Please enter your PIN:"
  },
  { "��ѡ����",
    "Please select amount"
  },
  { "�������������4λ",
    "Wrong PIN. At least 4 digits"
  },
  { "��",
    "jia"
  },
  { "��",
    "Please enter amount"
  },
  { "��������",
    "jie"
  },
  { "��",
    "Please enter amount"
  },
  { "��",
    "Please enter amount"
  },
  { "��ͣ",
    "pause"
  },
  { "����",
    "set"
  },
  { "ֹͣ",
    "stop"
  },
  { "����",
    "back"
  },

};


/*
*********************************************************************************************************
*	�� �� ��: _GetLang
*	����˵��: ��ȡҪ��ʾ�ַ����ĵ�ַ
*	��    �Σ�Index  Ҫ��ʾ�ַ��������
*	�� �� ֵ: p      ����Ҫ��ʾ�ַ�����ַ
*********************************************************************************************************
*/
const char * _GetLang(unsigned Index) 
{
	const char * p;
	
	p = NULL;
	Index--;
	
	if ((Index < GUI_COUNTOF(_aLang)) && (_Language < GUI_COUNTOF(_aLang[0]))) 
	{
		p = _aLang[Index][_Language];
	}
	
	return p;
}



/*
*********************************************************************************************************
*	�� �� ��: _DrawDownRectEx
*	����˵��: ��ʾ���ݵľ��ο�
*	��    �Σ�pEffect  �ؼ���ʾЧ��
*             pRect    GUI_RECT���ͱ�����ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _DrawDownRectEx(const WIDGET_EFFECT* pEffect, const GUI_RECT* pRect) 
{
	WM_LOCK();
	pEffect->pfDrawDownRect(pRect);
	WM_UNLOCK();
}

/*
*********************************************************************************************************
*	�� �� ��: _DrawDownRect
*	����˵��: ��ʾ���ݵľ��ο�
*	��    �Σ�pEffect  �ؼ���ʾЧ��
*             x0       ��ʼx������
*             y0       ��ʼy������
*             x1       ����x������
*             y1       ����y������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _DrawDownRect(const WIDGET_EFFECT* pEffect, int x0, int y0, int x1, int y1) 
{
	GUI_RECT r;
	
	r.x0 = x0;
	r.y0 = y0;
	r.x1 = x1;
	r.y1 = y1;
	_DrawDownRectEx(pEffect, &r);
}

/*
*********************************************************************************************************
*	�� �� ��: _DrawUpRectEx
*	����˵��: ��ʾ͹��ľ��ο�
*	��    �Σ�pEffect  �ؼ���ʾЧ��
*             pRect    GUI_RECT���ͱ�����ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _DrawUpRectEx(const WIDGET_EFFECT* pEffect, const GUI_RECT* pRect) 
{
	WM_LOCK();
	pEffect->pfDrawUpRect(pRect);
	WM_UNLOCK();
}




/* ����������ʾ�߶� */
#define WARN_HEIGHT  40
/* ����������ʾ��� */
#define WAEN_WIDTH 200
/* Ĭ����ʾʱ�� */
#define DEFAULT_TIME 10 
/* ��������λ��x */
#define WARN_X 140
/* ��������λ��y */
#define WARN_Y 10
/* ����������Ϣ�ṹ�� */
struct  warn_msg m_ifo ={NULL,0,0};

/**
 * @brief  ��������
 * @param  contents: ��Ҫ����������,����̫��,�����ʾ12������
 * @param  grade_value: �����ĵȼ�,������,1,2,3
 * @return uint8_t: 0�ɹ�,1��������
 */
uint8_t change_warn(char * contents,int grade_value)
{
	/* �������ݸ�ֵ */
	m_ifo.grade = grade_value;
	/* �����ȼ���ֵ */
	m_ifo.w_msg = contents;
	/* ����Ϣ */
	m_ifo.new_mag = 1;
	/* ������������� */
	if(m_ifo.grade > 3 ||m_ifo.w_msg == NULL)
	{
		/* �޸�����,����ʾ */
		m_ifo.grade = 0;
		return 1;
	}		
	return 0;
}

/**
 * @brief ����������ʾ��
 */
static show_warn(void)
{
	/* ��ʾʱ�� */
	static int count = DEFAULT_TIME;
	//���Ʊ�����ʾ				
	if(m_ifo.grade && count)
	{
		GUI_SetFont(&GUI_FontHZ16);
		GUI_SetColor(MAIN_TEXTCOLOR);
		/* ���ݱ����ȼ���ʾ��ͬ������ɫ */
		switch (m_ifo.grade)
		{
		case 1:
			GUI_SetBkColor(GUI_WHITE);
		break;
		case 2:
			GUI_SetBkColor(GUI_YELLOW);
		break;
		case 3:
			GUI_SetBkColor(GUI_RED);
		break;
		default:
			GUI_SetBkColor(GUI_WHITE);
			break;
		}
		count --;
		/* ��ʾ���������� */
		GUI_ClearRect(WARN_X,WARN_Y,WAEN_WIDTH + WARN_X,WARN_HEIGHT + WARN_Y);
		/* ��ʾ�������� */
		GUI_DispStringHCenterAt(m_ifo.w_msg, 240,20);
	}	
	/* �µ���Ϣ���� */
	if(m_ifo.new_mag == 1)
	{
		count = DEFAULT_TIME;
		m_ifo.new_mag  = 0;
	}
	/* ��ʾʱ�䵽�� */
	else if(count == 0)
	{
		count = DEFAULT_TIME;
		m_ifo.grade = 0;
		m_ifo.w_msg = NULL;
	}
}

/**
 * @brief ���Ƶ�ǰ��ص���,����������ĳ��ֵ��ʱ��
 * ������ʾ��ص�����
 * @return uint8_t: 0��������
 */
uint8_t display_battery(void)
{
	int        bat_vol;
	int        i,temp;
	static int flag = 1;
	//��ȡ��ǰ��ѹ
	bat_vol = Get_Adc_Average(1,ADC_Channel_5,20);
	bat_vol=(float)bat_vol*(3.3/4096);          //��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111

	/* ���Ƶ�����ʾ��� */
	GUI_SetColor(GUI_WHITE);
	GUI_DrawRect(383,18,432,42);
	/* ��ص����� */
	if(bat_vol<=1)
	{
		GUI_SetColor(GUI_RED);
		/* ֻʣһ����� */
		GUI_FillRect(385,20,390,40);
		/* ֻ��ʾһ�ε�ص����� */
		if(flag)
		{
			change_warn("��ص�������",2);	
			/* д���������־ */
			write_log(1);
			flag = 0;
		}		
	}
	else
	{
		GUI_SetBkColor(MAIN_BKCOLOR);
		GUI_SetColor(MAIN_TEXTCOLOR);
		GUI_SetFont(MAIN_FONT);
	}
	for(i=0;i<bat_vol;i++)
	{
		/* ����һ������ */
		temp = 15 * i;	
		GUI_FillRect(400 +temp -15,20,400+temp,40);		
	}
	/* �ظ�Ĭ�������ʽ */
	GUI_SetBkColor(MAIN_BKCOLOR);
	GUI_SetColor(MAIN_TEXTCOLOR);
	GUI_SetFont(MAIN_FONT);

	return 0;
}
/*
*********************************************************************************************************
*	�� �� ��: _PaintFrameSta
*	����˵��: ��ܴ���״̬�����ػ溯��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _PaintFrameSta(void) 
{			
	//��ȡ��ǰʱ��
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN,&RTC_DateStructure);
	
	GUI_SetBkColor(MAIN_BKCOLOR);
	GUI_SetColor(MAIN_TEXTCOLOR);
	GUI_SetFont(MAIN_FONT);
	GUI_ClearRect(0,0,480,60);
	/* ��������ͼ�� */
	GUI_DrawBitmap(&bmblue_tooth, 340, 10);
	GUI_DrawBitmap(&bmwarm, 80, 10);
	//��ʾʱ��
	GUI_DispStringHCenterAt(":", 44, 10 + MAIN_TITLE_HEIGHT);
	GUI_DispDecAt(RTC_TimeStructure.RTC_Hours,10,10 + MAIN_TITLE_HEIGHT,2);
	GUI_DispDecAt(RTC_TimeStructure.RTC_Minutes,50,10 + MAIN_TITLE_HEIGHT,2);
	//��ʾ��ص���
	display_battery();
	/* ��ʾ�������� */
	show_warn();
}


/*
*********************************************************************************************************
*	�� �� ��: _PaintFrame
*	����˵��: ��ܴ��ڵ��ػ溯��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _PaintFrame(void) 
{

	GUI_SetBkColor(FRAME_BKCOLOR);
	GUI_SetColor(FRAME_TEXTCOLOR);
	GUI_SetFont(FRAME_FONT);
	GUI_SetTextMode(GUI_TM_TRANS);
	GUI_ClearRect(0,61,480,320);
	
}



/*
*********************************************************************************************************
*	�� �� ��: _CreateFrame
*	����˵��: ������ܴ���
*	��    �Σ�cb  �ص�������ַ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
WM_HWIN _CreateFrame(WM_CALLBACK* cb) 
{
	int x = 0;
	int y = 0;
//	x = FRAME_BORDER + MAIN_BORDER;
//	y = FRAME_BORDER + MAIN_TITLE_HEIGHT;
	_hLastFrame = WM_CreateWindowAsChild(x, y, FRAME_WIDTH, FRAME_HEIGHT, WM_HBKWIN, WM_CF_SHOW, cb, 0);
	return _hLastFrame;
}

/*
*********************************************************************************************************
*	�� �� ��: _DeleteFrame
*	����˵��: ɾ�������Ŀ�ܴ���
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void _DeleteFrame(void) 
{
	WM_DeleteWindow(_hLastFrame);
	_hLastFrame = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: _CreateButton
*	����˵��: ������ť
*	��    �Σ�hParent  ������
*             pText    ��������ʾ���ı�
*             Id       ��ťId         
*             x        x������
*             y        y������
*             w        ��ť��
*             h        ��ť��
*             TextId   �ı���ID     
*	�� �� ֵ: ��
*********************************************************************************************************
*/
WM_HWIN _CreateButton(WM_HWIN hParent, const char* pText, int Id, int x, int y, int w, int h, unsigned TextId) 
{
	WM_HWIN hButton;
	hButton = BUTTON_CreateEx(x, y, w, h, hParent, WM_CF_SHOW, 0, Id);
	
	/* ��ȡ��ǰ������ťҪ��ʾ���ı� */
	if (TextId) 
	{
		pText = _GetLang(TextId);
	}
	// BUTTON_SetTextAlign();
	BUTTON_SetText      (hButton,    pText);
	BUTTON_SetFont      (hButton,    FRAME_BUTTON_FONT);
	BUTTON_SetBkColor   (hButton, 0, FRAME_BUTTON_BKCOLOR0);
	BUTTON_SetBkColor   (hButton, 1, FRAME_BUTTON_BKCOLOR1);
	BUTTON_SetBkColor   (hButton, 2, FRAME_BUTTON_BKCOLOR2);
	BUTTON_SetTextColor (hButton, 0, FRAME_BUTTON_COLOR0);
	BUTTON_SetTextColor (hButton, 1, FRAME_BUTTON_COLOR1);
	BUTTON_SetTextColor (hButton, 2, FRAME_BUTTON_COLOR0);
	WIDGET_SetEffect    (hButton,    FRAME_BUTTON_EFFECT);
	
	/* ���ý������뽹������� */
	BUTTON_SetFocussable(hButton,    1);
	
	BUTTON_SetBkColor(hButton,BUTTON_BI_UNPRESSED,FRAME_BKCOLOR);
	return hButton;
}

WM_HWIN _CreateButton_P(WM_HWIN hParent, const char* pText, int Id, int x, int y, int w, int h, unsigned TextId,const GUI_BITMAP *pBitmap,int pos) 
{
	WM_HWIN hButton;
	hButton = BUTTON_CreateEx(x, y, w, h, hParent, WM_CF_SHOW, 0, Id);
	
	/* ��ȡ��ǰ������ťҪ��ʾ���ı� */
	if (TextId) 
	{
		pText = _GetLang(TextId);
	}
	BUTTON_SetText      (hButton,    pText);
	BUTTON_SetFont      (hButton,    &GUI_FontHZ32);
	BUTTON_SetTextAlign (hButton,    GUI_TA_BOTTOM|GUI_TA_HCENTER);
	BUTTON_SetBkColor   (hButton, 0, FRAME_BUTTON_BKCOLOR0);
	BUTTON_SetBkColor   (hButton, 1, FRAME_BUTTON_BKCOLOR1);
	BUTTON_SetBkColor   (hButton, 2, FRAME_BUTTON_BKCOLOR2);
	BUTTON_SetTextColor (hButton, 0, FRAME_BUTTON_COLOR0);
	BUTTON_SetTextColor (hButton, 1, FRAME_BUTTON_COLOR1);
	BUTTON_SetTextColor (hButton, 2, FRAME_BUTTON_COLOR0);
	
	// WIDGET_SetEffect    (hButton,    FRAME_BUTTON_EFFECT);
	
	/* ���ý������뽹������� */
	BUTTON_SetFocussable(hButton,    1);
	
	BUTTON_SetSkinClassic(hButton);
	
	BUTTON_SetBkColor(hButton,BUTTON_BI_UNPRESSED,FRAME_BKCOLOR);//�ɿ���ʾ�ı���ͼƬ
	BUTTON_SetBkColor(hButton,BUTTON_BI_PRESSED,FRAME_BKCOLOR);  //��������ʾ�ı���ͼƬ
	if(pos == 1)
	{
		BUTTON_SetBitmapEx(hButton,BUTTON_BI_UNPRESSED,pBitmap,65,50);
	}
	else if(pos == 2)
	{
		BUTTON_SetBitmapEx(hButton,BUTTON_BI_UNPRESSED,pBitmap,0,50);
	}
	else
	{
		BUTTON_SetFrameColor(hButton,    FRAME_BKCOLOR);
		BUTTON_SetBitmapEx(hButton,      BUTTON_BI_UNPRESSED,pBitmap,0,0);
	}
		
	return hButton;
}

WM_HWIN _CreateButton_Plus(WM_HWIN hParent, const char* pText, int Id, int x, int y, int w, int h, unsigned TextId,GUI_HMEM hButtonStreamBitmap,GUI_BITMAP Bitmap) 
{
	WM_HWIN        hButton;
	char *         _acBuffer;
	GUI_LOGPALETTE Palette;
	hButton = BUTTON_CreateEx(x, y, w, h, hParent, WM_CF_SHOW, 0, Id);
	
	//��ȡ��ť�ı���ͼƬ
	_acBuffer = GUI_ALLOC_h2p(hButtonStreamBitmap);
	GUI_CreateBitmapFromStream(&Bitmap, &Palette, _acBuffer);
	/* ��ȡ��ǰ������ťҪ��ʾ���ı� */
	if (TextId) 
	{
		pText = _GetLang(TextId);
	}
	BUTTON_SetText      (hButton,    pText);
	BUTTON_SetFont      (hButton,    FRAME_BUTTON_FONT);
	BUTTON_SetBkColor   (hButton, 0, FRAME_BUTTON_BKCOLOR0);
	BUTTON_SetBkColor   (hButton, 1, FRAME_BUTTON_BKCOLOR1);
	BUTTON_SetBkColor   (hButton, 2, FRAME_BUTTON_BKCOLOR2);
	BUTTON_SetTextColor (hButton, 0, FRAME_BUTTON_COLOR0);
	BUTTON_SetTextColor (hButton, 1, FRAME_BUTTON_COLOR1);
	BUTTON_SetTextColor (hButton, 2, FRAME_BUTTON_COLOR0);
	// WIDGET_SetEffect    (hButton,    FRAME_BUTTON_EFFECT);
	
	/* ���ý������뽹������� */
	BUTTON_SetFocussable(hButton,    1);
	
	BUTTON_SetSkinClassic(hButton);
	
	BUTTON_SetBkColor(hButton,BUTTON_BI_UNPRESSED,FRAME_BKCOLOR);//�ɿ���ʾ�ı���ͼƬ
	BUTTON_SetBkColor(hButton,BUTTON_BI_PRESSED,FRAME_BKCOLOR);  //��������ʾ�ı���ͼƬ

	BUTTON_SetBitmapEx(hButton,BUTTON_BI_UNPRESSED,&Bitmap,0,0);
	
	return hButton;
}

WM_HWIN _CreateText(WM_HWIN hParent, const char* pText, int Id, int x, int y, int w, int h, unsigned TextId) 
{
	WM_HWIN hText;
	hText = TEXT_CreateEx(x, y, w, h, hParent, WM_CF_SHOW, 0, Id,pText);
	
	/* ��ȡ��ǰ�����ı���Ҫ��ʾ���ı� */
	TEXT_SetText      (hText,    pText);
	TEXT_SetFont      (hText,    FRAME_BUTTON_FONT);
	WIDGET_SetEffect    (hText,  FRAME_BUTTON_EFFECT);
	
	/* ���ý������뽹������� */
	
	return hText;
}


/*
*********************************************************************************************************
*	�� �� ��: _LoadStreamBitmap
*	����˵��: ������λͼ��emWin��̬�ڴ棬�����ʹ���ˣ����Ե��ú���GUI_ALLOC_Free�ͷŴ���λͼռ�õ�
*             ��̬�ڴ档
*	��    ��: sFilename  Ҫ��ȡ���ļ���
*	�� �� ֵ: ������λͼռ�õĶ�̬�ڴ�����������λͼ�ĵ����Լ���ʹ��ʱ�ͷŴ˶�̬�ڴ档
*********************************************************************************************************
*/
GUI_HMEM  _LoadStreamBitmap(const char *sFilename) 
{
	GUI_HMEM hMem;
	uint8_t result;
	char *_acBuffer;


	/* ���ļ� */		
	result = f_open(file, sFilename, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
	if (result != FR_OK)
	{
		return 0;
	}
	
	/* ����һ���ڴ�ռ� ���ҽ������� */
    hMem = GUI_ALLOC_AllocZero(file->obj.objsize);
	
	/* �����뵽�ڴ�ľ��ת����ָ������ */
	_acBuffer = GUI_ALLOC_h2p(hMem);

     /* ��ȡ�ļ�����̬�ڴ� */
     result = f_read(file, _acBuffer, file->obj.objsize, &bw);
     if (result != FR_OK)
     {
         return 0;
     }
	 /* �ر��ļ� */
	 f_close(file);
	 
	return hMem;
}



