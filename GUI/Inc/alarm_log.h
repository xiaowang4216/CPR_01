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
    /* �洢�������� */
    char  * time_ifo;
    /* �洢����ԭ�� */
    char  * warn_ifo;
    /* �洢�����ȼ� */
    uint8_t warn_grade; 
    /* �洢�ܵ���־��Ŀ */
    int     total_log;
    
}logmsg;

// typedef struct log_msg1
// {
//     struct log_msg *log_msg[50];
    
// }logmsg1;
void _get_logs(void *p_arg);

void _cblogs(WM_MESSAGE* pMsg) ;

#endif


