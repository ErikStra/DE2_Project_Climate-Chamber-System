#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdint.h>

// Proměnné z čidel
extern int16_t TEMP1;      
extern uint8_t HUM1;     
extern uint8_t HUM2;      
extern uint8_t hours;    
extern uint8_t minutes;  

extern uint8_t LED;           
extern int16_t TEMP2;        
extern uint8_t fan_big;     
extern uint8_t fan_small;   
extern uint8_t fan_led;     

extern uint8_t pump;       
extern uint8_t wlevel;     

// Proměnné pro nastavení
extern int8_t max_temp;       
extern int8_t min_temp;     
extern int8_t max_LED_temp;  
extern int8_t max_airhum;     
extern int8_t min_airhum;     
extern int8_t control;         
extern uint8_t sunset;       
extern uint8_t sunrise;        
extern int8_t max_soilhum;     
extern int8_t min_soilhum;     
extern int8_t water_time;       
extern int8_t autowater;        
extern int8_t chartest;        

#endif // VARIABLES_H
