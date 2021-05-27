//Inkluderer alle bibliotekene vi skal bruke

#include <Wire.h> //Init
#include <EEPROM.h>
#include <Zumo32U4.h> //Zumo bibliotek
#include "TurnSensor.h" //Tilleggsfiler for gyro
#include "battery.h" //Tillegsfiler for funksjonene til batteriet
#include "linefollower.h" //Tilleggsfunksjoner for linefollower
#include "kjoreFunk.h"
#include <math.h> //Mattebibliotek for lineFollower


/*______________________________________________________________________________________________________________________________________________________________________________________________________*/

//Instansering av Zumoelementer

Zumo32U4LCD lcd; //Ærklærer LCD-displayet til Zumoen for lcd
Zumo32U4Motors motors; //Ærklærer motoren til Zumoen for Motors
Zumo32U4Encoders encoders; //Ærklærer encoderne til Zumoen for encoders
Zumo32U4ButtonA buttonA; //Ærklærer knapp A til Zumoen for buttonA
Zumo32U4ButtonB buttonB; //Ærklærer knapp B til Zumoen for buttonB
Zumo32U4ButtonC buttonC; //Ærklærer knapp C til Zumoen for buttonC
Zumo32U4Buzzer buzzer; //Ærklærer buzzeren til Zumoen for buzzer
Zumo32U4LineSensors lineSensors; //Ærklærer frontarray-sensorene til Zumoen for lineSensors
L3G gyro; //Ærklærer gyroen til Zumoen for gyro


/*______________________________________________________________________________________________________________________________________________________________________________________________________*/

//Variabler for hovedfil

unsigned long printMillis; //Variabel for å lagre tiden i printfunksjonen
unsigned long printMillis2;
unsigned long printTid; //Variabel for å lagre total periode mht. millis()
long printPeriode = 0; //Perioden det skal ta før neste print kommer
unsigned long printPeriod;
unsigned long aMillis;
unsigned long aMillis2;
unsigned long aTid;
long aPeriode = 0;
unsigned long bMillis;
unsigned long bMillis2;
unsigned long bTid;
long bPeriode = 0;
unsigned long cMillis;
unsigned long cMillis2;
unsigned long cTid;
long cPeriode = 0;


bool printStopp;
bool aStopp;
bool bStopp;
bool cStopp;


/*______________________________________________________________________________________________________________________________________________________________________________________________________*/


//Arrays med prints

String print_instruction[] = {"A for", "B for", "and", "C for"}; //Print i void loop() rad 1
String print_instruction2[] = {"Autonom", "Slalom", "LnFollwr.", "Battery"}; //Print i void loop() rad 2
int instruction_length = 6; //Lengde på listen (max-1 mhp 0)
int resetPrint = 0; //Konstant for å resette counten til 0
int printStart = 0; //Starter count på 0

String printsub[] = {"A for", "B for", "C to"};


String printA[] = {"Square", "Circle", "tur/retu"}; //Print i void loop() rad 2
int a_length = 3; //Lengde på listen (max-1 mhp 0)
int resetA = 0; //Konstant for å resette counten til 0
int a = 0; //Starter count på 0


String printB[] = {"Slalom", "LnFollwr", "Go back"}; //Print i void loop() rad 2
int b_length = 3; //Lengde på listen (max-1 mhp 0)
int resetB = 0; //Konstant for å resette counten til 0
int b = 0; //Starter count på 0

String printC[] = {"Charging", "Bank", "Go back"}; //Print i void loop() rad 2
int c_length = 3; //Lengde på listen (max-1 mhp 0)
int resetC = 0; //Konstant for å resette counten til 0
int c = 0; //Starter count på 0

/*______________________________________________________________________________________________________________________________________________________________________________________________________*/

//clearEEPROM(); Denne kjøres i setup for å nullstille minnet til EEPROMen

//Dette inkluderes i void setup() når vi skal bruke EEPROM til å lagre verdier
/* 
  batteryLife_stored = EEPROM.get(adresse4, batteryLife);
  batteryHealth_stored = EEPROM.read(adresse1);
  batteryCycles_stored = EEPROM.read(adresse2);
  balance_stored = EEPROM.get(adresse3, account_balance_zumo);

  batteryLife = batteryLife_stored;
  account_balance_zumo = balance_stored;
  batteryCycles = batteryCycles_stored;
  batteryHealth = batteryHealth_stored;
*/

/*______________________________________________________________________________________________________________________________________________________________________________________________________*/

void setup() {
  
  randomSeed(analogRead(0)); //Leser av en tom pin for å få et pseudo-random tall.
  total_countRight = 0; //Reseter counts som er lagret på høyre
  total_countLeft = 0; //Reseter counts som er lagret på venstre
  countLeft = 0;
  countRight = 0;

  buzzer.play("!T240 L8 agafaea dac+adaea fa<aa<bac#a dac#adaea f4"); //Spiller Johan S. Bach's fugue in D-minor :)

  batteryLife_stored = EEPROM.get(adresse4, batteryLife);
  batteryHealth_stored = EEPROM.read(adresse1);
  batteryCycles_stored = EEPROM.read(adresse2);
  balance_stored = EEPROM.get(adresse3, account_balance_zumo);

  batteryLife = batteryLife_stored;
  account_balance_zumo = balance_stored;
  batteryCycles = batteryCycles_stored;
  batteryHealth = batteryHealth_stored;

  batteryHealth = 100;
  batteryCycles = 0;
  account_balance_zumo = 1000;
  batteryLife = batteryMax;
  updateEEPROM();

 
  print1sek("Press A", "to calib!");

  buttonA.waitForButton(); //Pauser all kode til knapp A blir trykket ned
  lineSensors.initFiveSensors(); //Initialiserer sensorene
  
  calibrateSensors(); //Funksjon for å kalibrere sensorer
  delay(500); //Delay på 500ms
  turnSensorSetup(); // kalibrerer sensoren
  delay(500); //Delay på 500ms

  batteryDegrade(); //Kaller på batteryDegrade for å simulerer produksjonsfeil i batteriet

  print1sek("Press A", "to start!");
  buttonA.waitForButton(); //Pauser all kode til knapp A blir trykket ned
  delay(500);

}


/*______________________________________________________________________________________________________________________________________________________________________________________________________*/


void loop() {
  printStopp = false;
  printMillis = millis(); //Starter millis();
  printPeriode = 1000;
    
/*______________________________________________________________________________________________________________________________________________________________________________________________________*/
//Knapp A, Autonom kjøring

  if (buttonA.getSingleDebouncedPress()) { //Hvis knapp A blir trykket på kan du velge mellom autonom kjøring, linefollower og sensorshow
    delay(300);
    printStopp = true;
    while (printStopp != false) { //Loop
      aStopp = false; //Stopper printen
      aMillis = millis();
      aPeriode = 1000;

      //Kjører i et kvadrat
      if (buttonA.getSingleDebouncedPress()) { //Når du trykker på a vil du gå inn på lademodus
        aStopp = true; //Stopper printen
        x = 0;
        kvadrat();
        warning15(); //Varsler brukeren dersom batteriet er på under 15%
        showStats(); //Viser batteryLife (i %) og distanse kjørt (i cm)
        break; //Breaker fra while-løkken
      }

      //Kjører i sirkel
      else if (buttonB.getSingleDebouncedPress()) { //Når du trykker på b vil du gå inn på bankmodus
        aStopp = true; //Stopper printen
        x = 0;
        sirkel();
        warning15(); //Varsler brukeren dersom batteriet er på under 15%
        showStats(); //Viser batteryLife (i %) og distanse kjørt (i cm)
        break; //Breaker fra while-løkken
      }

      //Kjører frem og tilbake
      else if (buttonC.getSingleDebouncedPress()) { //Når du trykker på c vil du breake tilbake til void loop()
        aStopp = true;
        x = 0;
        fremogtilbake();
        warning15(); //Varsler brukeren dersom batteriet er på under 15%
        showStats(); //Viser batteryLife (i %) og distanse kjørt (i cm)
        break; //Breaker fra while-løkken
      }

      //Printer instruksjoner
      else if (((aMillis - aMillis2) >= aTid + aPeriode) && (aStopp != true)) {//Hvert 0,5s skal dette skje:
      lcd.clear(); //CLearer LCD-displayet
      lcd.print(printsub[a]); //Printer til første rad
      lcd.gotoXY(0,1); //Går til andre rad 2
      lcd.print(printA[a]); //Printer til andre rad
      aMillis2 = aMillis; //Lagrer millis til neste gang de sammenlignes
      aPeriode += 1000; //Legger til 500ms på periode for å oppdatere
      a += 1; //Går tik neste ord i arrayen
      if (a > a_length) a = resetA;
      }
    }
  }


/*______________________________________________________________________________________________________________________________________________________________________________________________________*/
//Knapp B, Slalom og linefollower

  else if (buttonB.getSingleDebouncedPress()) { //Hvis knapp B blir trykket på så går du inn i slalommodus
    delay(300);
    printStopp = true;
    while (printStopp != false) { //Loop
      bStopp = false; //Stopper printen
      bMillis = millis();
      bPeriode = 1000;

      //Slalom
      if (buttonA.getSingleDebouncedPress()) { //Når du trykker på a vil du gå inn på lademodus
        bStopp = true; //Stopper printen
        //Funksjon for slalom
        warning15(); //Varsler brukeren dersom batteriet er på under 15%
        showStats(); //Viser batteryLife (i %) og distanse kjørt (i cm)
      }

      //Linefollower
      else if (buttonB.getSingleDebouncedPress()) { //Når du trykker på b vil du gå inn på bankmodus
        bStopp = true; //Stopper printen
        linefollowerprogram();
        warning15(); //Varsler brukeren dersom batteriet er på under 15%
        showStats(); //Viser batteryLife (i %) og distanse kjørt (i cm)
      }

      //Tilbake-funksjon til void loop()
      else if (buttonC.getSingleDebouncedPress()) { //Når du trykker på c vil du breake tilbake til void loop()
        delay(200);
        bStopp = true;
        print1sek("Går til", "Voidloop");
        break; //Breaker fra while-løkken
      }

      //Printer instruksjoner
      else if (((bMillis - bMillis2) >= bTid + bPeriode) && (bStopp != true)) {//Hvert 0,5s skal dette skje:
      lcd.clear(); //CLearer LCD-displayet
      lcd.print(printsub[b]); //Printer til første rad
      lcd.gotoXY(0,1); //Går til andre rad 2
      lcd.print(printB[b]); //Printer til andre rad
      bMillis2 = bMillis; //Lagrer millis til neste gang de sammenlignes
      bPeriode += 1000; //Legger til 500ms på periode for å oppdatere
      b += 1; //Går tik neste ord i arrayen
      if (b > b_length) b = resetB;
      }
    }
  }


/*______________________________________________________________________________________________________________________________________________________________________________________________________*/
//Knapp C, Lade/Bank

 else if (buttonC.getSingleDebouncedPress()) { //Hvis knapp C blir trykket på så går du inn i batteri og bank
    delay(300);
    printStopp = true;
    while (printStopp != false) { //Loop
      cStopp = false; //Stopper printen
      cMillis = millis();
      cPeriode = 1000;

      //Batterimodus
      if (buttonA.getSingleDebouncedPress()) { //Når du trykker på a vil du gå inn på lademodus
        cStopp = true; //Stopper printen
        delay(200);
        ButtonA(); //Samlefunksjon for batteri
      }

      //Bankmodus
      else if (buttonB.getSingleDebouncedPress()) { //Når du trykker på b vil du gå inn på bankmodus
        cStopp = true; //Stopper printen
        delay(200);
        ButtonB(); //Samlefunksjon for bank
      }

      //Tilbake-funksjon til void loop()
      else if (buttonC.getSingleDebouncedPress()) { //Når du trykker på c vil du breake tilbake til void loop()
        cStopp = true;
        delay(200);
        
        print1sek("Går til", "Voidloop");
        break; //Breaker fra while-løkken
      }

      //Printer instruksjoner
      else if (((cMillis - cMillis2) >= cTid + cPeriode) && (cStopp == false)) {//Hvert 0,5s skal dette skje:
      lcd.clear(); //CLearer LCD-displayet
      lcd.print(printsub[c]); //Printer til første rad
      lcd.gotoXY(0,1); //Går til andre rad 2
      lcd.print(printC[c]); //Printer til andre rad
      cMillis2 = cMillis; //Lagrer millis til neste gang de sammenlignes
      cPeriode += 1000; //Legger til 500ms på periode for å oppdatere
      c += 1; //Går tik neste ord i arrayen
      if (c > c_length) c = resetC;
      }
    }
  }


/*______________________________________________________________________________________________________________________________________________________________________________________________________*/
//Instruksjoner for A, B og C

   else if (((printMillis - printMillis2) >= printTid + printPeriode) && (printStopp != true)) { //I intervaller på 0,5 sek vil dette skje
    lcd.clear(); //CLearer LCD-displayet
    lcd.print(print_instruction[printStart]); //Printer til første rad
    lcd.gotoXY(0,1); //Går til andre rad 2
    lcd.print(print_instruction2[printStart]); //Printer til andre rad
    printMillis2 = printMillis; //Lagrer millis til neste gang de sammenlignes
    printPeriode += 1000; //Legger til 500ms på periode for å oppdatere
    printStart += 1; //Går tik neste ord i arrayen
    if (printStart > instruction_length) printStart = resetPrint; //Starten fra 0 i array når siste ordet er printet i begge arrayene
  }
}
