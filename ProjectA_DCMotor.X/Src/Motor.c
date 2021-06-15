/* 
 * File:   Motor.h
 * Author: Samuel
 *
 * Created on 24 de Maio de 2021, 15:54
 */

#include <xc.h>
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
