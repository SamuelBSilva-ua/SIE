#include "../inc/GP2Y0A21YK0F.h"
#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


#define NUMBER_ROWS 11
#define NUMBER_COLS 2
#define MIN_VOLTAGE 0.5
#define MAX_VOLTAGE 2.3

static float Lookup_table[NUMBER_ROWS][NUMBER_COLS] =
{
 {100,2.3},
 {150,1.6},
 {200,1.3},
 {250,1.1},
 {300,0.9},
 {350,0.8},
 {400,0.75},
 {450,0.7},
 {500,0.6},
 {550,0.55},
 {600,0.5}
};

int CubicInterpolation(float ADC_value){
    int interpol_val;
        
    if(ADC_value <= MIN_VOLTAGE || ADC_value >= MAX_VOLTAGE){
        
        interpol_val=-1;
        
    }else{
        float third_degree=pow(ADC_value,3.0);
        float second_degree=pow(ADC_value,2.0);   

        interpol_val = -9.6160*third_degree + 63.4734*second_degree - 139.4803*ADC_value + 113.5413;   
        interpol_val=interpol_val*10;
    }

    
    return interpol_val;
}