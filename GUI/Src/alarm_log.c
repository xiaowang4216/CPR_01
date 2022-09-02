/**************************************************
	* @File Name: alarm_log.c
	* @brief 报警日志的读取和显示
	* @author 王现刚 (2891854535@qq.com)
	* @Version : 1.0
	* @date 2022-08-31
	*V1.1 修改日志从最新的一条开始显示
	* 
***************************************************/
#include "alarm_log.h"
#include "set_menu.H"
#include "string.h"

/***
 * 打开日志界面
 * 系统读取SD卡关于日志的文件
 * 读取到的信息通过分割打印在系统GUI界面
 * 如果条目大于多少条 :待定
 * 就只显示最新的多少条 :待定
 * 可通过滑条滑动查看
 ***/


/**************************************************
	* 
	* @brief 按钮ID宏定义
	* 
***************************************************/
//返回按钮
#define ID_LOG_BACK_BUTTON    (GUI_ID_USER + 0x00)
//上移按钮
#define ID_LOG_ADD_BUTTON     (GUI_ID_USER + 0x01)
//下移按钮
#define ID_LOG_DEC_BUTTON     (GUI_ID_USER + 0x02)

/**************************************************
	* 
	* @brief 变量定义
	* 
***************************************************/
extern const TCHAR* log_pname;  /* 用于获取日志文件存储的路径 */
static FIL log_file;	        /* SD卡日志文件 */
char * log_buf[5];              /* 日志文件读取缓冲 */
int total_log = 0;              /* 总的日志文件数目 */
struct log_msg log_ifo[50];     /* 日志文件存储结构体 */
#define LOG_MAX 200             /* 先显示50条日志文件(200/4 = 50) */

/*
*********************************************************************************************************
*	函 数 名: split_log
*	功能说明: 将从SD卡读到的数据进行分割,将分割好的数据存储到指针数组里面
*	形    参：buf 要分割的数据
*	返 回 值: 直接从全局变量里面读取数据
*********************************************************************************************************
*/
void split_log_old(char * buf)
{
    char *p;

    int i = 0,j = 0;		

    p = NULL;	
	/* 以 @ 富豪开始对字符串进行分割 */
    p = strtok(buf,"@");

    while(p)
    {
		/* 如果分割到换行符号,就舍弃,继续下一轮分割 */
		if(strncmp(p,"\r\n",sizeof("\r\n")) == 0)
		{
			p = strtok(NULL,"@");
		}
		/* 存储当前分割日志 */
		else
		{
			log_buf[i] = p;

			++i;			
		
			p = strtok(NULL,"@");
			if((i)%3 ==0)
			{
//				+log_ifo->total_log*3
				log_ifo[j].time_ifo  = log_buf[0];
				log_ifo[j].warn_ifo   = log_buf[1];
				log_ifo[j].warn_grade = (uint8_t)*log_buf[2];
				j ++;
				log_ifo->total_log = j;
				log_ifo[j -1].total_log = j;
				i = 0;
			}
		}
       
    }
	/* 舍弃最后一次分割得到的乱码 */
	// log_buf[i] = NULL;
	/* 获取总的日志条数 */
	// total_log = (i)/2;
	//测试分割是否正常
	//i - 1:去掉最后一行乱码
    // for(j = 0;j<i - 1;j++)
    // {
	// 	App_Printf("%s",log_buf[j]);
    // }
}


/*
*********************************************************************************************************
*	函 数 名: split_log
*	功能说明: 将从SD卡读到的数据进行分割,将分割好的数据存储到指针数组里面
*	形    参：buf 要分割的数据
*	返 回 值: 直接从全局变量里面读取数据
*********************************************************************************************************
*/
void split_log(char * buf)
{
    char *p;

    int i = 0,j = 0;		

    p = NULL;	
	/* 以 @ 富豪开始对字符串进行分割 */
    p = strtok(buf,"\n");

    while(p)
    {
		/* 如果分割到换行符号,就舍弃,继续下一轮分割 */
		if(strncmp(p,"\r",sizeof("\r")) == 0)
		{
			p = strtok(NULL,"\n");
		}
		/* 存储当前分割日志 */
		else
		{
			log_buf[i] = p;

			++i;			
		
			p = strtok(NULL,"\n");
			if((i)%3 ==0)
			{
//				+log_ifo->total_log*3
				log_ifo[j].time_ifo  = log_buf[0];
				log_ifo[j].warn_ifo   = log_buf[1];
				log_ifo[j].warn_grade = (uint8_t)*log_buf[2];
				j ++;
				log_ifo->total_log = j;
				log_ifo[j -1].total_log = j;
				i = 0;
			}
		}
       
    }
}
/**
 * @brief 调用此函数,获取日志文件的总行数
 * @return int: 日志文件的总行数
 */
static int get_log_line_num(void * p_arg)
{
	u8          res;
	char      * buff;
	int         log_line_num = 0;
	CPU_SR_ALLOC();
	/* 防止编译器报错 */
	(void)p_arg;
	/* 打开日志文件 */
	res = f_open(&log_file,log_pname,FA_READ);
	/* 打开文件失败,利用窗口打印错误位置 */
	if(res != FR_OK)
	{
		App_Printf("在获取日志文件总行数的时候文件打开失败\r\n");
		return res;
	}
	/* 日志文件打开成功 */
	OS_CRITICAL_ENTER();	                //临界区
	for(;;)
	{
		res = f_read(&log_file,buff,1,&br); //读取数据
		if(*buff == '\n')log_line_num++;    //读取到换行符号,总行数加一
		/* 完成读取文件 */
		if(br == 0)break;	
	}
    OS_CRITICAL_EXIT();                     //退出临界区
	/* 关闭文件 */
    f_close(&log_file);
	/* 返回总的日志文件总数 */
	return log_line_num;
}

/*
*********************************************************************************************************
*	函 数 名: _get_logs
*	功能说明: 从SD卡读取数据
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/
void _get_logs(void *p_arg)
{
    u8          res;
    u16         filesize;
    char        *buf;
	
	CPU_SR_ALLOC();
	/* 防止编译器报错 */
    (void)p_arg;  
    /* 获取日志文件的大小 */
    res = f_open(&log_file,log_pname,FA_OPEN_EXISTING | FA_READ);
    filesize = f_size(&log_file);
    f_close(&log_file);
	/* 再次打开文件 */
    res = f_open(&log_file,log_pname,FA_READ);
	/* 打开文件失败 */
    if(res != FR_OK)
    {
        App_Printf("日志读取错误\r\n");
        return;
    }
    /* 文件打开成功 */
    buf = mymalloc(SRAMEX,filesize);//申请内存
    OS_CRITICAL_ENTER();	//临界区
    //读取文件到缓存
    for(;;)
	{
		res = f_read(&log_file,buf,4096,&br); //读取数据
		/* 完成读取文件 */
		if(br == 0)break;	
	}
    OS_CRITICAL_EXIT();//退出临界区
    f_close(&log_file);
	/* 开始分割日志文件 */
    split_log(buf);
    /* 释放内存 */
    myfree(SRAMEX,buf);	
    
}

/*
*********************************************************************************************************
*	函 数 名: _get_logs
*	功能说明: 从SD卡读取数据
*	形    参：
*	返 回 值: 
*********************************************************************************************************
*/
void _get_logs1(void *p_arg)
{
    u8          res;
    u16         filesize;
	int         i;
	int         log_start;
	int         log_num = LOG_MAX;
	int         temp_locate = 0;
	/* 先读取50条 */
    char        *buf;
	
	CPU_SR_ALLOC();
	/* 防止编译器报错 */
    (void)p_arg;  
	log_start = get_log_line_num(0);                               //获取文件总行数
	/* 行数超过LOG_MAX条,前面舍弃掉 */
	if(log_start > log_num)
	{
		log_start = log_start - (log_num + 1);                     //减去要显示的总行数,得到开始读取的位置
	}
	else
	{
		log_start = 0;
	}
    /* 获取日志文件的大小 */
    res = f_open(&log_file,log_pname,FA_OPEN_EXISTING | FA_READ);
    filesize = f_size(&log_file);
    f_close(&log_file);
	/* 获取日志条数 */
	
	/* 再次打开文件 */
    res = f_open(&log_file,log_pname,FA_READ);
	/* 打开文件失败 */
    if(res != FR_OK)
    {
        App_Printf("日志读取错误\r\n");
        return;
    }
    /* 文件打开成功 */
	/* 跳过前面多余的部分 */
	OS_CRITICAL_ENTER();	                                       //临界区
	for(i=0;i<log_start;)
	{
		res = f_read(&log_file,buf,1,&br);                         //读取数据		
		if(*buf == '\n')                                           //读取到了一行数据
		{
			temp_locate = f_tell(&log_file);                       //获取当前读指针
			f_lseek(&log_file,temp_locate);                        //移动文件指针位置
			i++;
		}
	}
	OS_CRITICAL_EXIT();//退出临界区
    buf = mymalloc(SRAMEX,filesize - temp_locate);//申请内存
    OS_CRITICAL_ENTER();	//临界区
    //读取文件到缓存
    for(;;)
	{
		res = f_read(&log_file,buf,4096,&br);                      //读取数据
		/* 完成读取文件 */
		if(br == 0)break;	
	}
    OS_CRITICAL_EXIT();                                            //退出临界区
    f_close(&log_file);
	/* 开始分割日志文件 */
    split_log(buf);
    /* 释放内存 */
    myfree(SRAMEX,buf);	
    
}

/**
 * @brief 绘制报警日志表格的背景颜色
 * @param  i: 当前选中的日志的索引
 * @param  j: 当前要绘制的框框
 */
static void draw_alarm_log_back(int i,int j)
{
	uint8_t mode = log_ifo[i].warn_grade - '0';
	/* 选择第几个背景框 */
	switch (j)
	{
		case 0:
			/* 根据报警等级选择背景颜色 */
			switch (mode)
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
			GUI_ClearRect(0,112,408,168);
			break;
		case 1:
			/* 根据报警等级选择背景颜色 */
			switch (mode)
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
			GUI_ClearRect(0,172,408,228);
			break;
		case 2:
			/* 根据报警等级选择背景颜色 */
			switch (mode)
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
			GUI_ClearRect(0,232,408,288);
			break;
		default:
			App_Printf("绘制报警日志背景参数有误,请检查\r\n");
			break;
	}
}


/**
 * @brief 显示报警日志
 * @param  i: 当前所选日志位置
 * @return uint8_t: 0:显示成功
 */
uint8_t show_log(int i)
{
	int     j = 0;
	char    string_buf[10];

	GUI_SetFont(&GUI_FontHZ24);
	GUI_SetColor(GUI_BLACK);
	
	draw_alarm_log_back(i,j);                                                     //绘制当前所选日志的报警等级背景颜色
	GUI_DispStringHCenterAt(log_ifo[i].time_ifo, 230, 120 - MAIN_TITLE_HEIGHT);
	GUI_DispStringHCenterAt(log_ifo[i].warn_ifo, 230, 150 - MAIN_TITLE_HEIGHT);
	if(i >= 1)
	{
		draw_alarm_log_back(i-1,j+1);
		GUI_DispStringHCenterAt(log_ifo[i -1].time_ifo, 230, 180 - MAIN_TITLE_HEIGHT);
		GUI_DispStringHCenterAt(log_ifo[i -1].warn_ifo, 230, 210 - MAIN_TITLE_HEIGHT);
	}
	
	if(i >=2)
	{
		draw_alarm_log_back(i-2,j+2);
		GUI_DispStringHCenterAt(log_ifo[i -2].time_ifo, 230, 240 - MAIN_TITLE_HEIGHT);
		GUI_DispStringHCenterAt(log_ifo[i -2].warn_ifo, 230, 270 - MAIN_TITLE_HEIGHT);
	}
	
	/* 显示当前日志文件时第几条 */
	sprintf(string_buf,"%d%s%d",log_ifo->total_log-i,"/",log_ifo->total_log);
	GUI_DispStringHCenterAt("报警日志", 230, 70 - MAIN_TITLE_HEIGHT);
	if(log_ifo[i].total_log) GUI_DispStringHCenterAt(string_buf, 440, 200 - MAIN_TITLE_HEIGHT);
	/* 如果没有日志文件,则不显示 */
	else GUI_DispStringHCenterAt("0/0", 440, 200- MAIN_TITLE_HEIGHT);

	return 0;
}


/*
*********************************************************************************************************
*	函 数 名: draw_alarm_log_sheet
*	功能说明: 绘制报警日志表格
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void draw_alarm_log_sheet(void)
{
	//设置画笔颜色
	GUI_SetColor(GUI_BLUE_98);
	//设置画笔大小
	GUI_SetPenSize(4);
	//开始绘制表格
	GUI_DrawLine(0,  110,480,110);
	GUI_DrawLine(0,  170,480,170);
	GUI_DrawLine(0,  230,480,230);
	GUI_DrawLine(0,  290,480,290);
	GUI_DrawLine(410,110,410,290);
}


/*
*********************************************************************************************************
*	函 数 名: _cblogs
*	功能说明: 警告日志显示界面
*	形    参：pMsg  参数指针
*	返 回 值: 
*********************************************************************************************************
*/
void _cblogs(WM_MESSAGE* pMsg) 
{
	WM_HWIN hItem = pMsg->hWin;
    int     NCode;
	int     Id;
	static  int     i = 0;

	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			/* 设置聚焦 */
			WM_SetFocus(hItem);
            //创建此页面所需的所有按钮
            GUI_SetFont(&GUI_FontHZ24);
			_get_logs1(0);
			i = log_ifo->total_log - 1;
			_CreateButton_P(hItem, " ",     ID_LOG_ADD_BUTTON,      430, 125 + MAIN_TITLE_HEIGHT, 20,  20, 0,&bmup,0);
			_CreateButton_P(hItem, " ",     ID_LOG_DEC_BUTTON,      430, 250 + MAIN_TITLE_HEIGHT, 20,  20, 0,&bmdown,0);
			_CreateButton_P(hItem, " ",     ID_LOG_BACK_BUTTON,     40,  60  + MAIN_TITLE_HEIGHT, 40,  40, 0,&bmback,0);
			/* 创建定时器 */
			WM_CreateTimer(hItem, /* 接受信息的窗口的句柄 */
						   0, 	                     /* 用户定义的Id。如果不对同一窗口使用多个定时器，此值可以设置为零。 */
						   10,                       /* 周期，此周期过后指定窗口应收到消息*/
						   0);
		case WM_PAINT:
			_PaintFrame();	
			draw_alarm_log_sheet();
			_PaintFrameSta();
			show_log(i);
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
				case ID_LOG_ADD_BUTTON: /* 向下查询日志 */
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED: 
							i = i + 1;
							if(i >= log_ifo->total_log)
							{
								i = log_ifo->total_log - 1;
							}
                            WM_InvalidateWindow(hItem);
							break;	
					}
					break;
                    
				case ID_LOG_DEC_BUTTON: /* 向上查询日志 */
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:							
							i = i - 1;
							if(i <= 0)
							{
								i = 0;
							}
                            WM_InvalidateWindow(hItem);
							break;
					}
					break;
                case ID_LOG_BACK_BUTTON: /* 返回设置菜单界面 */
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							break;
						case WM_NOTIFICATION_RELEASED:
							/* 删除这个创建的界面 */
							i = 0;//恢复显示日志第一条
                            _DeleteFrame();   
                            _CreateFrame(&_cbsystem_menu_1);
							break;
					}
					break;
			}
			break;
		default:
		WM_DefaultProc(pMsg);
	}
}

