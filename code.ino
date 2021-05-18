#include "Wire.h"                                 // library for I2C
#include "LiquidCrystal_I2C.h"                    // library for LCD
#include "GyverEncoder.h"                         // library for encoder
#include <OneWire.h>                              // library for connecting
#include <DallasTemperature.h>                    // library for ds18b20

//Pins

#define CLK       2
#define DT        3
#define ds18b20   4                         
#define Led       5                        
#define RELAY     6                         
#define SW        7                          

int newPos = 0, lastPos = 0, WaterTemp = 25, FinalTemp, led = 0;  // variable
boolean buttonWasUp = true, heater = 0;   
byte w = 0;
float Temp;

LiquidCrystal_I2C LCD(0x3F, 16, 2);   // name of display
Encoder enc1(CLK, DT, SW);            // for work with encoder 

OneWire oneWire(ds18b20);             // connecting ds18b20
DallasTemperature sensors(&oneWire);       //

void setup() {
  LCD.init();            // start of the display
  LCD.backlight();       // turning on backlight 
  Serial.begin(9600);    // starting of ports
  enc1.setType(TYPE1);   // choosing the type of encoder
  attachInterrupt(0, isrCLK, CHANGE);    // debuging  CLK 
  attachInterrupt(1, isrDT, CHANGE);     // debuging DT
  sensors.begin();  // Start up the library

  pinMode(SW, INPUT_PULLUP);             // pin positions 
  pinMode(RELAY, OUTPUT);                //
  pinMode(led, OUTPUT);                  //
  digitalWrite(RELAY, LOW);             //
}

void isrCLK() { enc1.tick(); }             // start of debuging CLK
void isrDT() { enc1.tick(); }              // start of debuging DT

void loop() {
while (w ==0){
  sensors.requestTemperatures(); 

    LCD.setCursor(0, 0);                   // printing on the display word "WaterTemp" in position 0, 0
    LCD.print("WaterTemp ");
    LCD.setCursor(10, 0);                  // printing on the display variable value of temperature in position 11, 0 
    LCD.print(sensors.getTempCByIndex(0));
    LCD.setCursor(15, 0);                  // printing on the display word "C" in position 14, 0
    LCD.print("C");

  if (enc1.isRight()) {newPos++;}          // add one if encoder turnig right
  if (enc1.isLeft()) {newPos--; }          // take one if encoder turnig left
 
   if (lastPos != newPos) {                // changing value on the LCD if turnig encoder                          //
     LCD.setCursor(0, 1);                
     LCD.print("ChooseT ");
     LCD.setCursor(11, 1);                 //
     LCD.print(WaterTemp + newPos);        // showing new value on display
     lastPos=newPos;                       // remember new position of encoder
   }

   
    boolean buttonIsUp = digitalRead(SW);  // read the encoder button 
    if (buttonWasUp && !buttonIsUp) {      //
      delay(50);                           // microcontroller wait 50ms before next step
      buttonIsUp = digitalRead(SW);         //         
    }
      if (!buttonIsUp)  { FinalTemp = WaterTemp + newPos; heater = 1; w = 2; LCD.clear(); } // remebmer choosen temerature


      while (w == 2) {
    Temp = sensors.getTempCByIndex(0);
    sensors.requestTemperatures(); 
    LCD.setCursor(5, 0);                   // printing on the display word "WaterTemp" in position 0, 0
    LCD.print("Heating ");
    LCD.setCursor(2, 1);                  // printing on the display variable value of temperature in position 11, 0 
    LCD.print(sensors.getTempCByIndex(0));
    LCD.setCursor(7, 1);                   // printing on the display word "WaterTemp" in position 0, 0
    LCD.print(" over"); 
    LCD.setCursor(13, 1);                  // printing on the display word "C" in position 14, 0
    LCD.print(FinalTemp);
     
         digitalWrite(RELAY, HIGH);             // turning on heater element                              // 
         if (Temp > FinalTemp)  {digitalWrite(RELAY, LOW); led = 1; w = 0; LCD.clear();}   // turning off heater element
      }
      
  /*    if (led == 1) { // checking the condition 
        led = 0;
        digitalWrite(led, HIGH);                 // led is turning on 
        delay(120000);                           // microcontroller waiting 120,000ms = 2 mins
        digitalWrite(led, LOW);                  // led is turning off
        }
    */  Serial.println(sensors.getTempCByIndex(0));
      }
}
      
