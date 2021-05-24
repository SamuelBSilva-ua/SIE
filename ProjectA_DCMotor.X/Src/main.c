#include "../includes/config_bits.h" 

#include <xc.h>
#include <stdint.h>


int main(int argc, char** argv) {
    
   

    
    TRISAbits.TRISA3 = 0;
    
    //Pin Config
    TRISEbits.TRISE5 = 0;  //RE5 set as output - 1A l293
    TRISEbits.TRISE6 = 0;  //RE6 set as output - 2A l293

    TRISEbits.TRISE7 = 1;  //RE7 set as input  - Channel B Motor encoder
    TRISEbits.TRISE8 = 1;  //RE8 set as input  - Channel A Motor encoder
    
    Timer2Init(20000, 8); //frequency, prescaler
    PWM1Init(2); //  PWM using timer 2
    
    
   
    while (1) {
        PWM1_updateDutyCycle(50,2);
        
        PORTAbits.RA3 = !PORTAbits.RA3;
        
       
     
       

    }

    return (EXIT_SUCCESS);
}
