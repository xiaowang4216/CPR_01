#ifndef _PWM_H
#define _PWM_H
#include "sys.h"


/* 电机使能控制 */
#define MOTOR_EN  PAout(4)
/* 电机旋转方向控制 */
#define MOTOR_DIR PAout(6)
/* 共阳极引脚 */
#define MOTOR_A   PAout(8)
/* 顺时针旋转 */
#define CW        1
/* 逆时针旋转 */
#define CCW       0


void motor_init(uint32_t arr,uint32_t psc);


#endif
