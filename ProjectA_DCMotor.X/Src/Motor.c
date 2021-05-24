#include <proc/p32mx795f512l.h>

#include "../includes/Motor.h"

/********************************************************************
* Function: 	setRotationRight()
* Precondition: -
* Input: 		-
* 
* Returns:      
 *              
* 
* Overview:     Set rotation "sentido horario"
* 
********************************************************************/	
void setRotationRight(){
     LATEbits.LATE5 = 0;
     LATEbits.LATE6 = 1;

}

/********************************************************************
* Function: 	setRotationLeft()
* Precondition: -
* Input: 		-
* 
* Returns:      
 *              
* 
* Overview:     Set rotation "sentido anti-horario"
* 
********************************************************************/	
void setRotationLeft(){
    LATEbits.LATE5 = 1;
    LATEbits.LATE6 = 0;

}

/********************************************************************
* Function: 	fastStop()
* Precondition: -
* Input: 		-
* 
* Returns:      
 *              
* 
* Overview:     fast motor stop
* 
********************************************************************/	
void fastStop(){
    LATEbits.LATE5 = 0;
    LATEbits.LATE6 = 0;
}


/********************************************************************
* Function: 	setRPM()
* Precondition: -
* Input: 		- setpoint
*               - rpm desired
* Returns:      
 *              
* 
* Overview:     set rpm desired
* 
********************************************************************/	
void setRPM(uint8_t setpoint, uint8_t rpm){
    uint8_t dif=0;
    if(rpm > setpoint){ //decrease rpm
        dif=rpm-setpoint;
        OC1RS=1.02*(dif);
    }
    else if(rpm < setpoint){ //increase rpm
        dif=setpoint-rpm;
        //duty*0.098*(dif)
    }
    
}