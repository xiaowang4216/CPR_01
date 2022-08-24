#ifndef _GUI_PUBLIC_H_
#define _GUI_PUBLIC_H_

#include "lcd.h"
#include "DIALOG.h"
#include "EmWinHZFont.h"
#include "includes.h"
#include "led.h"
#include "ff.h"
#include "fattester.h"
#include "exfuns.h"
#include "os_control.h"
#include "icon.h"
#include "rtc.h"
#include "log.h"

extern GUI_CONST_STORAGE GUI_BITMAP bmLogo_armflySmall;

#define MAIN_BKCOLOR              GUI_BLUE_95
#define MAIN_TEXTCOLOR            0x000000
#define MAIN_FONT                 (GUI_FONT_32B_ASCII)
#define MAIN_BORDER               0
#define MAIN_TITLE_HEIGHT         5
#define MAIN_LOGO_BITMAP          (&bmLogo_armflySmall)
#define MAIN_LOGO_OFFSET_X        0
#define MAIN_LOGO_OFFSET_Y        0

#define FRAME_BKCOLOR             GUI_BLUE_96
#define FRAME_TEXTCOLOR           0x000000
#define FRAME_FONT                (&GUI_FontHZ16)
#define FRAME_EFFECT              (&WIDGET_Effect_3D2L)
#define FRAME_BORDER              FRAME_EFFECT->EffectSize
//#define FRAME_WIDTH               (LCD_GetXSize() - (FRAME_BORDER * 2) - (MAIN_BORDER * 2))
//#define FRAME_HEIGHT              (LCD_GetYSize() - (FRAME_BORDER * 2) - (MAIN_BORDER + MAIN_TITLE_HEIGHT))
#define FRAME_WIDTH               480
#define FRAME_HEIGHT              320


#define FRAME_BUTTON_BKCOLOR0     0xB8B8B8              /* Unpressed */
#define FRAME_BUTTON_BKCOLOR1     0xE0E0E0              /* Pressed   */
#define FRAME_BUTTON_BKCOLOR2     0xC0C0C0              /* Disabled  */
#define FRAME_BUTTON_COLOR0       0x000000              /* Unpressed */
#define FRAME_BUTTON_COLOR1       0x000000              /* Pressed   */
#define FRAME_BUTTON_FONT         (&GUI_FontHZ16)
#define FRAME_BUTTON_EFFECT       (&WIDGET_Effect_3D2L)

#define LOGO_FRAME_OFFSET_Y       5
#define LOGO_FRAME_SIZE_X         116
#define LOGO_FRAME_SIZE_Y         92
#define LOGO_FRAME_DIST_X         4
#define LOGO_FRAME_BKCOLOR        0xFFFFFF
#define LOGO_FRAME_EFFECT         (&WIDGET_Effect_3D2L)


//初始界面按钮
//增加深度按钮
#define ID_DEPTH_INCEA_BUTTON  (GUI_ID_USER + 0x00)
//减少深度按钮	
#define ID_DEPTH_REDUC_BUTTON  (GUI_ID_USER + 0x01)
//模式左变换按钮
#define ID_MODE_LCHAGE_BUTTON  (GUI_ID_USER + 0x02)
//模式右变换按钮
#define ID_MODE_RCHAGE_BUTTON  (GUI_ID_USER + 0x03)
//暂停/继续按钮
#define ID_PAUSE_RESUME_BUTTON (GUI_ID_USER + 0x04)
//停止按钮
#define ID_STOP_BUTTON         (GUI_ID_USER + 0x05)
//打开设置按钮
#define ID_SEETING_BUTTON      (GUI_ID_USER + 0x06)
//返回上一级按钮
#define ID_BACK_BUTTON         (GUI_ID_USER + 0x11)

#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x07)
#define ID_FRAMEWIN_1 (GUI_ID_USER + 0x08)

#define ID_TEXT_0 (GUI_ID_USER + 0x09)
#define ID_TEXT_1 (GUI_ID_USER + 0x10)

typedef struct warn_msg
{
    /* 报警内容 */
    char            * w_msg;
    /* 报警等级 */
    int               grade;
    /* 新消息 */
    int               new_mag;
}warnmsg;



//停止按钮背景图片
extern  GUI_HMEM hStopButtonStreamBitmap;
extern  GUI_BITMAP StopBitmap;
//暂停按钮背景图片
extern  GUI_HMEM hPauseButtonStreamBitmap;
extern  GUI_BITMAP PauseBitmap;
//继续按钮背景图片
extern  GUI_HMEM hConButtonStreamBitmap;
extern  GUI_BITMAP ConBitmap;
//设置小按钮背景图片
extern  GUI_HMEM hSetsButtonStreamBitmap;
extern  GUI_BITMAP SetsBitmap;


extern  WM_HWIN    _hLastFrame;
extern  WM_HWIN    _hWarnFrame;
extern  WM_HWIN    _hNumPad;

extern  unsigned   _PIN_Value;
extern  int   _PIN_Digits;

extern  unsigned   _Language;
extern  WM_HWIN    _hTitle;

extern struct  warn_msg m_ifo;



const char * _GetLang(unsigned Index) ;
void _DrawDownRectEx(const WIDGET_EFFECT* pEffect, const GUI_RECT* pRect) ;
void _DrawDownRect(const WIDGET_EFFECT* pEffect, int x0, int y0, int x1, int y1) ;
void _DrawUpRectEx(const WIDGET_EFFECT* pEffect, const GUI_RECT* pRect) ;
void _PaintFrame(void) ;
WM_HWIN _CreateFrame(WM_CALLBACK* cb) ;
void _DeleteFrame(void) ;
WM_HWIN _CreateButton(WM_HWIN hParent, const char* pText, int Id, int x, int y, int w, int h, unsigned TextId) ;
WM_HWIN _CreateButton_P(WM_HWIN hParent, const char* pText, int Id, int x, int y, int w, int h, unsigned TextId,const GUI_BITMAP *pBitmap,int pos) ;
WM_HWIN _CreateButton_Plus(WM_HWIN hParent, const char* pText, int Id, int x, int y, int w, int h, unsigned TextId,GUI_HMEM hButtonStreamBitmap,GUI_BITMAP Bitmap) ;
WM_HWIN _CreateText(WM_HWIN hParent, const char* pText, int Id, int x, int y, int w, int h, unsigned TextId) ;
void _PaintFrameSta(void) ;
void Caculate_RTC(WM_MESSAGE * pMsg);
void InitDialogRTC(WM_MESSAGE * pMsg);
void Clock_Update(WM_MESSAGE * pMsg) ;
GUI_HMEM  _LoadStreamBitmap(const char *sFilename) ;

uint8_t change_warn(char * contents,int grade_value);


#endif


