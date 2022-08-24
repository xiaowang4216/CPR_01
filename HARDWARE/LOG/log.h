#ifndef _LOG_H_
#define _LOG_H_

#include "includes.h"
#include "ff.h"
#include "fattester.h"
#include "exfuns.h"
#include "rtc.h"


void write_data_sd(void);
u8 My_Copy(TCHAR*psrc,TCHAR*pdst,u8 fwmode);
u8 log_copy(void *p_arg);
uint8_t write_log(uint8_t mode);

#endif



