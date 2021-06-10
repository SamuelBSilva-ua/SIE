#include "../includes/config_bits.h" 
#include "../includes/pwm.h"
#include "../includes/general.h"
#include "../includes/timers.h"
#include "../includes/uart.h"
#include "../includes/external_int.h"
#include"../includes/PIController.h"
#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/* Motor */
#define MAIN_SHAFT 7
#define GEAR_RATIO 60
#define SHAFT_PULSES (MAIN_SHAFT*GEAR_RATIO)
#define FULL_ROTATION 360.00
#define ANGULAR_INCREMENT (FULL_ROTATION/SHAFT_PULSES)


/* Interruption Vectors */
#define UART1_VECTOR 24
#define INT1_VECTOR 7
#define TIMER3_VECTOR 12

/* User interface defines*/
#define COUNT_TIMER_USER_INTERFACE 10

/* Define State Machine States */
#define NO_OPERATION 0
#define FAST_STOP_MOTOR 1
#define CLOCKWISE_ROTATION 2
#define COUNTER_CLOCKWISE_ROTATION 3
#define STOP_MOTOR 4
#define RPM_MOTOR_SETPOINT 5
#define KP_UPDATE 6
#define KI_UPDATE 7

/* PI Controller*/
#define MINIMUM_KP 0
#define MAXIMUM_KP 100
#define MINIMUM_KI 0
#define MAXIMUM_KI 100
#define MINIMUM_RPM 10
#define MAXIMUM_RPM 50
#define H_SAMPLE 10

/* From system */
volatile uint8_t Ki;
volatile uint8_t Kp;
volatile uint8_t RPM;
volatile uint8_t user_operation;
volatile uint8_t UserOutputFlag;
volatile uint32_t CountPulses;
volatile float AnglePosition; 
volatile uint8_t pre_validation;

/* From user*/
volatile uint8_t setKi;
volatile uint8_t setKp;
volatile uint8_t RPM_reference;
volatile uint8_t DigitCount;
char rcvDigit[5];

void __ISR(UART1_VECTOR)UART1ISR(void);
void __ISR (INT1_VECTOR) ExtInt1ISR(void);
void __ISR(TIMER3_VECTOR)Timer3ISR(void);


int main(int argc, char** argv) {
    

    /* Set Interrupt Controller for multi-vector mode */
    INTCONSET = _INTCON_MVEC_MASK;
    
    /* Enable Interrupt Exceptions */
    // set the CP0 status IE bit high to turn on interrupts globally
    __builtin_enable_interrupts();
    
    PinConfig(); //Init output and input pin
   
    Timer2Init(20000, 8); //Set timer2 with frequency of 20kHz - used with PWM
    Timer3Init(10,256); //Set timer3 to 10Hz
    PWM1Init(2); //  Run PWM with timer 2
 
    
    //Init values
    PWM1_updateDutyCycle(0,2); //Set PWM to start with 50 duty-cycle
    setRotationRight(); 
    setKi=0;
    setKp=0;
    DigitCount=0;
    UserOutputFlag=0;
    CountPulses=0;
    AnglePosition=0.00;
    RPM_reference=50;
    user_operation=NO_OPERATION;
      
    /* UART and INT init*/
    UartInit(PBCLK, 115200);   
    __XC_UART = 1; /* Redirect stdin/stdout/stderr to UART1*/
    InitExternalInterrupt1(); //External interrupt inits
        
    /* Print user menu*/
    InterfaceMenu();
    

    while (1) {

        switch(user_operation){
            case NO_OPERATION:
                break;
            case CLOCKWISE_ROTATION:
                PutString("\n\rUser input-> Sentido da rotacao: Direita");
                setRotationRight();
                user_operation=NO_OPERATION;
                break;
            case COUNTER_CLOCKWISE_ROTATION:
                PutString("\n\rUser input-> Sentido da rotacao: Esquerda");
                setRotationLeft();
                user_operation=NO_OPERATION;
                break;
            case FAST_STOP_MOTOR:
                PutString("\n\rUser input-> Paragem rapida (fast stop)");
                fastStop();
                user_operation=NO_OPERATION;
                break;
            case STOP_MOTOR:
                PutString("\n\rUser input-> Paragem do motor");
                PWM1_updateDutyCycle(0,2);
                user_operation=NO_OPERATION;
                break;
            case RPM_MOTOR_SETPOINT:
                PutString("\n\rUser input-> Novo setpoint RPM com o valor: ");
                SendInteger(RPM_reference);
                user_operation=NO_OPERATION;
                break;
            case KP_UPDATE:
                PutString("\n\rUser input-> Novo Kp com o valor: ");
                SendInteger(Kp);
                UpdateKp(Kp);
                user_operation=NO_OPERATION;
                break;
           case KI_UPDATE:
                PutString("\n\rUser input-> Novo Ki com o valor: ");
                SendInteger(Ki);
                UpdateKi(Ki);
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
                
                PutString("\n\rInput: ");
            
                if(rcvchar == 0x52 || rcvchar == 0x72){
                    user_operation=CLOCKWISE_ROTATION;
                    PutString("r");
                }   

                if(rcvchar == 0x4c || rcvchar == 0x6c){
                    user_operation=COUNTER_CLOCKWISE_ROTATION;
                    PutString("l");
                }   

                if(rcvchar == 0x46 || rcvchar == 0x66){
                    user_operation=FAST_STOP_MOTOR;
                    PutString("f");
                }   

                if(rcvchar == 0x30){
                    user_operation=STOP_MOTOR;
                    PutString("0");
                    rcvchar=0;
                }
        }

        if(rcvchar == 0x50 || rcvchar == 0x70){
            PutString("p");
            PutString("\n\rUser input->Introduza valor do parametro Kp do controlador PI ");
            setKp=1;
        }   
        if(rcvchar == 0x49 || rcvchar == 0x69){
            PutString("i");
            PutString("\n\rUser input->Introduza valor do parametro Ki do controlador PI: ");
            setKi=1;
        }
        if(rcvchar >= 0x30 && rcvchar <= 0x39){
            rcvDigit[DigitCount]=rcvchar;
            PutChar(rcvchar);
            DigitCount++;
        }
        
        if (rcvchar == 0x0D){
            if(DigitCount==0){
                PutString("\n\rInput invalido!");
            }else{
                pre_validation=atoi(rcvDigit);
                if(setKp==1){

                    if(pre_validation < MINIMUM_KP || pre_validation > MAXIMUM_KP){
                      PutString("\n\rInput invalido! Introduzir valor entre [0 .. 100]");
                      user_operation=NO_OPERATION;
                    }else{
                      Kp=pre_validation;   
                      user_operation=KP_UPDATE;
                    }
                    setKp=0;       

                }else if (setKi==1){

                    if(pre_validation < MINIMUM_KI || pre_validation > MAXIMUM_KI){
                      PutString("\n\rInput invalido! Introduzir valor entre [0 .. 100]");
                      user_operation=NO_OPERATION;
                    }else{
                      Ki=pre_validation;   
                      user_operation=KI_UPDATE;
                    }
                    setKi=0;

                }else{

                  if(pre_validation < MINIMUM_RPM || pre_validation > MAXIMUM_RPM){
                    PutString("\n\rInput invalido! Introduzir valor entre [10 RPM .. 50 RPM]");
                    user_operation=NO_OPERATION;
                  }else{
                    RPM_reference=pre_validation;
                    user_operation=RPM_MOTOR_SETPOINT;
                  }

                }
                memset(rcvDigit, 0, sizeof rcvDigit); // Clear digit array
                DigitCount=0; //reset digit counter
            }
            
        }
    }
    
    IFS0bits.U1RXIF = 0; //Clear interrupt flag
}

void __ISR (INT1_VECTOR) ExtInt1ISR(void)
{
    CountPulses++;
    
    if(PORTEbits.RE7 == 1){//If RE7 == 1, Clockwise rotation
        if(abs(AnglePosition)==FULL_ROTATION){
            AnglePosition=0;    
        }else{
            AnglePosition+=ANGULAR_INCREMENT;              
        }
        
    } 
    else if(PORTEbits.RE7 == 0){//If RE7 == 0, Counter-clockwise rotation
        if(abs(AnglePosition)==0.00){
            AnglePosition=FULL_ROTATION;    
        }else{
            AnglePosition-=ANGULAR_INCREMENT;              
        }
    }
 
    IFS0bits.INT1IF = 0; // Reset int flag
}


void __ISR(TIMER3_VECTOR)Timer3ISR(void){
    
    UserOutputFlag++; //Increment variable every 0.1s
    
    RPM=(uint32_t)(CountPulses*60*H_SAMPLE)/(SHAFT_PULSES);
    
    PI_Execute(RPM_reference,RPM);
    
    CountPulses=0;
    
    if(UserOutputFlag==COUNT_TIMER_USER_INTERFACE) //Print to user at 1s (10x0.1))
    {
        UserOutputFlag=0; //Reset counter
        
        if(DigitCount == 0 && setKi==0 && setKp==0){ //only print if no more user inputs happening
            // to prevent clustering of the interface
            PutString("\n\rNova leitura-> RPM: ");
            SendInteger(RPM);
            PutString("\t\t Angular Position: ");
            printf("%6.2f",AnglePosition);   
        }
    }
    
    
    IFS0bits.T3IF = 0;
}