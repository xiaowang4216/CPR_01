#include "gui_public.h"
#include "rtc.h"
#include "timer.h"
#include "adc.h"

/* 最新窗口句柄 */
WM_HWIN    _hLastFrame;
/* 报警窗口句柄 */
WM_HWIN    _hWarnFrame;


unsigned   _Language;
WM_HWIN    _hTitle;

//停止按钮背景图片
GUI_HMEM hStopButtonStreamBitmap;
GUI_BITMAP StopBitmap;
//暂停按钮背景图片
GUI_HMEM hPauseButtonStreamBitmap;
GUI_BITMAP PauseBitmap;
//继续按钮背景图片
GUI_HMEM hConButtonStreamBitmap;
GUI_BITMAP ConBitmap;
//设置小按钮背景图片
GUI_HMEM hSetsButtonStreamBitmap;
GUI_BITMAP SetsBitmap;

//时间结构体
extern RTC_TimeTypeDef  RTC_TimeStructure;
extern RTC_DateTypeDef  RTC_DateStructure;

//logo
extern GUI_CONST_STORAGE GUI_BITMAP bmLogo_armflySmall;
/*
*********************************************************************************************************
*                                       静态数据
*********************************************************************************************************
*/
const char * _aLang[][2] = {
  { "取消",
    "Cancel"
  },
  { "更正",
    "Correction"
  },
  { "确认",
    "Confirm"
  },
  { "请输入密码：",
    "Please enter your PIN:"
  },
  { "请选择金额",
    "Please select amount"
  },
  { "密码错误！至少是4位",
    "Wrong PIN. At least 4 digits"
  },
  { "加",
    "jia"
  },
  { "减",
    "Please enter amount"
  },
  { "请输入金额",
    "jie"
  },
  { "左",
    "Please enter amount"
  },
  { "右",
    "Please enter amount"
  },
  { "暂停",
    "pause"
  },
  { "设置",
    "set"
  },
  { "停止",
    "stop"
  },
  { "返回",
    "back"
  },

};


/*
*********************************************************************************************************
*	函 数 名: _GetLang
*	功能说明: 获取要显示字符串的地址
*	形    参：Index  要显示字符串的序号
*	返 回 值: p      返回要显示字符串地址
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
*	函 数 名: _DrawDownRectEx
*	功能说明: 显示凹陷的矩形框
*	形    参：pEffect  控件显示效果
*             pRect    GUI_RECT类型变量地址
*	返 回 值: 无
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
*	函 数 名: _DrawDownRect
*	功能说明: 显示凹陷的矩形框
*	形    参：pEffect  控件显示效果
*             x0       起始x轴坐标
*             y0       起始y轴坐标
*             x1       结束x轴坐标
*             y1       结束y轴坐标
*	返 回 值: 无
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
*	函 数 名: _DrawUpRectEx
*	功能说明: 显示凸起的矩形框
*	形    参：pEffect  控件显示效果
*             pRect    GUI_RECT类型变量地址
*	返 回 值: 无
*********************************************************************************************************
*/
void _DrawUpRectEx(const WIDGET_EFFECT* pEffect, const GUI_RECT* pRect) 
{
	WM_LOCK();
	pEffect->pfDrawUpRect(pRect);
	WM_UNLOCK();
}




/* 报警弹窗显示高度 */
#define WARN_HEIGHT  40
/* 报警弹窗显示宽度 */
#define WAEN_WIDTH 200
/* 默认显示时长 */
#define DEFAULT_TIME 10 
/* 弹窗创建位置x */
#define WARN_X 140
/* 弹窗创建位置y */
#define WARN_Y 10
/* 创建弹窗信息结构体 */
struct  warn_msg m_ifo ={NULL,0,0};

/**
 * @brief  弹窗报警
 * @param  contents: 需要报警的内容,不宜太长,最多显示12个汉字
 * @param  grade_value: 报警的等级,分三级,1,2,3
 * @return uint8_t: 0成功,1传参有误
 */
uint8_t change_warn(char * contents,int grade_value)
{
	/* 报警内容赋值 */
	m_ifo.grade = grade_value;
	/* 报警等级赋值 */
	m_ifo.w_msg = contents;
	/* 新消息 */
	m_ifo.new_mag = 1;
	/* 如果传参有问题 */
	if(m_ifo.grade > 3 ||m_ifo.w_msg == NULL)
	{
		/* 修复报警,不显示 */
		m_ifo.grade = 0;
		return 1;
	}		
	return 0;
}

/**
 * @brief 创建报警提示框
 */
static show_warn(void)
{
	/* 显示时长 */
	static int count = DEFAULT_TIME;
	//绘制报警提示				
	if(m_ifo.grade && count)
	{
		GUI_SetFont(&GUI_FontHZ16);
		GUI_SetColor(MAIN_TEXTCOLOR);
		/* 根据报警等级显示不同背景颜色 */
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
		/* 显示报警背景框 */
		GUI_ClearRect(WARN_X,WARN_Y,WAEN_WIDTH + WARN_X,WARN_HEIGHT + WARN_Y);
		/* 显示报警内容 */
		GUI_DispStringHCenterAt(m_ifo.w_msg, 240,20);
	}	
	/* 新的消息到了 */
	if(m_ifo.new_mag == 1)
	{
		count = DEFAULT_TIME;
		m_ifo.new_mag  = 0;
	}
	/* 显示时间到了 */
	else if(count == 0)
	{
		count = DEFAULT_TIME;
		m_ifo.grade = 0;
		m_ifo.w_msg = NULL;
	}
}

/**
 * @brief 绘制当前电池电量,当电量低于某个值的时候
 * 弹窗显示电池电量低
 * @return uint8_t: 0运行正常
 */
uint8_t display_battery(void)
{
	int        bat_vol;
	int        i,temp;
	static int flag = 1;
	//获取当前电压
	bat_vol = Get_Adc_Average(1,ADC_Channel_5,20);
	bat_vol=(float)bat_vol*(3.3/4096);          //获取计算后的带小数的实际电压值，比如3.1111

	/* 绘制电量显示外框 */
	GUI_SetColor(GUI_WHITE);
	GUI_DrawRect(383,18,432,42);
	/* 电池电量低 */
	if(bat_vol<=1)
	{
		GUI_SetColor(GUI_RED);
		/* 只剩一点电量 */
		GUI_FillRect(385,20,390,40);
		/* 只提示一次电池电量低 */
		if(flag)
		{
			change_warn("电池电量不足",2);	
			/* 写入电量低日志 */
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
		/* 至少一个电量 */
		temp = 15 * i;	
		GUI_FillRect(400 +temp -15,20,400+temp,40);		
	}
	/* 回复默认字体格式 */
	GUI_SetBkColor(MAIN_BKCOLOR);
	GUI_SetColor(MAIN_TEXTCOLOR);
	GUI_SetFont(MAIN_FONT);

	return 0;
}
/*
*********************************************************************************************************
*	函 数 名: _PaintFrameSta
*	功能说明: 框架窗口状态栏的重绘函数
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void _PaintFrameSta(void) 
{			
	//获取当前时间
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN,&RTC_DateStructure);
	
	GUI_SetBkColor(MAIN_BKCOLOR);
	GUI_SetColor(MAIN_TEXTCOLOR);
	GUI_SetFont(MAIN_FONT);
	GUI_ClearRect(0,0,480,60);
	/* 绘制蓝牙图标 */
	GUI_DrawBitmap(&bmblue_tooth, 340, 10);
	GUI_DrawBitmap(&bmwarm, 80, 10);
	//显示时间
	GUI_DispStringHCenterAt(":", 44, 10 + MAIN_TITLE_HEIGHT);
	GUI_DispDecAt(RTC_TimeStructure.RTC_Hours,10,10 + MAIN_TITLE_HEIGHT,2);
	GUI_DispDecAt(RTC_TimeStructure.RTC_Minutes,50,10 + MAIN_TITLE_HEIGHT,2);
	//显示电池电量
	display_battery();
	/* 显示报警弹窗 */
	show_warn();
}


/*
*********************************************************************************************************
*	函 数 名: _PaintFrame
*	功能说明: 框架窗口的重绘函数
*	形    参：无
*	返 回 值: 无
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
*	函 数 名: _CreateFrame
*	功能说明: 创建框架窗口
*	形    参：cb  回调函数地址
*	返 回 值: 无
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
*	函 数 名: _DeleteFrame
*	功能说明: 删除创建的框架窗口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void _DeleteFrame(void) 
{
	WM_DeleteWindow(_hLastFrame);
	_hLastFrame = 0;
}

/*
*********************************************************************************************************
*	函 数 名: _CreateButton
*	功能说明: 创建按钮
*	形    参：hParent  父窗口
*             pText    按键上显示的文本
*             Id       按钮Id         
*             x        x轴坐标
*             y        y轴坐标
*             w        按钮宽
*             h        按钮高
*             TextId   文本的ID     
*	返 回 值: 无
*********************************************************************************************************
*/
WM_HWIN _CreateButton(WM_HWIN hParent, const char* pText, int Id, int x, int y, int w, int h, unsigned TextId) 
{
	WM_HWIN hButton;
	hButton = BUTTON_CreateEx(x, y, w, h, hParent, WM_CF_SHOW, 0, Id);
	
	/* 获取当前创建按钮要显示的文本 */
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
	
	/* 设置接收输入焦点的能力 */
	BUTTON_SetFocussable(hButton,    1);
	
	BUTTON_SetBkColor(hButton,BUTTON_BI_UNPRESSED,FRAME_BKCOLOR);
	return hButton;
}

WM_HWIN _CreateButton_P(WM_HWIN hParent, const char* pText, int Id, int x, int y, int w, int h, unsigned TextId,const GUI_BITMAP *pBitmap,int pos) 
{
	WM_HWIN hButton;
	hButton = BUTTON_CreateEx(x, y, w, h, hParent, WM_CF_SHOW, 0, Id);
	
	/* 获取当前创建按钮要显示的文本 */
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
	
	/* 设置接收输入焦点的能力 */
	BUTTON_SetFocussable(hButton,    1);
	
	BUTTON_SetSkinClassic(hButton);
	
	BUTTON_SetBkColor(hButton,BUTTON_BI_UNPRESSED,FRAME_BKCOLOR);//松开显示的背景图片
	BUTTON_SetBkColor(hButton,BUTTON_BI_PRESSED,FRAME_BKCOLOR);  //被按下显示的背景图片
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
	
	//获取按钮的背景图片
	_acBuffer = GUI_ALLOC_h2p(hButtonStreamBitmap);
	GUI_CreateBitmapFromStream(&Bitmap, &Palette, _acBuffer);
	/* 获取当前创建按钮要显示的文本 */
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
	
	/* 设置接收输入焦点的能力 */
	BUTTON_SetFocussable(hButton,    1);
	
	BUTTON_SetSkinClassic(hButton);
	
	BUTTON_SetBkColor(hButton,BUTTON_BI_UNPRESSED,FRAME_BKCOLOR);//松开显示的背景图片
	BUTTON_SetBkColor(hButton,BUTTON_BI_PRESSED,FRAME_BKCOLOR);  //被按下显示的背景图片

	BUTTON_SetBitmapEx(hButton,BUTTON_BI_UNPRESSED,&Bitmap,0,0);
	
	return hButton;
}

WM_HWIN _CreateText(WM_HWIN hParent, const char* pText, int Id, int x, int y, int w, int h, unsigned TextId) 
{
	WM_HWIN hText;
	hText = TEXT_CreateEx(x, y, w, h, hParent, WM_CF_SHOW, 0, Id,pText);
	
	/* 获取当前创建文本框要显示的文本 */
	TEXT_SetText      (hText,    pText);
	TEXT_SetFont      (hText,    FRAME_BUTTON_FONT);
	WIDGET_SetEffect    (hText,  FRAME_BUTTON_EFFECT);
	
	/* 设置接收输入焦点的能力 */
	
	return hText;
}


/*
*********************************************************************************************************
*	函 数 名: _LoadStreamBitmap
*	功能说明: 加载流位图到emWin动态内存，如果不使用了，可以调用函数GUI_ALLOC_Free释放此流位图占用的
*             动态内存。
*	形    参: sFilename  要读取的文件名
*	返 回 值: 返回流位图占用的动态内存句柄，方便流位图的调用以及不使用时释放此动态内存。
*********************************************************************************************************
*/
GUI_HMEM  _LoadStreamBitmap(const char *sFilename) 
{
	GUI_HMEM hMem;
	uint8_t result;
	char *_acBuffer;


	/* 打开文件 */		
	result = f_open(file, sFilename, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
	if (result != FR_OK)
	{
		return 0;
	}
	
	/* 申请一块内存空间 并且将其清零 */
    hMem = GUI_ALLOC_AllocZero(file->obj.objsize);
	
	/* 将申请到内存的句柄转换成指针类型 */
	_acBuffer = GUI_ALLOC_h2p(hMem);

     /* 读取文件到动态内存 */
     result = f_read(file, _acBuffer, file->obj.objsize, &bw);
     if (result != FR_OK)
     {
         return 0;
     }
	 /* 关闭文件 */
	 f_close(file);
	 
	return hMem;
}



