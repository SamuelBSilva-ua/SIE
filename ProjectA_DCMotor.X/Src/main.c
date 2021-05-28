#include "../includes/config_bits.h" 
#include "../includes/pwm.h"
#include "../includes/timers.h"
#include "../includes/uart.h"
#include "../includes/external_int.h"
#include <xc.h>
#include <stdint.h>


void __ISR (_EXTERNAL_1_VECTOR, IPL5SRS) ExtInt1ISR(void)
{
    
    IFS0bits.INT1IF = 0; // Reset int flag
}



int main(int argc, char** argv) {
    

    /* Set Interrupt Controller for multi-vector mode */
    INTCONSET = _INTCON_MVEC_MASK;
    
    /* Enable Interrupt Exceptions */
    // set the CP0 status IE bit high to turn on interrupts globally
    __builtin_enable_interrupts();
    
    
    PinConfig(); //Init output and input pins
    InitExternalInterrupt1(); //External interrupt inits
    Timer2Init(20000, 8); //Set timer with frequency of 20kHz
    PWM1Init(2); //  Run PWM with timer 2
 
    
    //Init values:
    PWM1_updateDutyCycle(50,2); //Set PWM to start with 50 duty-cycle
    
   
    while (1) {
        
        PORTAbits.RA3 = !PORTAbits.RA3;
        
   
    }

    return (EXIT_SUCCESS);
}
