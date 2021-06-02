#include "../includes/config_bits.h" 
#include "../includes/pwm.h"
#include "../includes/general.h"
#include "../includes/timers.h"
#include "../includes/uart.h"
#include "../includes/external_int.h"
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>



#define UART1_VECTOR 24
#define INT1_VECTOR 7
#define NO_OPERATION 0
#define FAST_STOP_MOTOR 1
#define CLOCKWISE_ROTATION 2
#define COUNTER_CLOCKWISE_ROTATION 3
#define STOP_MOTOR 4
#define RPM_MOTOR_SETPOINT 5
#define KP_UPDATE 6
#define KI_UPDATE 7

//From system
volatile uint8_t h;
volatile uint8_t Ki;
volatile uint8_t Kp;
volatile uint8_t RPM;
volatile uint8_t user_operation;

// From user
volatile uint8_t setKi;
volatile uint8_t setKp;
volatile uint8_t DigitCount;
char rcvDigit[3];


void __ISR (INT1_VECTOR) ExtInt1ISR(void)
{

    IFS0bits.INT1IF = 0; // Reset int flag
}



int main(int argc, char** argv) {
    

    /* Set Interrupt Controller for multi-vector mode */
    INTCONSET = _INTCON_MVEC_MASK;
    
    /* Enable Interrupt Exceptions */
    // set the CP0 status IE bit high to turn on interrupts globally
    __builtin_enable_interrupts();
    
    
    PinConfig(); //Init output and input pin
   
    Timer2Init(20000, 8); //Set timer with frequency of 20kHz
    PWM1Init(2); //  Run PWM with timer 2
 
    
    //Init values:
    PWM1_updateDutyCycle(50,2); //Set PWM to start with 50 duty-cycle
    setRotationLeft();
    setKi=0;
    setKp=0;
    DigitCount=0;
    user_operation=NO_OPERATION;
    
    
    UartInit(PBCLK, 115200);   
    __XC_UART = 1; /* Redirect stdin/stdout/stderr to UART1*/
       InterfaceMenu();
    
    InitExternalInterrupt1(); //External interrupt inits
        

    

    while (1) {

        switch(user_operation){
            case NO_OPERATION:
                break;
            case CLOCKWISE_ROTATION:
                PutString("Sentido da rotacao -> Direita \n\r");
                setRotationRight();
                user_operation=NO_OPERATION;
                break;
            case COUNTER_CLOCKWISE_ROTATION:
                PutString("Sentido da rotacao -> Esquerda \n\r");
                setRotationLeft();
                user_operation=NO_OPERATION;
                break;
            case FAST_STOP_MOTOR:
                PutString("Fast Stop \n\r");
                fastStop();
                user_operation=NO_OPERATION;
                break;
            case STOP_MOTOR:
                PutString("Paragem do motor \n\r");
                PWM1_updateDutyCycle(0,2);
                user_operation=NO_OPERATION;
                break;
            case RPM_MOTOR_SETPOINT:
                PutString("New RPM \n\r");
                //TODO: PWM1_updateDutyCycle(0,2);
                user_operation=NO_OPERATION;
                break;
            case KP_UPDATE:
                PutString("New Kp \n\r");
                user_operation=NO_OPERATION;
                break;
           case KI_UPDATE:
                PutString("New Ki \n\r");
                user_operation=NO_OPERATION;
                break;
            default:
                break;
        }
    }

    return (EXIT_SUCCESS);
}
void __ISR(UART1_VECTOR)UART1ISR(void){
    if(IFS0bits.U1RXIF == 1){
        

        uint8_t rcvchar = U1RXREG;
        
        if(DigitCount==0){
                if(rcvchar == 0x52 || rcvchar == 0x72){
                    user_operation=CLOCKWISE_ROTATION;
                }   

                if(rcvchar == 0x4c || rcvchar == 0x6c){
                    user_operation=COUNTER_CLOCKWISE_ROTATION;
                }   

                if(rcvchar == 0x46 || rcvchar == 0x66){
                    user_operation=FAST_STOP_MOTOR;
                }   

                if(rcvchar == 0x30){
                    user_operation=STOP_MOTOR;
                }
        }

        if(rcvchar == 0x50 || rcvchar == 0x70){
            PutString("Valor do parametro Kp do controlador PI \n\r");
            setKp=1;
        }   
        if(rcvchar == 0x49 || rcvchar == 0x69){
            PutString("Valor do parametro Ki do controlador PI \n\r");
            setKi=1;
        }
        if(rcvchar > 0x30 && rcvchar < 0x3a){
            rcvDigit[DigitCount]=rcvchar;
            DigitCount++;
        }
        
        if (rcvchar == 0x0D){
            DigitCount=0;
            if(setKp==1){
              Kp=atoi(rcvDigit);
              setKp=0;
              user_operation=KP_UPDATE;
            }else if (setKi==1){
              Ki=atoi(rcvDigit);
              setKi=0;
              user_operation=KI_UPDATE;
            }else{
               RPM=atoi(rcvDigit);
               user_operation=RPM_MOTOR_SETPOINT;
            }
            
        }
    }
            IFS0bits.U1RXIF = 0;
}

//TODO: TIMER 0.1S