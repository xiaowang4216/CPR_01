#ifndef _PWM_H
#define _PWM_H
#include "sys.h"


/* ���ʹ�ܿ��� */
#define MOTOR_EN  PAout(4)
/* �����ת������� */
#define MOTOR_DIR PAout(6)
/* ���������� */
#define MOTOR_A   PAout(8)
/* ˳ʱ����ת */
#define CW        1
/* ��ʱ����ת */
#define CCW       0


void motor_init(uint32_t arr,uint32_t psc);


#endif
