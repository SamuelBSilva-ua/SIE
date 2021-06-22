/* 
 * File:   pwm.h
 * Author: Leandro Alexandrino, 74342
 * Author: Samuel Silva, 80773
 * 
 */

#ifndef PWM_H
#define	PWM_H

#include <stdint.h>

/* Interval values */
#define PWM1_MINIMUM_DUTYCYCLE 0
#define PWM1_MAXIMUM_DUTYCYCLE 100

/* Return messages */
#define PWM1_SUCCESS 0
#define PWM1_INVALID_TIMER -1
#define PWM1_INVALID_DUTYCYCLE -2

/* Functions */
int PWM2Init(uint8_t timer_number);
int PWM2_updateDutyCycle(uint8_t dutycycle,uint8_t timer_number);
int PWM2_adjustment(int adjustment, uint8_t timer_number);

#endif	/* PWM_H */

