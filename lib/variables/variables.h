#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdint.h>

// Proměnné z čidel
extern int16_t TEMP1;      
extern int16_t HUM1;     
extern int16_t HUM2;      
extern uint8_t hours;    
extern uint8_t minutes;  
extern uint8_t secs;

extern uint8_t LED;           
extern int16_t TEMP2;     
extern int16_t PRESSURE;  
extern uint8_t fan_big;     
extern uint8_t fan_small;   
extern uint8_t fan_led;    

extern uint8_t pump;       
extern uint8_t wlevel;  
extern uint16_t water_delay;

// Proměnné pro nastavení
extern int8_t max_temp1;       
extern int8_t max_temp2;     
extern int8_t max_airhum;     
extern int8_t control;         
extern uint8_t sunset;       
extern uint8_t sunrise;            
extern int8_t min_soilhum;     
extern int8_t water_time;       
extern int8_t autowater;  
extern int8_t autolight;        
extern int8_t chartest;        

// PID konstanty
double KP1;  // Proporční konstanta
double KI1;  // Integrální konstanta
double KD1;  // Derivační konstanta

double KP2;  // Proporční konstanta
double KI2;  // Integrální konstanta
double KD2;  // Derivační konstanta

uint8_t drying_speed; // speed of FAN 1 when humidity of air crosses upper limit

#endif // VARIABLES_H
