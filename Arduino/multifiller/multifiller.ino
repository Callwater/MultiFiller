#include <EEPROM.h>

///// Define pins ////

//Calibrate - Time how long it takes to fill the can
#define CalibrateButton 30 //Atmega328p-pu pin xx

//Define values
int CanFillUpTimeCalibrated = 0;
int CanFillUpTime = 0; // 10 seconds 
int Calibratedms = 0;
int CountDelayValueMs = 500; //should not be lower than 250
int countButtonPress = 0;
int Co2PurgeBeforeDelay = 2500; //2,5 seconds
int Co2PurgeAfterCountLimit = 8; // on/off 8 times
int Co2PurgeAfterCount = 0; // Counter

//Controller
#define BeerFiller1Button 31 //Atmega328p-pu pin xx - Only Filler #1
#define BeerFiller12Button 32 //Atmega328p-pu pin xx - Run both fillers
#define EmergencyShutDownButton 33 //Atmega328p-pu pin xx - Stop everything

//Filler 1
#define BeerValve1 22 //Atmega328p-pu pin xx
#define GasValve1 23 //Atmega328p-pu pin xx
//Filler 2
#define BeerValve2 24 //Atmega328p-pu pin xx
#define GasValve2 25 //Atmega328p-pu pin xx



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Startup...");
 
  //CalibrateButton
  pinMode(CalibrateButton,INPUT);
  //Controller
  pinMode(BeerFiller1Button,INPUT);
  pinMode(BeerFiller12Button,INPUT);
  pinMode(EmergencyShutDownButton,INPUT);
  
  //Filler 1
  pinMode(BeerValve1,OUTPUT);
  pinMode(GasValve1,OUTPUT);
  //Filler 2
  pinMode(BeerValve2,OUTPUT);
  pinMode(GasValve2,OUTPUT);

  Serial.println("Startup complete!");
  Serial.println("Waiting for input!");
  Serial.println("..."); 
}

void loop() {
  // put your main code here, to run repeatedly:

  // CalibrateButton
  if (digitalRead(CalibrateButton) == HIGH) {
    Serial.println("CalibrateButton pressed");
    CalibrateButtonFunction();
    delay(250);
  }

  // BeerFiller1Button
  if (digitalRead(BeerFiller1Button) == HIGH) {
    Serial.println("BeerFiller1Button pressed");
    BeerFiller1ButtonFunction();
    delay(250);
  }

  // BeerFiller12Button
  if (digitalRead(BeerFiller12Button) == HIGH) {
    Serial.println("BeerFiller12Button pressed");
    BeerFiller12ButtonFunction();
    delay(25);
  }  

  // EmergencyShutDownButton
  if (digitalRead(EmergencyShutDownButton) == HIGH) {
    Serial.println("EmergencyShutDownButton pressed");
    EmergencyShutDownButtonFunction();
    delay(25);
  }  

  delay(100);
}

//Calibrate the time it takes to fill a can
void CalibrateButtonFunction(){
    int CanFillUpTimeCalibrated = 0;
    Serial.println("Calibrating");
    while(digitalRead(CalibrateButton) == HIGH){
      Serial.print(".");
      //Start filling beer
      digitalWrite(BeerValve1,HIGH);
      delay(CountDelayValueMs);
      CanFillUpTimeCalibrated++;
    }
    //Stop filling beer
    digitalWrite(BeerValve1,LOW);
    //Set new time
    int CanFillUpTime = CanFillUpTimeCalibrated; //CanFillUpTimeCalibrated * 500 = 0,5 second (1 * 0,5)
    //Write to EEPROM
    Serial.println("Saving to EEPROM..." + String(CanFillUpTimeCalibrated));
    EEPROM.update(0, CanFillUpTime);
    Serial.println("Saved");
    int CanFillUpTimeRead = EEPROM.read(0);
    Serial.println("ms: " + String(CanFillUpTimeRead));
    String StrCanFillUpTime = "Calibrated CanFillUpTime (ms):" + String(CanFillUpTime);
    Serial.println(StrCanFillUpTime);
    //return CanFillUpTime;
}

//GasValve1 - Co2 Co2 Purge Before filling
void GasValve1Co2PurgeBefore(){
  Serial.println("GasValve1Co2PurgeBefore");
   digitalWrite(GasValve1,HIGH);
   delay(Co2PurgeBeforeDelay);
   digitalWrite(GasValve1,LOW);
}

//GasValve2 - Co2 Co2 Purge Before filling
void GasValve2Co2PurgeBefore(){
  Serial.println("GasValve2Co2PurgeBefore");
   digitalWrite(GasValve2,HIGH);
   delay(Co2PurgeBeforeDelay);
   digitalWrite(GasValve2,LOW);
}

//GasValves - Co2 Co2 Purge Before filling
void GasValvesCo2PurgeBefore(){
  Serial.println("GasValvesCo2PurgeBefore");
    digitalWrite(GasValve1,HIGH);
    digitalWrite(GasValve2,HIGH);
    delay(Co2PurgeBeforeDelay);
    digitalWrite(GasValve1,LOW);
    digitalWrite(GasValve2,LOW);
}

//BeerValve1 - Fill can
void BeerValve1FillCan(){
    int CanFillUpTime = EEPROM.read(0);
    Serial.println("Start - Beer Valve 1 Fill Can (CountDelayValueMs * " + String(CanFillUpTime) +")");
    digitalWrite(BeerValve1,HIGH);

   int LoopCount = 0;
   long LoopCountMs = 0;
   while(LoopCount <= CanFillUpTime){
    delay(CountDelayValueMs);
    LoopCount++;
    LoopCountMs = LoopCount * CountDelayValueMs;
    Serial.println("Current fill time (ms): " + String(LoopCountMs));
   }
   //delay(CanFillUpTime);
   digitalWrite(BeerValve1,LOW);
   Serial.println("End - Beer Valve 1 Fill Can (ms): " + String(LoopCountMs));
}

//BeerValve2 - Fill can
void BeerValve2FillCan(){
  Serial.println("BeerValve2FillCan");
   digitalWrite(BeerValve2,HIGH);
   delay(CanFillUpTime);
   digitalWrite(BeerValve2,LOW);
}

//BeerValve both - Fill cans
void BeerValveFillCans(){
  Serial.println("BeerValveFillCans");
   digitalWrite(BeerValve1,HIGH);
   digitalWrite(BeerValve2,HIGH);
   delay(CanFillUpTime);
   digitalWrite(BeerValve1,LOW);
   digitalWrite(BeerValve2,LOW);
}



//GasValve1 - Co2 Co2 Purge after filling
void GasValve1Co2PurgeAfter(){
  Serial.println("GasValve1Co2PurgeAfter");
  while(Co2PurgeAfterCount <= Co2PurgeAfterCountLimit){
   Serial.println("+");
   digitalWrite(GasValve1,HIGH);
   delay(25);
   digitalWrite(GasValve1,LOW);
   delay(25);
   Co2PurgeAfterCount++;
  }
  Co2PurgeAfterCount = 0;
  //Safety
  digitalWrite(GasValve1,LOW);
}

//GasValve2 - Co2 Co2 Purge after filling
void GasValve2Co2PurgeAfter(){
  Serial.println("GasValve2Co2PurgeAfter");
  while(Co2PurgeAfterCount <= Co2PurgeAfterCountLimit){
    Serial.println("+");
   digitalWrite(GasValve2,HIGH);
   delay(25);
   digitalWrite(GasValve2,LOW);
   delay(25);
   Co2PurgeAfterCount++;
  }
  Co2PurgeAfterCount = 0;
  //Safety
  digitalWrite(GasValve2,LOW);
}

//GasValves - Co2 Co2 Purge after filling
void GasValvesCo2PurgeAfter(){
  Serial.println("GasValvesCo2PurgeAfter");
  while(Co2PurgeAfterCount <= Co2PurgeAfterCountLimit){
    Serial.println("+");
    digitalWrite(GasValve1,HIGH);
   digitalWrite(GasValve2,HIGH);
   delay(25);
   digitalWrite(GasValve1,LOW);
   digitalWrite(GasValve2,LOW);
   delay(25);
   Co2PurgeAfterCount++;
   }
  
  Co2PurgeAfterCount = 0;
  //Safety
  digitalWrite(GasValve1,LOW);
  digitalWrite(GasValve2,LOW);
}

//Softstop - Just stop everything to be sure
void SoftStopFunction(){
  Serial.println("SoftStopFunction");
    digitalWrite(GasValve1,LOW);
    digitalWrite(GasValve2,LOW);
    digitalWrite(BeerValve1,LOW);
    digitalWrite(BeerValve2,LOW);
}

//Emergency Shutdown
void EmergencyShutDownButtonFunction(){
  Serial.println("EmergencyShutDownButtonFunction");
  while(digitalRead(EmergencyShutDownButton) == HIGH){
    Serial.println("+");
    digitalWrite(GasValve1,LOW);
    digitalWrite(GasValve2,LOW);
    digitalWrite(BeerValve1,LOW);
    digitalWrite(BeerValve2,LOW);
  }
}


//Run first beerfiller
void BeerFiller1ButtonFunction(){
  Serial.println("BeerFiller 1 Button Function");
  GasValve1Co2PurgeBefore();
  delay(25);
  BeerValve1FillCan();
  delay(25);
  GasValve1Co2PurgeAfter();
  delay(25);
  SoftStopFunction();
}

//Run both fillers
void BeerFiller12ButtonFunction(){
  Serial.println("BeerFiller 1-2 Button Function");
  GasValvesCo2PurgeBefore();
  delay(25);
  BeerValveFillCans();
  delay(25);
  GasValvesCo2PurgeAfter();
  delay(25);
  SoftStopFunction();
}
