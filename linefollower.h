#pragma once

#include <Zumo32U4.h>
#include "TurnSensor.h"
#include <math.h> 
#include "battery.h" //Tillegsfiler for funksjonene til batteriet
#include "kjoreFunk.h"

//Instanseringer som er i hovedfilen
extern Zumo32U4Motors motors;
extern Zumo32U4Encoders encoders;
extern Zumo32U4LCD lcd;
extern Zumo32U4ButtonA buttonA;
extern Zumo32U4ButtonB buttonB;
extern Zumo32U4ButtonC buttonC;
extern Zumo32U4LineSensors lineSensors; //Ærklærer frontarray-sensorene til Zumoen for lineSensors
extern L3G gyro;

#define NUM_SENSORS 5 //Antall sensorer
extern unsigned int lineSensorValues[]; //Values av antall sensorer

//Variabler til linefollower

extern const uint16_t sensorThreshold;
extern int Turnspeed;
extern int Leftspeed; //Konstant leftspeed på 200
extern int Rightspeed; //Konstant rightspeed på 186 grunnet feil på Zumo

extern int antall_kjegler;

extern int intersection;
extern unsigned long currentmillis;
extern unsigned long previousmillis;
extern unsigned long millis1;
extern unsigned long millis2;
extern unsigned long tid;
extern unsigned int periode;
extern unsigned int s;
extern unsigned int x;
extern unsigned int kjegle;
extern int period;
extern bool loop1;
extern bool i;
extern bool ferdig;
extern bool input_ferdig;
extern bool fortsett1;
extern bool fortsett2;
extern bool buttonApressed;
extern bool buttonBpressed;
extern String distance_instruction[];
extern int distance_count;
extern int distance;

//Funksjoner i linefollower
extern void linefollower(); //Linefollowerfunksjon
extern void kvadrat(); //Funksjon som lar bilen kjøre kvadrat
extern void sirkel();  //Funksjon som lar bilen kjøre i sirkel
extern void fremogtilbake(); //Funksjon som kjører Zumoen frem, rotere 180 grader og kjører tilbake.
extern void turn180(); //Funksjon som gjør at bilen roterer 180 grader
extern void stopp(); //Stopper Zumoen
extern void Turnleft(); //Zumoen roterer 90 grader til venstre
extern void Turnright(); //Zumoen roterer 90 grader til høyre
extern void FoundLeft_intersectioncheck();
extern void Foundright_intersectioncheck();
extern void Deadendcheck();
extern void linefollowerprogram();
extern void calibrateSensors();
