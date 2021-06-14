#ifndef PICONTROLLER_H
#define	PICONTROLLER_H

#include <stdint.h>

void PI_Execute(uint8_t setpoint, uint8_t rpm);
void UpdateKp(float Kp);
void UpdateKi(float Ki);
void PI_Clear(void);

#endif	/* PICONTROLLER_H */

