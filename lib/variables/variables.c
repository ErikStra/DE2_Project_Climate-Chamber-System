#include "variables.h"


// Proměnné z čidel
int16_t TEMP1 = 0;      
int16_t HUM1 = 0;     
int16_t HUM2 = 0;      
uint8_t hours = 0;    
uint8_t minutes = 0; 
uint8_t secs = 0; 

uint8_t LED = 0;           
int16_t TEMP2 = 0;        
uint8_t fan_big = 0;     
uint8_t fan_small = 0;   
uint8_t fan_led = 0;

uint8_t pump = 0;       
uint8_t wlevel = 0;     

// Proměnné pro nastavení
int8_t max_temp1 = 25;       
int8_t max_temp2 = 30;   
int8_t max_airhum = 70;     
int8_t control = 1;         
uint8_t sunset = 22;       
uint8_t sunrise = 8;        
int8_t min_soilhum = 30;     
int8_t water_time = 8;       
int8_t autowater = 1;
int8_t autolight = 1;           
int8_t chartest = 0;       

// PID konstanty
double KP1 = 2.0;  // Proporční konstanta
double KI1 = 0.5;  // Integrální konstanta
double KD1 = 1.0;  // Derivační konstanta

double KP2 = 2.0;  // Proporční konstanta
double KI2 = 0.5;  // Integrální konstanta
double KD2 = 1.0;  // Derivační konstanta

uint8_t drying_speed = 179; // speed of FAN 1 when humidity of air crosses upper limit