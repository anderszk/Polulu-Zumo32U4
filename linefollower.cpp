#include <Wire.h>
#include "TurnSensor.h"
#include "linefollower.h"
#include "battery.h" //Tillegsfiler for funksjonene til batteriet
#include "kjoreFunk.h"
#include <EEPROM.h>
#include <math.h> //Mattebibliotek for lineFollower


#define NUM_SENSORS 5 //Antall sensorer
unsigned int lineSensorValues[NUM_SENSORS]; //Values av antall sensorer

const uint16_t sensorThreshold = 250;
int Turnspeed = 150;
int Leftspeed = 250; //Konstant leftspeed på 200
int Rightspeed = 231; //Konstant rightspeed på 186 grunnet feil på Zumo

int antall_kjegler = 4;

int intersection = 0;
unsigned long currentmillis;
unsigned long previousmillis;
unsigned long millis1;
unsigned long millis2;
unsigned long tid;
unsigned int periode;
unsigned int s;
unsigned int x;
unsigned int kjegle;
int period = 0;
bool loop1 = false;
bool i;
bool ferdig;
bool input_ferdig;
bool fortsett1;
bool fortsett2;
bool buttonApressed;
bool buttonBpressed; 

/* String distance_instruction[] = {"Press", "A,B or C", "to", "assign", "value to", "avstand.", "A=1000mm", "B =100mm", "C = 10mm", "Press", "button A", "to begin.", "You have", "10 seconds"};
int distance_count = 18;
int distance = 0; */




void kvadrat()
{
  while( x < 4) // while løkke som får zumoen til å kjøre rett fram i 1 sekund, og snu til venstre like mange ganger som det er antall sider i et kvadrat.
  {
  x += 1;
  motors.setSpeeds(Leftspeed, Rightspeed);
  countLeft = encoders.getCountsAndResetLeft();
  countRight = encoders.getCountsAndResetRight();
  delay(1000);
  Turnleft();
  stopp();
  }
}



void sirkel() // funksjoen som får bilen til å kjøre i sikrel, gyroscopet vil aldri nå er verdi = 180, så vi nullstiller gyroscopet for hver gang den har nådd 90 grader( kvartsirkel)
{
  while (x < 4)
  {
    x += 1;
  turnSensorReset();
  motors.setSpeeds(Leftspeed/2, Rightspeed);
  countLeft = encoders.getCountsAndResetLeft();
  countRight = encoders.getCountsAndResetRight();
    while((int32_t) turnAngle < turnAngle90)
    {
      delay(1);
      turnSensorUpdate();
    }
  utladning();
  distanse();
}
}



void fremogtilbake() // funksjon som får bilen til å kjøre rett fram i 1 sekung før den snur til venstre 2 ganger (90 + 90 = 180). Dette vil den gjøre 2 ganger, det vil si fram og tilbake.
{
  while( x < 2)
  {
    x += 1;
  motors.setSpeeds(Leftspeed, Rightspeed);
  countLeft = encoders.getCountsAndResetLeft();
  countRight = encoders.getCountsAndResetRight();
  delay (1000);
  distanse();
  utladning();
  stopp();
  Turnleft();
  Turnleft();
  distanse();
  utladning();
}
}



void calibrateSensors() //Program som kalibrerer sensorene til Zumoen, Dette vil ikke påvirke batteryLife
{ 
  ledYellow(1); //Kalibrering startet, skrur på gul LED.

  // Wait 1 second and then begin automatic sensor calibration
  // by rotating in place to sweep the sensors over the line
  delay(1000);
  for(uint16_t i = 0; i < 120; i++)
  {
    if (i > 30 && i <= 90)
    {
      motors.setSpeeds(-200, 200); //Snurrer rundt sin egen akse mot klokken
    }
    else
    {
      motors.setSpeeds(200, -200); //Snurrer rundt sin egen akse med klokken
    }

    lineSensors.calibrate(); //Kalibrerer sensorene
  }
  motors.setSpeeds(0, 0); //Stopper Zumoen
  ledYellow(0); //Kalibrering ferdig, skrur av gul LED
}


//PID-regulert linefollower
void lineFollower() { //Mainfunksjon for lineFollower
  unsigned int last_proportional=0; //Variabel for lagring av verdi
  long integral=0; //Variabel for lagring av verdi
  
  int16_t position = lineSensors.readLine(lineSensorValues); //Leser av verdiene til sensorene mellom 0-4000
  int16_t proportional = position - 2000; //For at Zumoen skal ha 0 som midtpunkt må man subtrahere 2000 fra den leste verdien
  
  int derivative = proportional - last_proportional; //Differanse
  integral += proportional; //Lagrer verdien
 
  last_proportional = proportional; //Lagrer verdien i en variabel
  
  int power_difference = proportional/20 + integral/10000 + derivative*3/2; //Konstantene vi brukte
  
  const int max1 = 300; //MaxSpeed til Zumoen
    
  if(power_difference > max1) {
    power_difference = max1;
  }
    
  if(power_difference < -max1) {
    power_difference = -max1;
  }
    
  if(power_difference < 0) {
    motors.setSpeeds(max1+power_difference, max1);
    countLeft = encoders.getCountsAndResetLeft();
    countRight = encoders.getCountsAndResetRight();
  }
    
  else {
    motors.setSpeeds(max1, max1-power_difference);
    countLeft = encoders.getCountsAndResetLeft();
    countRight = encoders.getCountsAndResetRight();
  }
  utladning();
  distanse();
  warning15();
}


//Funksjon som sammenligner verdien mot en treshhold for å avgjøre om den er av eller på linjen
bool aboveLine(uint8_t sensorIndex)
{
  return lineSensorValues[sensorIndex] > sensorThreshold;
}


//Funksjon som stopper bilen
void stopp()
{
  motors.setSpeeds(0,0);
  delay(10);
}


//Zumoen svinger til venstre 90 grader
void Turnleft()
{
  turnSensorReset();
  motors.setSpeeds(-Turnspeed, Turnspeed);
  while((int32_t) turnAngle < turnAngle90)
  {
    delay(1);
    turnSensorUpdate();
  }
  stopp();
}


//Zumoen svinger til høyre 90 grader
void Turnright()
{
  turnSensorReset();
  motors.setSpeeds(Turnspeed, -Turnspeed);
  while((int32_t) turnAngle > -turnAngle90)
  {
    delay(1);
    turnSensorUpdate();
  }
  stopp();
}



void FoundLeft_intersectioncheck()
{
    motors.setSpeeds(Leftspeed,Rightspeed);
    delay(150);
    stopp();
    lineSensors.readLine(lineSensorValues);
    if (intersection > 0)
    {
     intersection = 0;
     stopp();
     Turnleft();
     stopp();
     motors.setSpeeds(Leftspeed,Rightspeed);
     delay(50);
     distanse();
     utladning();
    }
    else if (!aboveLine(0) && !aboveLine(1) && !aboveLine(2) && !aboveLine(3) && !aboveLine(4))
    {
      stopp();
      Turnleft();
      stopp();
      motors.setSpeeds(Leftspeed, Rightspeed);
      int periode = 1000;
      while(currentmillis < tid + periode)
      {
        currentmillis = millis();
      lineSensors.readLine(lineSensorValues);
      if ( aboveLine(1) || aboveLine(2) || aboveLine(3)) 
      {
        currentmillis = 0;
        break;
      }
      }
    }
    else if (aboveLine(1) || aboveLine(2) || aboveLine(3)) 
    { 
      intersection +=1;
    }
}



void Foundright_intersectioncheck()
{
    motors.setSpeeds(Leftspeed, Rightspeed);
    delay(150);
    stopp();
    lineSensors.readLine(lineSensorValues);
    if (intersection > 0) 
    {
     intersection = 0;
     stopp();
     Turnright();
     stopp();
     motors.setSpeeds(Leftspeed,Rightspeed);
     delay(50);
    }
    else if (!aboveLine(0) && !aboveLine(1) && !aboveLine(2) && !aboveLine(3) && !aboveLine(4))
    {
      stopp();
      Turnright();
      stopp();
      motors.setSpeeds(Leftspeed, Rightspeed);
      int periode = 1000;
      while(currentmillis < tid + periode)
      {
      currentmillis = millis();
      lineSensors.readLine(lineSensorValues);
      if ( aboveLine(1) || aboveLine(2) || aboveLine(3)) 
      {
        currentmillis = 0;
        break;
      }
      }
    }
    else if (aboveLine(1) || aboveLine(2) || aboveLine(3)) 
    { 
      intersection +=1;
    }
}



void Deadendcheck()
{
  motors.setSpeeds(Leftspeed, Rightspeed);
  delay(500);
  utladning();
  distanse();
  lineSensors.readLine(lineSensorValues);
  if (!aboveLine(0) && !aboveLine(1) && !aboveLine(2) && !aboveLine(3) && !aboveLine(4)) // dersom det er en blindvei, skal den snu 180 grader, og kjøre tilbake til der den sist var på teipen
  {
    stopp();
    Turnleft();
    Turnleft();
    stopp();
    motors.setSpeeds(Leftspeed, Rightspeed);
    delay(500);
  }
  
}






//Samleprogram for linefollower
void linefollowerprogram()
{
  loop1 = true;

  while (loop1 == true) {
    lineSensors.readLine(lineSensorValues);
    lineFollower();
    
    if (aboveLine(0)) // dersom venstre sensor er på teipen skal den sjekke om det er et kryss
      {
        FoundLeft_intersectioncheck();
      }
    else if (aboveLine(4)) // dersom høyre sensor er på teipen skal den sjekke om det er et kryss
      {
        Foundright_intersectioncheck();
      }
      
    else if (!aboveLine(0) && !aboveLine(1) && !aboveLine(2) && !aboveLine(3) && !aboveLine(4)) // dersom ingen sensorerer er på teipen, skal bilen skjekke om det er en blindvei eller manglende teip
      {
        Deadendcheck(); // kaller på en funskjon som sjekker om det er en blindvei
      }
    else if (buttonB.isPressed()) {
      print1sek("Going","out");
      loop1 = false;
      break;
    }
  }
}
