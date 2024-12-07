#include "variables.h"


// Proměnné z čidel
int16_t TEMP1 = 2233;      
uint8_t HUM1 = 45;     
uint8_t HUM2 = 79;      
uint8_t hours = 12;    
uint8_t minutes = 56;  

uint8_t LED = 0;           
int16_t TEMP2 = 3721;        
uint8_t fan_big = 35;     
uint8_t fan_small = 100;   
uint8_t fan_led = 100;     

uint8_t pump = 0;       
uint8_t wlevel = 1;     

// Proměnné pro nastavení
int8_t max_temp = 25;       
int8_t min_temp = 15; 
int8_t max_LED_temp = 40;      
int8_t max_airhum = 75;     
int8_t min_airhum = 65;     
int8_t control = 0;         
uint8_t sunset = 0;       
uint8_t sunrise = 8;        
int8_t max_soilhum = 75;     
int8_t min_soilhum = 65;     
int8_t water_time = 8;       
int8_t autowater = 1;        
int8_t chartest = 0;        
