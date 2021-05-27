#pragma once

#include <Zumo32U4.h>


//Variabler som er definert i batteryDummy.cpp

extern int adresse4; //BatteryLife 
extern int adresse1; //BatteryHealth (Problem: batteryHealth = 100 - (5*batteryCycles); i GVar, trengs likevel adresse1?
extern int adresse2; //BatteryCycles
extern int adresse3; //account_balance_zumo
extern long batteryLife_stored; //Lagret EEPROM-verdi
extern int batteryHealth_stored; //Lagret EEPROM-verdi
extern int batteryCycles_stored; //Lagret EEPROM-verdi
extern long balance_stored; //Lagret EEPROM-verdi
extern unsigned long tid; //Holder tid
extern long ladebelop;
extern long maxBelop;

extern bool loop_func1; //Initialiserer løkke
extern bool loop_func2; //Initialiserer løkke
extern bool loop_func3; //Initialiserer løkke
extern bool loop_func4; //Initialiserer løkke
extern bool emrgncy_charge_used;

extern long batteryMax; //cpr_maxright + cpr_max_left pga ulike counts. (385020+422100)
extern long batteryLife; //403560=50% 645969=80% (for testing) 
extern long battery5percentL; //5 prosent av batteryMax(807120*0,05)
extern long battery15percent; //15 prosent av batteryMax (807120*0,15)
extern long account_balance_zumo;
extern int transferred_amount;
extern int batteryCycles;
extern int batteryHealth;
extern int level_0; //Kritisk batteriHealth på 5%
extern int level_1; //Lavt batterinivå på 50%
extern long batteryChargeAmount;
extern float batteryPercent;

extern unsigned long buttonAMillis;
extern unsigned long buttonAMillis2;
extern unsigned long buttonATid;
extern long buttonAPeriode;
extern unsigned long buttonBMillis;
extern unsigned long buttonBMillis2;
extern unsigned long buttonBTid;
extern long buttonBPeriode;

extern bool loopA;
extern bool loopB;


//Instanseringer ekstern

extern  Zumo32U4Motors motors;
extern  Zumo32U4LCD lcd;
extern  Zumo32U4Encoders encoders;
extern  Zumo32U4ButtonA buttonA;
extern  Zumo32U4ButtonB buttonB;
extern  Zumo32U4ButtonC buttonC;
extern Zumo32U4Buzzer buzzer;

//Funksjonene

void warning15(); //Funksjon for varsling ved lavt batterinivå
void lade(); //Ladefunksjon
void lade_belop(); //Funksjon for å betale for lading
void lademodus(); //Simulering av lading
void showBattery(); //Funksjon for å vise batteri i %
void serviceBattery(); //Funksjon for service av batteri
void replaceBattery(); //Funksjon for replace
void batteryDegrade(); //Funksjon for å simulere batterifeil

void innskuddsmodus(); //Funksjon for å legge inn pæng på konto
void showBalance(); //Funksjon for å vise pæng på konto
void updateEEPROM(); //Funksjon for å oppdatere EEPROM
void clearEEPROM(); //Funksjon for å cleare minne på EEPROM

void ButtonA(); //Samlefunksjon for lademodus
void ButtonB(); //Samlefunksjon for bankmodus
void print1sek(String x, String y); //Funksjon for å printe ting i 1 sek
