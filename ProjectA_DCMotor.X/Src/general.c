#include "../includes/general.h"


void PinConfig(void){
    
    TRISAbits.TRISA3 = 0; //LED on pic
    
    //Pin Config
    TRISEbits.TRISE5 = 0;  //RE5 set as output - 1A l293
    TRISEbits.TRISE6 = 0;  //RE6 set as output - 2A l293

    TRISEbits.TRISE7 = 1;  //RE7 set as input  - Channel B Motor encoder
    TRISEbits.TRISE8 = 1;  //RE8 set as input  - Channel A Motor encoder    

}
