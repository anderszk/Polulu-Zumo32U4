#include <Wire.h>
#include "battery.h"
#include <EEPROM.h>


//______VARIABLER TILKNYTTET EEPROM 
int adresse4 = 4; //BatteryLife 
int adresse1 = 1; //BatteryHealth (Problem: batteryHealth = 100 - (5*batteryCycles); i GVar, trengs likevel adresse1?
int adresse2 = 2; //BatteryCycles
int adresse3 = 3; //account_balance_zumo

long batteryLife_stored;
int batteryHealth_stored; 
int batteryCycles_stored;
long balance_stored;
//________________________________


long ladebelop;
long maxBelop;
 
bool loop_func1;//For å lagre forrige tilstand til knapp
bool loop_func2;
bool loop_func3;
bool loop_func4;
bool emrgncy_charge_used = false;


long batteryMax = 807120L; //cpr_maxright + cpr_max_left pga ulike counts. (385020+422100)
long batteryLife = 807120; //403560=50% 645969=80% (for testing) 
long battery5percent = 40356L; //5 prosent av batteryMax(807120*0,05)
long battery15percent = 121068L; //15 prosent av batteryMax (807120*0,15)
float batteryPercent;


long account_balance_zumo; //Satt til 1000 nå for testing
int transferred_amount;


int batteryCycles = 0; //Batterisykluser
int batteryHealth = 100 - (5 * batteryCycles); //Batteryhealth
int level_0 = 5; //Kritisk nivå på batterylife
int level_1 = 50; //Lav batterylife

long batteryChargeAmount; //Variabel som holder hvor mye den skal legge til batteriet når ladefunksjonen kjøres

//For knappefunksjonene
unsigned long buttonAMillis;
unsigned long buttonAMillis2;
unsigned long buttonATid;
long buttonAPeriode = 0;
unsigned long buttonBMillis;
unsigned long buttonBMillis2;
unsigned long buttonBTid;
long buttonBPeriode = 0;

bool loopA;
bool loopB;
bool buttonAStopp;
bool buttonBStopp;

String btnA[] = {"Press", "for", "Press", "to check", "Press C", "Service/"}; //Print i void loop() rad 1
String btnA2[] = {"button A", "charging", "button B", "battery", "for", "Replace"}; //Print i void loop() rad 2
int buttonAlength = 5; //Lengde på listen (max-1 mhp 0)
int reset_A = 0; //Konstant for å resette counten til 0
int startA = 0; //Starter count på 0

String btnB[] = {"Press", "for", "Press B", "Account", "Press", "to go"}; //Print i void loop() rad 1
String btnB2[] = {"button A", "$$$", "to check", "balance", "button C", "back"}; //Print i void loop() rad 2
int buttonBlength = 5; //Lengde på listen (max-1 mhp 0)
int reset_B = 0; //Konstant for å resette counten til 0
int startB = 0; //Starter count på 0


//For batteri/bank
/*unsigned long bankMillis;
unsigned long bankMillis2;
unsigned long bankATid;
long bankPeriode = 0;
unsigned long batteryMillis;
unsigned long batteryMillis2;
unsigned long batteryTid;
long batteryPeriode = 0;

bool bankStopp;
bool bbatteryStopp;

String bank[] = {"Press", "for", "Press", "to check", "Press C", "Service/"}; //Print i void loop() rad 1
String bank2[] = {"button A", "charging", "button B", "battery", "for", "Replace"}; //Print i void loop() rad 2
int banklength = 5; //Lengde på listen (max-1 mhp 0)
int resetbank = 0; //Konstant for å resette counten til 0
int startbank = 0; //Starter count på 0

String battery[] = {"Press", "for", "Press B", "Account", "Press", "to go"}; //Print i void loop() rad 1
String battery2[] = {"button A", "$$$", "to check", "balance", "button C", "back"}; //Print i void loop() rad 2
int batterylength = 5; //Lengde på listen (max-1 mhp 0)
int resetbattery = 0; //Konstant for å resette counten til 0
int startbattery = 0; //Starter count på 0 */



//Funksjon for å oppdatere verdiene til EEPROM-minne
void updateEEPROM() {
EEPROM.put(adresse4, 0); //Reseter value
EEPROM.put(adresse4, batteryLife); //Oppdaterer value

EEPROM.put(adresse1, 0); //Reseter value
EEPROM.put(adresse1, batteryHealth); //Oppdaterer value

EEPROM.put(adresse2, 0); //Reseter value
EEPROM.put(adresse2, batteryCycles); //Oppdaterer value

EEPROM.put(adresse3, 0); //Reseter value
EEPROM.put(adresse3, account_balance_zumo); //Oppdaterer value
}


/*//Funksjon for å cleare EEPROM-minne
void clearEEPROM() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
  EEPROM.write(i, 0);
  }
  EEPROM.put(adresse1,100);
  //EEPROM.put(adresse4, 807120L);
} */


//Funksjon som sier ifra når det er lavt batterinivå på zumoen (Kjøres 1 gang)
void warning15() {
  if (batteryLife <= battery15percent) { //Varsler
      motors.setSpeeds(0, 0);
      buzzer.play(">g32>>c32");
      ledRed(1);

      print1sek("Battery", "low!");
      print1sek("Charge", "now!");
      
      ledRed(0);
      }
}


//Funksjin som simulerer lading og ladetid basert på hvor mye brukeren betalte
void lade() {  //Funksjon for lading av selve Zumo
    int i = 0; //Variabel som lagrer sekunder (delay-basert)
    long ladetidsekunder = ladebelop  /  5L; //5 Zumo coins = 1 sek =>
    batteryChargeAmount = (ladetidsekunder * 13452L); //Antall counts den skal legge til i batteriet
    batteryLife += batteryChargeAmount; //Legger til counts på batteriet
    EEPROM.put(adresse4, 0); //Oppdaterer batteryLife på EEPROM
    EEPROM.put(adresse4, batteryLife);
    batteryCycles += 1; //Legger til +1 på ladesyklus
    EEPROM.update(adresse2, batteryCycles); //Oppdaterer batteryCycles på EEPROM
    batteryHealth = (100 - (batteryCycles * 5)); //=> batteryHealth går ned med 5%
    EEPROM.update(adresse1, batteryHealth); //Oppdaterer batteryHealth på EEPROM
    
    loop_func3 = true; //Loop variabel som initialiserer while-løkken
        
    while (loop_func3 = true) { //Funksjon for å "simulere" lading siden
      int ladetidSanntid = ladetidsekunder - i; //Variabel som holder tid som er igjen av lading
      motors.setSpeeds(0, 0); //Stopp Zumoen og simuler "lading" selvom det er lagt til (For realistisk scenario)
      lcd.clear();
      lcd.gotoXY(0,0);
      lcd.print("Ladetid");
      lcd.gotoXY(0,1);
      lcd.print(ladetidSanntid); //Printer hvor mye som er igjen av lading
      
      delay(1000);
      i++;
      lcd.clear();
      lcd.gotoXY(0,0);
      lcd.print("Cycles:");
      lcd.gotoXY(0,1);
      lcd.print(batteryCycles); //Printer batteryCycles
      
      delay(1000);
      i++;
      lcd.clear();
      lcd.gotoXY(0,0);
      lcd.print("Cost:");
      lcd.gotoXY(0,1);
      lcd.print(ladebelop); //Printer ladebeløp
      
      delay(1000);
      i++;
      lcd.clear();
      lcd.gotoXY(0,0);
      lcd.print("Balance:");
      lcd.gotoXY(0,1);
      lcd.print(account_balance_zumo); //Prineter balanse på konto
      
      delay(1000);
      i++;
      
      if(i >= ladetidsekunder){ //Breaker når det har gått like mange sekunder som du har betalt for i ladebelop();
        loop_func3=false; //Kan muligens fjernes pga break;
        lcd.clear();
        lcd.gotoXY(0,0);
        lcd.print("Lading");
        lcd.gotoXY(0,1);
        lcd.print("ferdig");

        //Resetter verdiene
        i = 0;
        ladetidsekunder = 0;
        batteryChargeAmount = 0;
        break; //Breaker fra while-løkken
      }
   }
}



void lade_belop() { //funksjon som setter Zumo i modus for å tilføre penger ved knappetrykk
  
  ladebelop = 0; //reseter ladebeløp
  int x = 20;  //Beløpet som overføres ved knappetrykk A
  int y = 40;  //Beløpet som overføres ved knappetrykk B
  int z = 60; //Beløpet som overføres ved knappetrykk C
  loop_func1 = true; //Initialiserer while-løkken

  //Instruksjoner til bruker om knappefunksjoner
  print1sek("A+B når", "ferdig");

  print1sek("Knapp A", "=20 coins");

  print1sek("Knapp B", "=40 coins");

  print1sek("Knapp C", "=60 coins");

  
  //Her legger brukeren til penger :)
  while(loop_func1 = true){
    
    if((buttonA.isPressed() == true) && (buttonB.isPressed() != true) && (buttonC.isPressed() != true)){
      delay(1000);
      
      if (buttonA.isPressed() != true){
        ladebelop +=x;
        lcd.clear();
        lcd.gotoXY(0,0);
        lcd.print(ladebelop);
      }

      //Dersom man trykker på A og B samtidig vil den breake fra løkken
     else if ((buttonB.isPressed() == true) && (ladebelop <= account_balance_zumo) && (ladebelop <= maxBelop)) {
        buttonA.waitForRelease();
        buttonB.waitForRelease();

        print1sek("Går ut", "");
;
        account_balance_zumo -= ladebelop; //Trekker beløpet du har valgt fra kontoen
        EEPROM.put(adresse3, 0);
        EEPROM.put(adresse3, account_balance_zumo);
        
        print1sek("Starter", "Lading");
        delay(1000);
        
        lade();
        loop_func1 = false;
        break;
      }

     else if ((buttonB.isPressed() == true) && ((ladebelop >= account_balance_zumo) || (ladebelop >= maxBelop))){ //PROBLEM: vanskelig å registrere to knappetrykk samtidig, må være veldig nøyaktig.
        buttonA.waitForRelease();
        buttonB.waitForRelease();

        print1sek("Ikke nok", "penger"); 
        
        loop_func1 = false;
        break;
      }
    }

    if((buttonB.isPressed() == true) && (buttonA.isPressed() != true) && (buttonC.isPressed() != true)){
      buttonB.waitForRelease();
      ladebelop +=y;
      lcd.clear();
      lcd.gotoXY(0,0);
      lcd.print(ladebelop);
      }

    if((buttonC.isPressed() == true) && (buttonA.isPressed() != true) && (buttonC.isPressed() != true)){
      buttonC.waitForRelease();
      ladebelop +=z;
      lcd.clear();
      lcd.gotoXY(0,0);
      lcd.print(ladebelop);
      }
   }
}
 

//Funksjon som samler ladebelop() og lade()
void lademodus() { //Funksjon for lademodus på Zumo

  float healthFactor = batteryHealth / 100;
  
  print1sek("Velg", "beløp");
  delay(1000);
  
  //Regner ut hva maxbeløp du kan kjøpe strøm for, dette er for å forhindre overcharge
  //Denne utregningen tar hensyn til batteryHealth slik at man kan maks kan lade til nåværende batterikapasitet
  maxBelop = ((((batteryMax * healthFactor) - batteryLife) * 5) / 13452); //5kr per 13452 counts =>
  round(maxBelop); //Runder av maxBelop til et heltall
  
  lcd.clear();
  lcd.gotoXY(0,0);
  lcd.print("Max");
  lcd.gotoXY(0,1);
  lcd.print("Beløp:");
  delay(1000);  
  lcd.clear();
  lcd.gotoXY(0,0);
  lcd.print(maxBelop);
  
  delay(1000);
  
  lcd.clear();
  lcd.gotoXY(0,0);
  lcd.print("Balanse:");
  lcd.gotoXY(0,1);
  lcd.print(account_balance_zumo);
  
  delay(2000);
  
  lade_belop();
  
  maxBelop = 0;
}


//Funksjon for å vise hvor mye juice som er igjen på batteriet vist i %
void showBattery() {
  while (buttonB.isPressed() == true) {
    float percent = 807120 / 100; //Regner ut hvor mye 1% av batteriet er i counts
    batteryPercent = batteryLife / percent; //Deler nåværende batteryLife med prosent
    batteryPercent = round(batteryPercent); //Runder av tallet
   
    lcd.clear();
    lcd.gotoXY(0,0);
    lcd.print("Battery%");
    lcd.gotoXY(0,1);
    lcd.print(batteryPercent);
    delay(50);
  }
}


//Funksjon for service av batteri (Service booster % til 100% batteryhealth)
void serviceBattery() 
{
  int pris = 20; //Pris per % service
  int prosentmangel = 100 - batteryHealth; //Hvor mange prosent den skal ta service på
  int kostnad = pris * prosentmangel; //Hvor mye det koster

  //Sjekker om du har råd
  if (kostnad <= account_balance_zumo) 
  {
    account_balance_zumo -= kostnad; //Trekker fra summen fra bankkontoen din
    EEPROM.put(adresse3, 0); //Oppdaterer EEPROM
    EEPROM.put(adresse3, account_balance_zumo);
    
    batteryCycles = 0; //Reseter batteryCycles
    EEPROM.put(adresse2, 0);//Oppdaterer EEPROM
    EEPROM.put(adresse2, batteryCycles);
    batteryHealth = (100 - (batteryCycles * 5)); //BatteryHealth opp til 100%
    EEPROM.update(adresse1, batteryHealth); //Oppdaterer EEPROM
  
    print1sek("Battery", "Service");

    lcd.clear();
    lcd.gotoXY(0,0);
    lcd.print("% fixed:");
    lcd.gotoXY(0,1);
    lcd.print(prosentmangel); //Printer hvor mange prosent som er fikset
    
    delay(1000);
    lcd.clear();
    lcd.gotoXY(0,0);
    lcd.print("Price:");
    lcd.gotoXY(0,1);
    lcd.print(kostnad); //Printer kostnad for bruker
    
    delay(3000);
    
    buzzer.play(">g32>>c32"); //Spiller en liten melodi når den er ferdig med service
    
    print1sek("Service", "done");

    lcd.clear();
    lcd.gotoXY(0,0);
    lcd.print("Balance:");
    lcd.gotoXY(0,1);
    lcd.print(account_balance_zumo);
    
    delay(1000);
  }

  //Dersom du ikke har nok penger vil du få denne beskjeden og vil bli sendt tilbake til void loop()
  else if (account_balance_zumo < kostnad) 
  {
    lcd.clear();
    lcd.gotoXY(0,0);
    lcd.print("Ikke nok");
    lcd.gotoXY(0,1);
    lcd.print("penger");
    
    delay(2000);
  }
}


//Funksjon å bytte hele batteriet
void replaceBattery() {
  int kostnad_replace = 1500; //Dette er prisen vi har satt for å replace batteriet
  
  print1sek("Battery", "needs");
  
  print1sek("to be", "replaced");
  
  //Dersom du har nok penger vil batteriet bli byttet
  if (kostnad_replace <= account_balance_zumo)
  {
    account_balance_zumo -= kostnad_replace;
    EEPROM.put(adresse3, 0); //Update EEPROM
    EEPROM.put(adresse3, account_balance_zumo);
    
    batteryCycles = 0; //Reseter batterycycles
    EEPROM.put(adresse2, 0); //Update EEPROM
    EEPROM.put(adresse2, batteryCycles);
    batteryHealth = (100 - (batteryCycles * 5)); //reseter batteryHealth
    EEPROM.update(adresse1, batteryHealth); //Update EEPROM
    
    buzzer.play(">g32>>c32"); //Spiller av en melodi når den er ferdig
   
    print1sek("Cost:", "1500kr");
    
    print1sek("New", "Balance:");
    
    lcd.clear();
    lcd.gotoXY(0,0);
    lcd.print(account_balance_zumo);
    
    delay(2000);
  }

  //Hvis du ikke har nok penger vil du bli sendt tilbake til void loop()
  else if (account_balance_zumo < kostnad_replace)
  {
    print1sek("Ikke nok", "penger!");
  }
}

//Funksjon for å simulere uheldig batterifeil
void batteryDegrade() { //Randomfunksjon som degraderer batteriet til 50 (1:100 sjanse)
  int rn = random(1,100); //Ruller et random nummer fra 0-100

  if (rn == 13) { //Dersom nummeret er 13 (ulykkestall)
    
    //Sjekker om zumoen har mindre enn 50% på batteryhealth for å unngå - i batteryHealth
    if (batteryCycles > 10) {
      batteryCycles = 20; //Zumoen sin batteryHealth går ned med 50%
      batteryHealth = (100 - (batteryCycles * 5));
      updateEEPROM();//Oppdaterer EEPROM

      print1sek("Battery", "damaged!");

      delay(1000);
      
      lcd.clear();
      lcd.gotoXY(0,0);
      lcd.print("Health:");
      lcd.gotoXY(0,1);
      lcd.print(batteryHealth);
      
      delay(2000);
    }

    //Dersom batteriet har over 50%< vil den gå ned med 50% av maxkapasitet
    else if (batteryCycles <= 10) {
    batteryCycles += 10; //Zumoen sin batteryHealth går ned med 50%
    batteryHealth = (100 - (batteryCycles * 5));
    updateEEPROM();

    print1sek("Battery", "damaged!");
    
    delay(1000);
    
    lcd.clear();
    lcd.gotoXY(0,0);
    lcd.print("Health:");
    lcd.gotoXY(0,1);
    lcd.print(batteryHealth);
    
    delay(2000);
  }
 }
}

/*______________________________________________________________________________________________________________________________________________________________________________________________________*/
//ZUMOBANK_________________

//Funksjon for å legge inn penger på kontoen
void innskuddsmodus() { //funksjon som setter Zumo i modus for å tilføre penger ved knappetrykk

  //Dette er hvor mye den vil legge inn på de respektive knappetrykkene
  int x = 20;  //Beløpet som overføres ved knappetrykk A
  int y = 40;  //Beløpet som overføres ved knappetrykk B
  int z = 60; //Beløpet som overføres ved knappetrykk C
  int periode = 30000; //Periode funksjonen lopper
  loop_func2 = true;

  print1sek("Velg", "Beløp");
  print1sek("A+B når", "ferdig");
  print1sek("Knapp A", "20 coins");
  print1sek("Knapp B", "40 coins");
  print1sek("Knapp C", "60 coins"); 
  
  while(loop_func2 = true){
    
     if((buttonA.isPressed() == true) && (buttonB.isPressed() != true) && (buttonC.isPressed() != true)){
      delay(1000);

 
    if (buttonA.isPressed() != true) {
      account_balance_zumo +=x; //Legger til 20kr dersom man trykker på A
      updateEEPROM(); //Oppdaterer EEPROM-verdiene
      
      lcd.clear();
      lcd.gotoXY(0,0);
      lcd.print(account_balance_zumo);
      
      }
      
    if ((buttonB.isPressed() == true)) { //Går ut dersom man trykker på A og B samtidig
      buttonA.waitForRelease();
      buttonB.waitForRelease();

      print1sek("Går ut", "av modus");
      
      loop_func2 = false;
      break;
     }
     }
     
     if((buttonA.isPressed() != true) && (buttonB.isPressed() == true) && (buttonC.isPressed() != true)){
      buttonB.waitForRelease();
      
      account_balance_zumo +=y; //Legger til 40 om man trykker på B
      updateEEPROM(); //Oppdaterer EEPROM-verdiene
      
      lcd.clear();
      lcd.gotoXY(0,0);
      lcd.print(account_balance_zumo);
      
      }

     if((buttonA.isPressed() != true) && (buttonB.isPressed() != true) && (buttonC.isPressed() == true)){
      buttonC.waitForRelease();
      
      account_balance_zumo +=z; //Legger til 60 dersom man trykker på C
      updateEEPROM(); //Oppdaterer EEPROM-verdiene
      
      lcd.clear();
      lcd.gotoXY(0,0);
      lcd.print(account_balance_zumo);      
      }
  }
}


//Dette er en funksjon for å vise balanse på konto
void showBalance() {
  while (buttonB.isPressed() == true) {
    lcd.clear();
    lcd.gotoXY(0,0);
    lcd.print("Balance:");
    lcd.gotoXY(0,1);
    lcd.print(account_balance_zumo);;
    delay(50);
  }
}


/*______________________________________________________________________________________________________________________________________________________________________________________________________*/
//FUNSKJON FOR PRINT
void print1sek(String x, String y) {
  lcd.clear();
  lcd.print(x);
  lcd.gotoXY(0,1);
  lcd.print(y);
  delay(1000);
}
/*______________________________________________________________________________________________________________________________________________________________________________________________________*/

//FUNKSJONER TILKNYTTET KNAPPER__________________

void ButtonA(){ //Samlefunksjon for lademodus
  loopA = true;
  while (loopA == true) {
    buttonAStopp = false;
    buttonAMillis = millis();
    buttonAPeriode = 1000;
    
    if (buttonA.getSingleDebouncedPress()) {
      delay(200);
      buttonAStopp = true;
      lademodus();
      break;
    }
    
    else if (buttonB.getSingleDebouncedPress()) {
      buttonAStopp = true;
      delay(200);
      lcd.clear();
      lcd.print("Hold B");
      
      if(buttonB.isPressed() == true) {
        showBattery();
        }
        break;
      }

     else if(buttonC.getSingleDebouncedPress()) {
        buttonAStopp = true;
        if (level_1 >= batteryHealth) {
            serviceBattery();
            break; 
            }
        else if (batteryHealth <= level_0) {
            replaceBattery();
            break;
            }
        else if (level_1 <= batteryHealth) {
            print1sek("Battery", "do not");
            print1sek("need", "service");
            break;
        }
      }
      else if (((buttonAMillis - buttonAMillis2) >= buttonATid + buttonAPeriode) && (buttonAStopp == false)) {
        lcd.clear(); //CLearer LCD-displayet
        lcd.print(btnA[startA]); //Printer til første rad
        lcd.gotoXY(0,1); //Går til andre rad 2
        lcd.print(btnA2[startA]); //Printer til andre rad
        buttonAMillis2 = buttonAMillis; //Lagrer millis til neste gang de sammenlignes
        buttonAPeriode += 1000; //Legger til 500ms på periode for å oppdatere
        startA += 1; //Går tik neste ord i arrayen
        if (startA > buttonAlength) startA = reset_A;
      }
    }
}

void ButtonB() { //Samlefunksjon for bankmodus
  loopB = true;
  while (loopB == true) {
    buttonBStopp = false;
    buttonBMillis = millis();
    buttonBPeriode = 1000;

    if (buttonA.getSingleDebouncedPress()) {
      delay(300);
      buttonBStopp = true;
      innskuddsmodus();
      
    }

    else if (buttonB.getSingleDebouncedPress()) {
      buttonBStopp = true;
      delay(300);
      lcd.clear();
      lcd.gotoXY(0,0);
      lcd.print("Hold B");
      
      if(buttonB.isPressed() == true) {
        showBalance();
       }
    }

    else if(buttonC.getSingleDebouncedPress()) {
      delay(300);
      buttonBStopp = true;
      lcd.clear();
      lcd.print("Går ut");
      lcd.gotoXY(0,1);
      lcd.print("av bank");
      break;
     }

    else if (((buttonBMillis - buttonBMillis2) >= buttonBTid + buttonBPeriode) && (buttonBStopp == false)) {
        lcd.clear(); //CLearer LCD-displayet
        lcd.print(btnB[startB]); //Printer til første rad
        lcd.gotoXY(0,1); //Går til andre rad 2
        lcd.print(btnB2[startB]); //Printer til andre rad
        buttonBMillis2 = buttonBMillis; //Lagrer millis til neste gang de sammenlignes
        buttonBPeriode += 1000; //Legger til 500ms på periode for å oppdatere
        startB += 1; //Går tik neste ord i arrayen
        if (startB > buttonBlength) startB = reset_B;
    }
  }
}
