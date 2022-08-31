/**************************************************
	* @File Name: servic.c
	* @brief GUI界面服务功能
	* @author 王现刚 (2891854535@qq.com)
	* @Version : 1.0
	* @date 2022-08-31
	* 
***************************************************/
#include "exfuns.h"
#include "fattester.h"
#include "ff.h"
#include "malloc.h"
#include "os_control.h"
#include "servic.h"
#include "set_menu.h"

/***
 * 打开此页面,显示密码框
 * 显示键盘
 * 提示输入密码
 * 验证密码是否正确
 * 密码错误,重新输入
 * 密码正确,进入下一页面
 ***/




//密码输入界面取消按钮
#define ID_SERVIC_BACK_BUTTON    (GUI_ID_USER + 0x00)
//密码输入界面确认按钮
#define ID_SERVIC_VERIFY_BUTTON    (GUI_ID_USER + 0x01)
//密码输入界面取消按钮
#define ID_SERVIC_CANCEL_BUTTON    (GUI_ID_USER + 0x02)
#define ID_SERVIC_0_BUTTON    (GUI_ID_USER + 0x03)
#define ID_SERVIC_1_BUTTON    (GUI_ID_USER + 0x04)
#define ID_SERVIC_2_BUTTON    (GUI_ID_USER + 0x05)
#define ID_SERVIC_3_BUTTON    (GUI_ID_USER + 0x06)
#define ID_SERVIC_4_BUTTON    (GUI_ID_USER + 0x07)
#define ID_SERVIC_5_BUTTON    (GUI_ID_USER + 0x08)
#define ID_SERVIC_6_BUTTON    (GUI_ID_USER + 0x09)
#define ID_SERVIC_7_BUTTON    (GUI_ID_USER + 0x0A)
#define ID_SERVIC_8_BUTTON    (GUI_ID_USER + 0x0B)
#define ID_SERVIC_9_BUTTON    (GUI_ID_USER + 0x0C)

#define NUMPAD_BKCOLOR            GUI_LIGHTGRAY
#define NUMPAD_EFFECT             (&WIDGET_Effect_3D2L)
#define NUMPAD_BORDER_X             160
#define NUMPAD_BORDER_Y             140
#define NUMPAD_PIN                1685

#define NUMPAD_BUTTON_BKCOLOR0    0xB8B8B8              /* Unpressed */
#define NUMPAD_BUTTON_BKCOLOR1    0xE0E0E0              /* Pressed   */
#define NUMPAD_BUTTON_BKCOLOR2    0xC0C0C0              /* Disabled  */
#define NUMPAD_BUTTON_COLOR0      0x700000              /* Unpressed */
#define NUMPAD_BUTTON_COLOR1      0x700000              /* Pressed   */
#define NUMPAD_BUTTON_RED0        0x0060FF              /* Unpressed */
#define NUMPAD_BUTTON_RED1        0x0030E8              /* Pressed   */
#define NUMPAD_BUTTON_BLUE0       0x00FF00              /* Unpressed */
#define NUMPAD_BUTTON_BLUE1       0x00FF7F              /* Pressed   */
#define NUMPAD_BUTTON_GREEN0      0x00B040              /* Unpressed */
#define NUMPAD_BUTTON_GREEN1      0x00D000              /* Pressed   */
#define NUMPAD_BUTTON_FONT        (&GUI_FontHZ16)
#define NUMPAD_BUTTON_EFFECT      (&WIDGET_Effect_3D1L)
#define NUMPAD_BUTTON_WIDTH       80
#define NUMPAD_BUTTON_HEIGHT      36
#define NUMPAD_BUTTON_DIST_X      7
#define NUMPAD_BUTTON_DIST_Y      7


/**************************************************
	* 
	* @brief 变量定义 
	* 
***************************************************/
int          _PIN_Digits;             /* 当前输入的密码位置 */
char         password[6] = "123456";  /* 默认密码 */
char         in_password[6];          /* 键盘采集密码 */
char         temp_password[6];        /* 修改密码临时变量 */
int          PWD_SELECT = 0;          /* 密码输入提示索引 */
static FIL   pwd_file;
/* 将密码保存到外部FLASH,该变量保存文件路径及文件名 */
const TCHAR* pwd_pname = "1:pwd.txt";
/**************************************************
	* 
	* @brief 静态变量
	* 
***************************************************/
static const char *_pwd_hint[][1] = {
	{"旧密码"},
	{"新密码"},
	{"再输一次"}
};

//维护菜单界面
void _cbservic_menu(WM_MESSAGE* pMsg) ;
//修改密码界面
void _cbpwd_change(WM_MESSAGE* pMsg) ;
//深度校准界面
void _cbcalib_depth(WM_MESSAGE* pMsg) ;
//传感器校准界面
void _cbcalib_lvdt(WM_MESSAGE* pMsg) ;

/*
*********************************************************************************************************
*	函 数 名: _get_password
*	功能说明: 从FLASH读取密码
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/

void _get_password(void *p_arg)
{
    u8 res;
	char        string_buf[20];
	int i;
	
	char bufer[20];
	
	CPU_SR_ALLOC();
		
    (void)p_arg;
	
	
       

    //获取日志文件的大小
    res = f_open(&pwd_file,pwd_pname,FA_OPEN_EXISTING | FA_READ);
	//若文件不存在,就新建一个,并且写入默认密码
    if(res != FR_OK)
    {
		OS_CRITICAL_ENTER();	//临界区
		memset(string_buf,0,sizeof(string_buf));
		sprintf(string_buf,"%s\r\n",password);
        res = f_open(&pwd_file,pwd_pname, FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
		res = f_write(&pwd_file,string_buf, strlen(string_buf), &bw);
		if(res != FR_OK)
		{
			App_Printf("写入失败\r\n");
		}
		else
		{
			App_Printf("写入成功\r\n");
		}
		OS_CRITICAL_EXIT();//退出临界区
		f_close(&pwd_file);
    }			
    f_close(&pwd_file);
	
	res = f_open(&pwd_file,pwd_pname,FA_READ);
    //文件打开成功

    OS_CRITICAL_ENTER();	//临界区
    //读取文件到缓存
	res = f_read(&pwd_file,bufer,22,&br); //读取数据
    for(i=0;i<6;i++)
	{
		password[i] = bufer[i];	
	}
    OS_CRITICAL_EXIT();//退出临界区
    f_close(&pwd_file);
    
    
}

/*
*********************************************************************************************************
*	函 数 名: _change_password
*	功能说明: 将修改后的密码存储到flash
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/
void _change_password(void *p_arg)
{
    u8 res;
    //防止编译器报错
	
	CPU_SR_ALLOC();
		
    (void)p_arg;
	
    //获取日志文件的大小
    res = f_open(&pwd_file,pwd_pname,FA_OPEN_EXISTING | FA_READ);
	//若文件不存在,就新建一个,并且写入默认密码
    if(res == FR_NO_FILE)
    {
		OS_CRITICAL_ENTER();	//临界区
		_change_password(0);
		OS_CRITICAL_EXIT();//退出临界区
		f_close(&pwd_file);
    }			
    f_close(&pwd_file);
	
	res = f_open(&pwd_file,pwd_pname,FA_WRITE);
    //文件打开成功
    OS_CRITICAL_ENTER();	//临界区
    //将密码写入文件
	res = f_write(&pwd_file,password,strlen(password),&br); //读取数据

    OS_CRITICAL_EXIT();//退出临界区
    f_close(&pwd_file);
    
}

/*
*********************************************************************************************************
*	函 数 名: _get_pwd_hint
*	功能说明: 获取当前密码输入提示
*	形    参：
*	返 回 值: p      返回当前密码提示
*********************************************************************************************************
*/
static const char * _get_pwd_hint(void)
{
    const char *p;

    p = NULL;
	/* 获取当前应该进行的密码操作 */
    p = * _pwd_hint[PWD_SELECT];

    return p;
}


/*
*********************************************************************************************************
*	函 数 名: _CreateNumPadButton
*	功能说明: 创建小键盘所需要的按钮
*	形    参：hParent  父窗口
*             pText    按键上显示的文本
*             Id       按钮Id         
*             x        x轴坐标
*             y        y轴坐标
*             w        按钮宽
*             h        按钮高
*             Color    按钮颜色
*             PressedColor  按钮被按下后的颜色
*             Enable   使能或者禁止状态
*             TextId   文本的ID          
*	返 回 值: 无
*********************************************************************************************************
*/
static WM_HWIN _CreateNumPadButton(WM_HWIN hParent, const char* pText, int Id,
                                   int x, int y, int w, int h,
                                   GUI_COLOR Color, GUI_COLOR PressedColor, int Enable, unsigned TextId)
{
	WM_HWIN hButton;
	
	
	hButton = BUTTON_CreateEx(x, y, w, h, hParent, WM_CF_SHOW, 0, Id);
	if (Enable) 
	{
		if (TextId) 
		{
			pText = _GetLang(TextId);
		}
		
		BUTTON_SetText(hButton, pText);
	} 
	else 
	{
		WM_DisableWindow(hButton);
	}
	
	BUTTON_SetFont      (hButton,    NUMPAD_BUTTON_FONT);
	BUTTON_SetBkColor   (hButton, 0, NUMPAD_BUTTON_BKCOLOR0);
	BUTTON_SetBkColor   (hButton, 1, NUMPAD_BUTTON_BKCOLOR1);
	BUTTON_SetBkColor   (hButton, 2, NUMPAD_BUTTON_BKCOLOR2);
	BUTTON_SetTextColor (hButton, 0, Color);
	BUTTON_SetTextColor (hButton, 1, PressedColor);
	BUTTON_SetTextColor (hButton, 2, Color);
	BUTTON_SetFocussable(hButton, 1);
	WIDGET_SetEffect    (hButton, NUMPAD_BUTTON_EFFECT);
	
	return hButton;
}



/*
*********************************************************************************************************
*	函 数 名: _cbNumPad
*	功能说明: 小键盘的回调函数
*	形    参：pMsg  参数指针
*	返 回 值: 无
*********************************************************************************************************
*/
void _cbNumPad(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
	/* 创建定时器 */
    int     NCode;
	int     Id;
    int     i;
    char    string_buf[6];
    int     bx, by, bw, bh, dx, dy;
    /* 位置和大小 */
	bx = NUMPAD_BORDER_X;
	by = NUMPAD_BORDER_Y;
	bw = NUMPAD_BUTTON_WIDTH;
	bh = NUMPAD_BUTTON_HEIGHT;
	dx = NUMPAD_BUTTON_DIST_X;
	dy = NUMPAD_BUTTON_DIST_Y;

	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* 设置聚焦 */
			WM_SetFocus(hItem);
            //创建此页面所需的所有按钮
            GUI_SetFont(&GUI_FontHZ24);
			_CreateButton_P(hItem, " ",  ID_SERVIC_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
            _PIN_Digits = 0;
			_get_password(0);
            /* 为键盘窗口创建按钮 */
            _CreateNumPadButton(hItem, "1",    ID_SERVIC_1_BUTTON,      bx + 0*(bw+dx),   by + 0*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "2",    ID_SERVIC_2_BUTTON,      bx + 1*(bw+dx),   by + 0*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "3",    ID_SERVIC_3_BUTTON,      bx + 2*(bw+dx),   by + 0*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "4",    ID_SERVIC_4_BUTTON,      bx + 0*(bw+dx),   by + 1*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "5",    ID_SERVIC_5_BUTTON,      bx + 1*(bw+dx),   by + 1*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "6",    ID_SERVIC_6_BUTTON,      bx + 2*(bw+dx),   by + 1*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "7",    ID_SERVIC_7_BUTTON,      bx + 0*(bw+dx),   by + 2*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "8",    ID_SERVIC_8_BUTTON,      bx + 1*(bw+dx),   by + 2*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "9",    ID_SERVIC_9_BUTTON,      bx + 2*(bw+dx),   by + 2*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "取消", ID_SERVIC_CANCEL_BUTTON, bx + 0*(bw+dx),   by + 3*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0) ;
            _CreateNumPadButton(hItem, "0",    ID_SERVIC_0_BUTTON,      bx + 1*(bw+dx),   by + 3*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "确认", ID_SERVIC_VERIFY_BUTTON, bx + 2*(bw+dx),   by + 3*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);

			WM_CreateTimer(hItem, /* 接受信息的窗口的句柄 */
						   0, 	                     /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
						   10,                       /* 周期，此周期过后指定窗口应收到消息*/
						   0);
		case WM_PAINT:
			_PaintFrame();
			_PaintFrameSta();		
			GUI_SetFont(&GUI_FontHZ24);
			GUI_DispStringHCenterAt("维护", 230, 70 - MAIN_TITLE_HEIGHT);
            GUI_DispStringHCenterAt("密码", 70, 160 - MAIN_TITLE_HEIGHT);
			
			memset(string_buf,0,sizeof(string_buf));
            for(i = 0;i<_PIN_Digits;i++)
            {
                string_buf[i] = '*';
            }
            GUI_DispStringHCenterAt(string_buf, 70, 220 - MAIN_TITLE_HEIGHT);
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
                case ID_SERVIC_BACK_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            _DeleteFrame();   
                            _CreateFrame(&_cbsystem_menu_3);
							break;
					}
					break;
                case ID_SERVIC_1_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '1';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_2_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '2';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_3_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '3';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_4_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '4';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_5_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '5';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_6_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '6';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_7_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '7';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_8_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '8';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_9_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '9';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_0_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '0';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_CANCEL_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除一位密码*/
                            if(_PIN_Digits>=0)
                            {
                                in_password[_PIN_Digits] = '*';
                                _PIN_Digits --;
                                if(_PIN_Digits<0)
                                {
                                    _PIN_Digits = 0;
                                }
                            }
							break;
					}
					break;
                case ID_SERVIC_VERIFY_BUTTON: //确认,进行密码验证
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
                            if(strncmp(password,in_password,sizeof(in_password)) == 0)
                            {
                                /* 删除这个创建的界面 */
                                _PIN_Digits = 0;
                                for(i=0;i<6;i++)
                                {
                                    in_password[i] = '*';
                                }
                                _DeleteFrame();   
                                _CreateFrame(&_cbservic_menu);
                            }//重新输入密码
							else
							{
								_PIN_Digits = 0;
                                for(i=0;i<6;i++)
                                {
                                    in_password[i] = '*';
                                }
							}

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
*	函 数 名: _cbservic_menu
*	功能说明: 警告日志显示界面
*	形    参：pMsg  参数指针
*	返 回 值: 
*********************************************************************************************************
*/
void _cbservic_menu(WM_MESSAGE* pMsg) 
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
			_CreateButton_P(hItem, " ",  ID_SERVIC_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton(hItem, "修改密码",    ID_SERVIC_1_BUTTON,      0,   122 + MAIN_TITLE_HEIGHT, 400,  66, 0);
            _CreateButton(hItem, "位移传感器校准",    ID_SERVIC_2_BUTTON,      0,   188 + MAIN_TITLE_HEIGHT, 400,  66, 0);
            _CreateButton(hItem, "上移", ID_SERVIC_CANCEL_BUTTON, 400,   122 + MAIN_TITLE_HEIGHT,   80,  66,0) ;
            _CreateButton(hItem, "按压深度校准",    ID_SERVIC_0_BUTTON,      0,   254 + MAIN_TITLE_HEIGHT,    400,  66, 0);
            _CreateButton(hItem, "下移", ID_SERVIC_VERIFY_BUTTON, 400,   254 + MAIN_TITLE_HEIGHT, 80,  66, 0);
			WM_CreateTimer(hItem, /* 接受信息的窗口的句柄 */
						   0, 	                     /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
						   10,                       /* 周期，此周期过后指定窗口应收到消息*/
						   0);
		case WM_PAINT:
			_PaintFrame();	
			_PaintFrameSta();		
			GUI_SetFont(&GUI_FontHZ24);
		
			GUI_DispStringHCenterAt("维护", 230, 70 - MAIN_TITLE_HEIGHT);
			GUI_DispStringHCenterAt("1/1", 440, 200 + MAIN_TITLE_HEIGHT);
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
                case ID_SERVIC_BACK_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            _DeleteFrame();   
                            _CreateFrame(&_cbsystem_menu_3);
							break;
					}
					break;
				case ID_SERVIC_1_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            _DeleteFrame();   
                            _CreateFrame(&_cbpwd_change);
							break;
					}
					break;
				case ID_SERVIC_2_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            _DeleteFrame();   
                            _CreateFrame(&_cbcalib_lvdt);
							break;
					}
					break;
				case ID_SERVIC_0_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            _DeleteFrame();   
                            _CreateFrame(&_cbcalib_depth);
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
*	函 数 名: _cbpwd_change
*	功能说明: 修改密码回调函数
*	形    参：pMsg  参数指针
*	返 回 值: 
*********************************************************************************************************
*/
void _cbpwd_change(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
	/* 创建定时器 */
    int     NCode;
	int     Id;
	int     i;
    char    string_buf[6];
	int     bx, by, bw, bh, dx, dy;
    /* 位置和大小 */
	bx = NUMPAD_BORDER_X;
	by = NUMPAD_BORDER_Y;
	bw = NUMPAD_BUTTON_WIDTH;
	bh = NUMPAD_BUTTON_HEIGHT;
	dx = NUMPAD_BUTTON_DIST_X;
	dy = NUMPAD_BUTTON_DIST_Y;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* 设置聚焦 */
			WM_SetFocus(hItem);
            //创建此页面所需的所有按钮
            GUI_SetFont(&GUI_FontHZ24);
			_CreateButton_P(hItem, " ",  ID_SERVIC_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
            _PIN_Digits = 0;
            /* 为键盘窗口创建按钮 */
            _CreateNumPadButton(hItem, "1", ID_SERVIC_1_BUTTON,         bx + 0*(bw+dx),   by + 0*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "2", ID_SERVIC_2_BUTTON,         bx + 1*(bw+dx),   by + 0*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "3", ID_SERVIC_3_BUTTON,         bx + 2*(bw+dx),   by + 0*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "4", ID_SERVIC_4_BUTTON,         bx + 0*(bw+dx),   by + 1*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "5", ID_SERVIC_5_BUTTON,         bx + 1*(bw+dx),   by + 1*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "6", ID_SERVIC_6_BUTTON,         bx + 2*(bw+dx),   by + 1*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "7", ID_SERVIC_7_BUTTON,         bx + 0*(bw+dx),   by + 2*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "8", ID_SERVIC_8_BUTTON,         bx + 1*(bw+dx),   by + 2*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "9", ID_SERVIC_9_BUTTON,         bx + 2*(bw+dx),   by + 2*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "取消", ID_SERVIC_CANCEL_BUTTON, bx + 0*(bw+dx),   by + 3*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0) ;
            _CreateNumPadButton(hItem, "0", ID_SERVIC_0_BUTTON,         bx + 1*(bw+dx),   by + 3*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
            _CreateNumPadButton(hItem, "确认", ID_SERVIC_VERIFY_BUTTON, bx + 2*(bw+dx),   by + 3*(bh+dy), bw,     bh, NUMPAD_BUTTON_COLOR0,  NUMPAD_BUTTON_COLOR1,  1, 0);
			WM_CreateTimer(hItem, /* 接受信息的窗口的句柄 */
						   0, 	                     /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
						   10,                       /* 周期，此周期过后指定窗口应收到消息*/
						   0);
		case WM_PAINT:
			_PaintFrame();	
			_PaintFrameSta();		
			GUI_SetFont(&GUI_FontHZ24);
		
			GUI_DispStringHCenterAt("修改密码", 230, 70 - MAIN_TITLE_HEIGHT);
			GUI_DispStringHCenterAt(_get_pwd_hint(), 70, 160 - MAIN_TITLE_HEIGHT);
			memset(string_buf,0,sizeof(string_buf));
            for(i = 0;i<_PIN_Digits;i++)
            {
                string_buf[i] = in_password[i];
            }
            GUI_DispStringHCenterAt(string_buf, 70, 220 - MAIN_TITLE_HEIGHT);
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
                case ID_SERVIC_BACK_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            _DeleteFrame();   
                            _CreateFrame(&_cbservic_menu);
							break;
					}
					break;
				 case ID_SERVIC_1_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '1';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_2_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '2';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_3_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '3';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_4_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '4';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_5_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '5';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_6_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '6';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_7_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '7';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_8_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '8';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_9_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '9';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_0_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            if(_PIN_Digits<6)
                            {
                                in_password[_PIN_Digits] = '0';
                                _PIN_Digits ++;
                            }
                                
							break;
					}
					break;
                case ID_SERVIC_CANCEL_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除一位密码*/
                            if(_PIN_Digits>=0)
                            {
                                in_password[_PIN_Digits] = '*';
                                _PIN_Digits --;
                                if(_PIN_Digits<0)
                                {
                                    _PIN_Digits = 0;
                                }
                            }
							break;
					}
					break;
                case ID_SERVIC_VERIFY_BUTTON: //确认,进行密码验证
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							switch (PWD_SELECT)
							{
								case 0://验证管理员密码
									if(strncmp(password,in_password,sizeof(in_password)) == 0)
									{
										_PIN_Digits = 0;
										for(i=0;i<6;i++)
										{
											in_password[i] = '*';
										}
										PWD_SELECT = 1;
									}//重新输入密码
									else
									{
										_PIN_Digits = 0;
										for(i=0;i<6;i++)
										{
											in_password[i] = '*';
										}
									}
									break;
								//第一次重置密码
								case 1:
									if(_PIN_Digits == 6)
									{
										for(i=0;i<6;i++)
										{
											temp_password[i] = in_password[i];
										}
										PWD_SELECT = 2;
										_PIN_Digits = 0;
										memset(in_password,0,sizeof(in_password));
									}
									else
									{
										App_Printf("请输入六位密码");
									}
									break;
								//验证两次密码是否一样
								case 2:
									if(_PIN_Digits == 6)
									{
										//两次输入一样
										if(strncmp(temp_password,in_password,sizeof(in_password)) == 0)
										{
											for(i=0;i<6;i++)
											{
												password[i] = in_password[i];
											}
											_change_password(0);
											PWD_SELECT = 0;
											_PIN_Digits = 0;
											memset(in_password,0,sizeof(in_password));
											_DeleteFrame();   
											_PaintFrameSta();
											_CreateFrame(&_cbservic_menu);

										}
										else
										{
											App_Printf("请输入相同密码");
										}
									}
									else
									{
										App_Printf("请输入六位密码");
									}
									break;
								default:
									break;
							}
                            

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
*	函 数 名: _cbcalib_lvdt
*	功能说明: 位移传感器校准界面
*	形    参：pMsg  参数指针
*	返 回 值: 
*********************************************************************************************************
*/
void _cbcalib_lvdt(WM_MESSAGE* pMsg) 
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
			_CreateButton_P(hItem, " ",  ID_SERVIC_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
            _CreateButton(hItem, "提示:按\"开始\"启动校准", ID_SERVIC_CANCEL_BUTTON, 0,   188 + MAIN_TITLE_HEIGHT,   480,  66,0) ;
            _CreateButton(hItem, "开始", ID_SERVIC_VERIFY_BUTTON, 0,   254 + MAIN_TITLE_HEIGHT, 480,  66, 0);
			WM_CreateTimer(hItem, /* 接受信息的窗口的句柄 */
						   0, 	                     /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
						   10,                       /* 周期，此周期过后指定窗口应收到消息*/
						   0);
		case WM_PAINT:
			_PaintFrame();
			_PaintFrameSta();		
			GUI_SetFont(&GUI_FontHZ24);
		
			GUI_DispStringHCenterAt("位移传感器校准", 230, 70 - MAIN_TITLE_HEIGHT);
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
                case ID_SERVIC_BACK_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            _DeleteFrame();   
                            _CreateFrame(&_cbservic_menu);
							break;
					}
					break;
				case ID_SERVIC_1_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            _DeleteFrame();   
                            _CreateFrame(&_cbpwd_change);
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
*	函 数 名: _cbcalib_depth
*	功能说明: 按压深度校准界面
*	形    参：pMsg  参数指针
*	返 回 值: 
*********************************************************************************************************
*/
void _cbcalib_depth(WM_MESSAGE* pMsg) 
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
			_CreateButton_P(hItem, " ",  ID_SERVIC_BACK_BUTTON, 40,  60 + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			_CreateButton(hItem, "修改密码",    ID_SERVIC_1_BUTTON,      0,   122 + MAIN_TITLE_HEIGHT, 400,  66, 0);
            _CreateButton(hItem, "位移传感器校准",    ID_SERVIC_2_BUTTON,      0,   188 + MAIN_TITLE_HEIGHT, 400,  66, 0);
            _CreateButton(hItem, "取消", ID_SERVIC_CANCEL_BUTTON, 400,   122 + MAIN_TITLE_HEIGHT,   80,  66,0) ;
            _CreateButton(hItem, "按压深度校准",    ID_SERVIC_0_BUTTON,      0,   254 + MAIN_TITLE_HEIGHT,    400,  66, 0);
            _CreateButton(hItem, "确认", ID_SERVIC_VERIFY_BUTTON, 400,   254 + MAIN_TITLE_HEIGHT, 80,  66, 0);
			WM_CreateTimer(hItem, /* 接受信息的窗口的句柄 */
						   0, 	                     /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
						   10,                       /* 周期，此周期过后指定窗口应收到消息*/
						   0);
		case WM_PAINT:
			_PaintFrame();	
			_PaintFrameSta();
			GUI_SetFont(&GUI_FontHZ24);
		
			GUI_DispStringHCenterAt("按压深度校准", 230, 70 - MAIN_TITLE_HEIGHT);
			GUI_DispStringHCenterAt("1/1", 440, 200 + MAIN_TITLE_HEIGHT);
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
                case ID_SERVIC_BACK_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            _DeleteFrame();   
                            _CreateFrame(&_cbservic_menu);
							break;
					}
					break;
				case ID_SERVIC_1_BUTTON: //设置按钮的通知代码,打开设置界面
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
                            _DeleteFrame();   
                            _CreateFrame(&_cbpwd_change);
							break;
					}
					break;
			}
			break;
		default:
		WM_DefaultProc(pMsg);
	}
}



