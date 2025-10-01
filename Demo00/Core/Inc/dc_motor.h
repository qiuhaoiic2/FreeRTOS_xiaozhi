#ifndef _DC_MOTOR_H_
#define _DC_MOTOR_H_

#include "stdint.h"

void dc_motor_init(void);
void dc_motor_set_speed(uint16_t speed);
void dc_motor_stop(void);

#endif
