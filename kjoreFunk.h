#pragma once

#include "battery.h"
#include <Zumo32U4.h>


//Variablene som kan brukes i hovedfilen

extern long countLeft; 
extern long countRight;
extern long battery_used; //Funksjon for hvor mange counts batteriet har brukt 

extern long total_countLeft; //Total count left siden void setup()
extern long total_countRight; //Total count right siden void setup()
extern long total_negative_right; //Total
extern long total_negative_left;

extern float distance_meters; //Lagrer distanse kjørt i meter
extern long total_distance; //Lagrer distanse kjørt i centimeter

extern float speed_sec_right; //Variabel som holder fart på høyrehjul
extern float speed_sec_left; //Variabel som holder fart på venstrehjul


//Funksjonene som kan brukes i hovedfilen

void speedometer(); //Speedometer (cm/s)
void distanse(); //Distanse kjørt i centimeter eller meter
void utladning(); //Funksjon som viser utladning 
void showStats(); //Funksjon som viser distanse kjørt siden setup og batterylife
