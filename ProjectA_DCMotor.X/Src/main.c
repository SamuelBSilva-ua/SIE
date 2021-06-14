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
#define MINIMUM_KP 0.00
#define MAXIMUM_KP 100.00
#define MINIMUM_KI 0.00
#define MAXIMUM_KI 100.00
#define MINIMUM_RPM 10
#define MAXIMUM_RPM 50
#define H_SAMPLE 10

/* From system */
volatile float Ki;
volatile float Kp;
volatile uint8_t RPM;
volatile uint8_t user_operation;
volatile uint8_t UserOutputFlag;
volatile uint32_t CountPulses;
volatile float AnglePosition; 
volatile uint8_t pre_validation_RPM;
volatile float pre_validation_controller;
volatile uint8_t MotorStatus;

/* From user*/
volatile float setKi;
volatile float setKp;
volatile uint8_t RPM_reference;
volatile uint8_t DigitCount;
char rcvDigit[7];

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
    PWM1_updateDutyCycle(0,2); //Set PWM to start with 0% duty-cycle
    MotorStatus=0;
    setRotationRight(); 
    setKi=0;
    setKp=0;
    DigitCount=0;
    UserOutputFlag=0;
    CountPulses=0;
    AnglePosition=0.00;
    RPM_reference=0;
    user_operation=NO_OPERATION;
      
    /* UART and INT init*/
    UartInit(PBCLK, 115200);   
    __XC_UART = 1; /* Redirect stdin/stdout/stderr to UART1*/
    InitExternalInterrupt1(); //External interrupt inits
        
    /* Print user menu*/
    InterfaceMenu();
    
    /* STATE MACHINE */
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
                MotorStatus=0;
                user_operation=NO_OPERATION;
                break;
            case STOP_MOTOR:
                PutString("\n\rUser input-> Paragem do motor");
                PWM1_updateDutyCycle(0,2);
                MotorStatus=0;
                user_operation=NO_OPERATION;
                break;
            case RPM_MOTOR_SETPOINT:
                PutString("\n\rUser input-> Novo setpoint RPM com o valor: ");
                SendInteger(RPM_reference);
                MotorStatus=1;
                PI_Clear();
                user_operation=NO_OPERATION;
                break;
            case KP_UPDATE:
                PutString("\n\rUser input-> Novo Kp com o valor: ");
                printf("%6.2f",Kp);
                UpdateKp(Kp);
                user_operation=NO_OPERATION;
                break;
           case KI_UPDATE:
                PutString("\n\rUser input-> Novo Ki com o valor: ");
                printf("%6.2f",Ki);
                UpdateKi(Ki);
                user_operation=NO_OPERATION;
                break;
            default:
                break;
        }
    }

    return (EXIT_SUCCESS);
}

/* User interface - receiving */
void __ISR(UART1_VECTOR)UART1ISR(void){

    if(IFS0bits.U1RXIF == 1){ //when interrupt flag is ON
       
        uint8_t rcvchar = U1RXREG; //byte received stored in variable
        
        if(DigitCount==0){ //if byte received is a command(left,right, stop, fast-stop)
                
                PutString("\n\rInput: "); //output to user interface
            
                if(rcvchar == 0x52 || rcvchar == 0x72){ //received 'r'/'R'
                    user_operation=CLOCKWISE_ROTATION; //new state -> Clockwise rotation
                    PutString("r");
                }   

                if(rcvchar == 0x4c || rcvchar == 0x6c){ //received 'l'/'L'
                    user_operation=COUNTER_CLOCKWISE_ROTATION;//new state -> Counter-clockwise rotation
                    PutString("l");
                }   

                if(rcvchar == 0x46 || rcvchar == 0x66){ //received 'f'/'F'
                    user_operation=FAST_STOP_MOTOR; //new state -> fast stop 
                    PutString("f");
                }   

                if(rcvchar == 0x30 && (setKp==0 && setKi==0)){//received '0' and not setting Kp values (floats)
                    user_operation=STOP_MOTOR; //new state -> Stop motor
                    PutString("0");
                    rcvchar=0; //clear rcvchar
                }
        }
        
        /* Kp value update menu now set*/
        if(rcvchar == 0x50 || rcvchar == 0x70){  //received 'p'/'P'
            PutString("p");
            PutString("\n\rUser input->Introduza valor do parametro Kp do controlador PI ");
            setKp=1; //Set Kp menu is active
        }   
        
        /* Ki value update menu now set*/
        if(rcvchar == 0x49 || rcvchar == 0x69){ //received 'i'/'I'
            PutString("i");
            PutString("\n\rUser input->Introduza valor do parametro Ki do controlador PI: ");
            setKi=1; //Set Ki menu is active
        }
        
        if(rcvchar == 0x2E && (setKp==1 || setKi==1) && DigitCount>0){ //received "." for Kp or Ki value
            rcvDigit[DigitCount]=rcvchar; //store on buffer
            PutChar(rcvchar); //print on user interface
            DigitCount++; //update buffer index
        }
        
        if(rcvchar >= 0x30 && rcvchar <= 0x39){ //received 0 .. 9
            rcvDigit[DigitCount]=rcvchar;  //store on buffer
            PutChar(rcvchar); //print on user interface
            DigitCount++; //update buffer index
        }
        
        if (rcvchar == 0x0D){ //received "ENTER" key
            if(DigitCount==0){ //if no inputs
                PutString("\n\rInput invalido!");
            }else{ //input processing
                if(setKp==1){ //if Kp update is set
                    pre_validation_controller=atof(rcvDigit); //convert buffer to float
                    if(pre_validation_controller < MINIMUM_KP || pre_validation_controller > MAXIMUM_KP){ //validate value range
                      PutString("\n\rInput invalido! Introduzir valor entre [0 .. 100]"); //value is not valid
                      user_operation=NO_OPERATION;
                    }else{
                      Kp=pre_validation_controller;   //valid value, update Kp value
                      user_operation=KP_UPDATE; //State machine new state
                    }
                    setKp=0; //clear menu flag

                }else if (setKi==1){//if Ki update is set
                    pre_validation_controller=atof(rcvDigit); //convert buffer to float
                    if(pre_validation_controller < MINIMUM_KI || pre_validation_controller > MAXIMUM_KI){ //validate value range
                      PutString("\n\rInput invalido! Introduzir valor entre [0 .. 100]"); //value is not valid
                      user_operation=NO_OPERATION;
                    }else{
                      Ki=pre_validation_controller;    //valid value, update Ki value
                      user_operation=KI_UPDATE; //State machine new state
                    }
                    setKi=0;  //clear menu flag

                }else{ //RPM value
                  pre_validation_RPM=atoi(rcvDigit); //convert buffer to int
                  if(pre_validation_RPM < MINIMUM_RPM || pre_validation_RPM > MAXIMUM_RPM){ //validate value range
                    PutString("\n\rInput invalido! Introduzir valor entre [10 RPM .. 50 RPM]"); //value is not valid
                    user_operation=NO_OPERATION;
                  }else{
                    RPM_reference=pre_validation_RPM; //valid value, update RPM value (setpoint)
                    user_operation=RPM_MOTOR_SETPOINT; //State machine new state
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
    CountPulses++; //Count rising edges number 
    
    if(PORTEbits.RE7 == 1){//If RE7 == 1, Clockwise rotation
        if(abs(AnglePosition)==FULL_ROTATION){ 
            AnglePosition=0;    //reset angular position on full turn
        }else{
            AnglePosition+=ANGULAR_INCREMENT; //increment angular position on clockwise rotation
        }
        
    } 
    else if(PORTEbits.RE7 == 0){//If RE7 == 0, Counter-clockwise rotation
        if(abs(AnglePosition)==0.00){
            AnglePosition=FULL_ROTATION;    //reset angular position on full turn
        }else{
            AnglePosition-=ANGULAR_INCREMENT; //decrement angular position on counter-clockwise rotation         
        }
    }
 
    IFS0bits.INT1IF = 0; // Reset int flag
}


void __ISR(TIMER3_VECTOR)Timer3ISR(void){
    
    UserOutputFlag++; //Increment variable every 0.1s
    
    RPM=(uint32_t)(CountPulses*60*H_SAMPLE)/(SHAFT_PULSES); //calculate RPM
    
    if(MotorStatus == 1){
        PI_Execute(RPM_reference,RPM); //PI controller with RPM obtained        
    }
    
    CountPulses=0; //reset count of rising edges
    
    if(UserOutputFlag==COUNT_TIMER_USER_INTERFACE) //Print to user at 1s (10x0.1))
    {
        UserOutputFlag=0; //Reset counter
        
        if(DigitCount == 0 && setKi==0 && setKp==0){ //only print if no more user inputs happening
            // to prevent clustering of the interface
            
            /* Print to user interface */
            PutString("\n\rNova leitura-> RPM: ");
            SendInteger(RPM);
            PutString("\t\t Angular Position: ");
            printf("%6.2f",AnglePosition);   
        }
    }
    
    
    IFS0bits.T3IF = 0; //reset interrupt flag
}