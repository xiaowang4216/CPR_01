#ifndef _ALARM_LOG_H_
#define _ALARM_LOG_H_

#include "gui_public.h"
#include "includes.h"
#include "fattester.h"
#include "exfuns.h"
#include "os_control.h"
#include "malloc.h"

typedef struct log_msg
{
    /* 存储日期数据 */
    char  * time_ifo;
    /* 存储报警原因 */
    char  * warn_ifo;
    /* 存储报警等级 */
    uint8_t warn_grade; 
    /* 存储总的日志数目 */
    int     total_log;
    
}logmsg;

// typedef struct log_msg1
// {
//     struct log_msg *log_msg[50];
    
// }logmsg1;
void _get_logs(void *p_arg);

void _cblogs(WM_MESSAGE* pMsg) ;

#endif


