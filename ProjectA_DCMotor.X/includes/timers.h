/* 
 * File:   timers.h
 * Author: Leandro Alexandrino, 74342
 * Author: Samuel Silva, 80773
 *
 */

#ifndef TIMERS_H
#define	TIMERS_H

#include <stdint.h>


/* Interval values*/
#define MINIMUM_PRESCALER 1
#define MAXIMUM_PRESCALER 256

/* Return messages */
#define TIMER_SUCESS 0
#define TIMER_PRESCALER_ERROR -1


/* Functions */
int Timer2Init(uint32_t frequency, uint16_t prescaler);
int Timer3Init(uint32_t frequency, uint16_t prescaler);

#endif	/* TIMERS_H */

