
#include <Wire.h>
#include <EEPROM.h>
#include "kjoreFunk.h"



//Variabler for .cpp
long total_countLeft;
long total_countRight;
long total_negative_right;
long total_negative_left;

float distance_meters; //Lagrer distanse kjørt i meter
long total_distance = 0; //Lagrer distanse kjørt i centimeter

float speed_sec_right; //Variabel som holder fart på høyrehjul
float speed_sec_left; //Variabel som holder fart på venstrehjul

long countLeft = 0; //I hovedfilen
long countRight = 0; //I hovedfilen
long battery_used = 0; //Funksjon for hvor mange counts batteriet har brukt



void utladning() { //Funksjon for utladning av batteri
  batteryLife -= (countLeft + countRight); //Trekker fra counts fra batteriet.
  battery_used = batteryMax - batteryLife; //Viser hvor mye av batteriet som er brukt (i counts)
  EEPROM.update(adresse4, batteryLife); //Oppdaterer value
}



void speedometer() { //Funksjon for speedometer, må kjøres hvert sekund (viser i intervaller på 1s)
  float rev = 909.7F; //Counts per rev
  float cm_rev = 11.624F; //Cm per rev

  //Speedometer
  float rev_left = countLeft / rev; //Formel for å vise hvor mange runder den kjørte
  float rev_right = countRight / rev; //Formel for å vise hvor mange runder den kjørte
  
  float distance_left = rev_left * cm_rev;//Distanse den kjørte iløpet av 1s (i cm/s)
  float distance_right =  rev_right * cm_rev; //Distance den kjørte iløpet av 1s (i cm/s)

  //Disse 2 (3) Variablene kalles på i print når du vil vise fart
  speed_sec_left = round(distance_left * 100.0) / 100; //Runder av tallet til 1 desimal.
  speed_sec_right = round(distance_right * 100.0) / 100; //Runder av tallet til 1 desimal.

  float avg_speed = (speed_sec_right + speed_sec_left) /2; //Inkluder denne hvis du vil ha gjennomsnittsfart fra begge hjulene
}



void distanse() { //Funksjon som viser hvor langt bilen har kjørt i centimeter eller meter
  float rev = 909.7F; //Counts per rev
  float cm_rev = 11.624F; //Cm per rev
  
  total_countRight += countRight;
  total_countLeft += countLeft;
  
  float total_rev_left = total_countLeft / rev; //Formel for å vise hvor mange runder den har kjørt toalt
  float total_rev_right = total_countRight / rev; //Formel for å vise hvor mange runder den har kjørt totalt
  
  float total_distance_left = total_rev_left * cm_rev; //Totalt distanse kjørt på venstrehjul
  float total_distance_right =  total_rev_right * cm_rev; //Totalt distanse kjørt på høyrehjul

  //Disse to variablene kalles på i print når du ønsker å printe distanse kjørt.
  total_distance = (total_distance_right + total_distance_left) / 2; //Beregner gjennomsnittet av de to distansene for et mest nøyaktig tall
  distance_meters = total_distance /100; //Distansen i meter
}

void showStats() { //Viser batteryLife i prosent samt distanse kjørt siden bilen startet
  print1sek("Battery", "life is;");
  
  lcd.clear();
  lcd.print("Counts:");
  lcd.gotoXY(0,1);
  lcd.print(batteryLife);
  delay(2000);
  
  print1sek("Distance", "driven:");
  
  lcd.clear();
  lcd.print(total_distance);
  lcd.gotoXY(0,1);
  lcd.print("cm.");
  delay(2000);
}
