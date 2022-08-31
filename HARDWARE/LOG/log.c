#include "log.h"
#include "alarm_log.h"
#include "os_control.h"
#include "malloc.h"
/***
 * 当系统发生问题时
 * 打开SD卡文件夹,检查文件是否存在
 * 如果文件不存在,创建文件
 * 如果文件存在
 * 打开文件
 * 向文件写入错误日志
 ***/

/***
 * 打开文件传输界面后
 * 检测U盘是否插入
 * 如果U盘已经就绪
 * 点击开始传输按钮
 * 开始将数据从SD卡向U盘备份
 ***/

//默认日志打印路径
const TCHAR* log_pathn = "0:LOG";
//默认日志打印路径文件
const TCHAR* log_pname = "0:LOG/log.txt";
//数据复制U盘路径
u8  ulog_pname[20];
//sd卡文件
static FIL sd_file;	  	
//U盘文件
static FIL ud_file;	
//时间结构体
extern RTC_TimeTypeDef  RTC_TimeStructure;
extern RTC_DateTypeDef  RTC_DateStructure;
//按压深度
extern int depth_val;
/**********************************************************
*函数功能：打开文件夹，如果文件夹不存在就创建文件夹，存在就关闭
*输入：	pathn：文件夹名字
*说明：
***********************************************************/
u8 mkdir(const TCHAR* pathn)
{
	u8 res;
	res = f_opendir(&dir, pathn);   // 尝试打开目录
	if(res != FR_OK)
	{
		res = f_mkdir(pathn);       // 如果打开失败，则新建文件夹
	}
	else
	{
		res = f_closedir(&dir);     // 如果目录存在，则关闭
	}
	return res;
}

char string_buf[50] = {0};		    //用来存储写入文件的内容

/**********************************************************
*函数功能：向SD卡写入报警日志
*输入：	   无
*说明：   暂时没有设置传参,后期应该设置参数来选择
* 写入的报警日志,目前的报警日志是主动触发
*返回值:  无
***********************************************************/
void write_data_sd(void)
{
    u8          res;               //获取文件读写状态
	u16         filesize;          //获取当前文件大小
	OS_ERR      err;               //获取系统函数执行状态

    res = mkdir(log_pathn);        //检测文件夹状态
    if(res != FR_OK)
    {
        App_Printf("文件夹创建失败\r\n");
    }
	
	memset(string_buf,0,sizeof(string_buf));//清空数组	

	//合并报警日志
	sprintf(string_buf,"开始时间：%02d-%02d-%02d %02d:%02d:%02d@\r\n@",\
	RTC_DateStructure.RTC_Year,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,\
	RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
	sprintf(string_buf,"%s按压深度：%d mm@\r\n@",string_buf,depth_val);

	//获取要写入的文件里面已经写入的文件大小
    res = f_open(&sd_file,log_pname,FA_OPEN_EXISTING | FA_READ);
    filesize = f_size(&sd_file);
    f_close(&sd_file);
    
	//打开文件,开始追加写入内容
    res=f_open(&sd_file,log_pname, FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
    res=f_lseek(&sd_file,filesize); 
    res = f_write(&sd_file,string_buf,strlen(string_buf),&bw);
    f_close(&sd_file);
	//延时一下,假装写入很久
    OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_PERIODIC,&err);//延时一秒钟
}

/**
 * @brief 调用此函数,向SD卡写入错误日志
 * @param  mode: 要写入的日志
 * 1:电池电量低
 * 2:按压深度调整
 * @return uint8_t: 
 * 0:写入成功
 * 1:文件夹创建失败
 * 2:传入的参数有问题
 * 3:创建文件失败
 */
uint8_t write_log_old(uint8_t mode)
{
	u8          res;               //获取文件读写状态
	u16         filesize;          //获取当前文件大小
	res = mkdir(log_pathn);        //检测文件夹状态
    if(res != FR_OK)
    {
        App_Printf("文件夹创建失败\r\n");
		return 1;
    }
	/* 文件夹存在 */
	memset(string_buf,0,sizeof(string_buf));//清空数组
	//获取当前时间
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN,&RTC_DateStructure);
	//合并报警日志公共部分
	sprintf(string_buf,"开始时间：%02d-%02d-%02d %02d:%02d:%02d@\r\n@",\
	RTC_DateStructure.RTC_Year,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,\
	RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
	/* 合并报警日志 */
	switch (mode)
	{
		/* 电池电量不足 */
		case 1:
			sprintf(string_buf,"%s电池电量不足@2@\r\n@",string_buf);
			break;
		/* 按压深度有问题 */
		case 2:
			sprintf(string_buf,"%s按压深度：%d mm@1@\r\n@",string_buf,depth_val);
			break;
		default:
			App_Printf("写入日志的参数错误,请纠正\r\n");
			return 2;
	}
	//获取要写入的文件里面已经写入的文件大小
    res = f_open(&sd_file,log_pname,FA_OPEN_EXISTING | FA_READ);
    filesize = f_size(&sd_file);
    f_close(&sd_file);

	//打开文件,开始追加写入内容
    res=f_open(&sd_file,log_pname, FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
	if(res != FR_OK)
	{
		App_Printf("日志文件打开失败\r\n");
		return 3;
	}
    res=f_lseek(&sd_file,filesize); 
    res = f_write(&sd_file,string_buf,strlen(string_buf),&bw);
    f_close(&sd_file);
	/* 写入正常 */
	return 0;
}

/**
 * @brief 调用此函数,向SD卡写入错误日志
 * @param  mode: 要写入的日志
 * 1:电池电量低
 * 2:按压深度调整
 * 3:复位失败
 * @return uint8_t: 
 * 0:写入成功
 * 1:文件夹创建失败
 * 2:传入的参数有问题
 * 3:创建文件失败
 */
uint8_t write_log(uint8_t mode)
{
	u8          res;               //获取文件读写状态
	u16         filesize;          //获取当前文件大小
	res = mkdir(log_pathn);        //检测文件夹状态
    if(res != FR_OK)
    {
        App_Printf("文件夹创建失败\r\n");
		return 1;
    }
	/* 文件夹存在 */
	memset(string_buf,0,sizeof(string_buf));//清空数组
	//获取当前时间
	RTC_GetTime(RTC_Format_BIN,&RTC_TimeStructure);
	RTC_GetDate(RTC_Format_BIN,&RTC_DateStructure);
	//合并报警日志公共部分
	sprintf(string_buf,"开始时间：%02d-%02d-%02d %02d:%02d:%02d\r\n",\
	RTC_DateStructure.RTC_Year,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,\
	RTC_TimeStructure.RTC_Hours,RTC_TimeStructure.RTC_Minutes,RTC_TimeStructure.RTC_Seconds);
	/* 合并报警日志 */
	switch (mode)
	{
		/* 电池电量不足 */
		case 1:
			sprintf(string_buf,"%s电池电量不足\r\n2\r\n\r\n",string_buf);
			break;
		/* 按压深度有问题 */
		case 2:
			sprintf(string_buf,"%s按压深度：%d mm\r\n1\r\n\r\n",string_buf,depth_val);
			break;
		/* 复位失败 */
		case 3:
			sprintf(string_buf,"%s复位失败\r\n1\r\n\r\n",string_buf);
			break;
		default:
			App_Printf("写入日志的参数错误,请纠正\r\n");
			return 2;
	}
	//获取要写入的文件里面已经写入的文件大小
    res = f_open(&sd_file,log_pname,FA_OPEN_EXISTING | FA_READ);
    filesize = f_size(&sd_file);
    f_close(&sd_file);

	//打开文件,开始追加写入内容
    res=f_open(&sd_file,log_pname, FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
	if(res != FR_OK)
	{
		App_Printf("日志文件打开失败\r\n");
		return 3;
	}
    res=f_lseek(&sd_file,filesize); 
    res = f_write(&sd_file,string_buf,strlen(string_buf),&bw);
    f_close(&sd_file);
	/* 写入正常 */
	return 0;
}
/**********************************************************
*函数功能：文件复制
*输入：	psrc,pdst:源文件和目标文件
				fwmode:文件写入模式
				0:不覆盖原有的文件	
				1:覆盖原有的文件
*说明：将psrc文件,copy到pdst.
***********************************************************/
u8 log_copy(void *p_arg)
{
	u8          res;
	u16         filesize;
	u16         index=0;
	char        *buf = NULL;	
	
	(void)p_arg;
	
	//获取日志文件的大小
    res = f_open(&sd_file,log_pname,FA_OPEN_EXISTING | FA_READ);
    filesize = f_size(&sd_file);
    f_close(&sd_file);
	//从新打开文件
	res = f_open(&sd_file,log_pname,FA_READ);
    if(res != FR_OK)
    {
        App_Printf("SD 读取错误\r\n");
        return res;
    }//SD卡 文件读取成功
	else
	{	
		//查看U盘LOG文件夹是否存在	
		res = mkdir("3:LOG");
		while(index<0XFFFF)
		{
			sprintf((char*)ulog_pname,"3:LOG/log%05d.txt",index);
			res=f_open(&ud_file,(const TCHAR*)ulog_pname,FA_READ);//尝试打开这个文件
			if(res!=FR_OK)break;   //改文件名不存在
			else if(res==FR_OK) f_close(&ud_file);//如果打开成功，就关闭掉
			index++;
		}
		res = f_open(&ud_file,(const TCHAR*)ulog_pname,7);//新建文件		
		if(res != FR_OK)
		{
			App_Printf("U盘 读取错误\r\n");
			return res;
		}
	}
	
	//文件打开成功
    buf = mymalloc(SRAMEX,filesize);//申请内存
	
	//读取文件到缓存
	App_Printf("开始复制文件\r\n");
    for(;;)
	{
		res = f_read(&sd_file,buf,512,&br); //读取数据
		if(res||br == 0)break;              //没有可读取的数据了,结束本次读取
		res = f_write(&ud_file,buf,(UINT)br,&bw); //开始写入数据
		if(res||bw<br)break;
	}
		
    f_close(&sd_file);
	f_close(&ud_file);
	
	myfree(SRAMEX,buf);		//释放内存
	
	return res;
}






