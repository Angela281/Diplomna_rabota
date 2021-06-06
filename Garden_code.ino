#include <RTClib.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include "DHT.h"
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h> 
 
  
#define DHTTYPE DHT11
#define dht_dpin 0
#define M1Enable D8
#define M2Enable D7
#define Ledpin D6
#define Sunlight D5
#define Sunleds D0


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(8, Ledpin, NEO_GRB + NEO_KHZ800);
DHT dht(dht_dpin, DHTTYPE); 
RTC_DS3231 rtc;


void DHT11_sensor();
void Light_sensor();
void Soil_sensor();
void Soil_sensor2();
void Water_level_sensor();
void RTC_clock();
void Sun_light_LED();

int WaterError = 0;
int Soilsensor1 = 3;
int Soilsensor2 = 3;
int tmpmin = 0;
int tmpsec = 0;
int motorstate = 0;
int suntime = 0;
int tmpsec2 = 0;
int motorstate2 = 0;
int count = 0;
int sunstate = 0;
int16_t Light, Soil1, Soil2, Water;

char daysOfTheWeek[7][12] = {"Sunday", "Monday",
"Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

Adafruit_ADS1015 ads;     


void setup(void) 
{  
  pinMode(M1Enable,OUTPUT);
  pinMode(M2Enable,OUTPUT);
  pinMode(Ledpin,OUTPUT);
  pinMode(Sunlight,OUTPUT);
  pinMode(Sunleds,OUTPUT);
  digitalWrite(M1Enable,0);
  digitalWrite(M2Enable,0);
  
  #ifndef ESP8266
  while (!Serial);
  #endif
  
  Serial.begin(9600);
  analogWrite(Sunlight, 900);
  
  Serial.println("Getting single-ended readings from AIN0..3");
  ads.begin();
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(__DATE__,__TIME__));
  dht.begin();
  Serial.println("Humidity and temperature\n\n");
  }
pixels.begin();
  pixels.setPixelColor(0, pixels.Color(255,0,0)); 
  for(int i = 1; i<8; i++){
     pixels.setPixelColor(i, pixels.Color(0,0,0)); 
          pixels.show();
  }
}
 
void loop(void) 
{
  DateTime now = rtc.now();
  
  Light = ads.readADC_SingleEnded(0);
  Soil1 = ads.readADC_SingleEnded(1);
  Soil2 = ads.readADC_SingleEnded(2);
  Water = ads.readADC_SingleEnded(3);
  Light_sensor();
  DHT11_sensor();
  Soil_sensor();
  Soil_sensor2();
  Water_level_sensor();
  Sun_light_LED();

  
  if(WaterError){
     Serial.println("WaterError");
     pixels.setPixelColor(5, pixels.Color(0,0,153)); 
     pixels.show();
  }
  else {
    pixels.setPixelColor(5, pixels.Color(0,0,0)); 
    pixels.show();
  }
     
  RTC_clock();

  switch(Soilsensor1)
  {
    case 0: 
      if(WaterError){
        digitalWrite(M1Enable,0);
        motorstate = 0;}
      else {
        if (motorstate == 0){
          tmpsec = millis();
          digitalWrite(M1Enable,1);
          
              pixels.setPixelColor(1, pixels.Color(0,255,0)); 
              pixels.show();

          motorstate = 1;
        }
        if( (millis() - tmpsec) >= 20000 ){
          digitalWrite(M1Enable,0);
          pixels.setPixelColor(1, pixels.Color(0,0,0)); 
          pixels.show();
           if((millis() - tmpsec) >= 35000)
            motorstate = 0;
        }
      } break;
      default:digitalWrite(M1Enable,0);
          pixels.setPixelColor(1, pixels.Color(0,0,0)); 
          pixels.show();
          motorstate = 0;
      break;
  }

   switch(Soilsensor2)
  {
    case 0: 
      if(WaterError){
        digitalWrite(M2Enable,0);
        motorstate2 = 0;}
      else {
        if (motorstate2 == 0){
          tmpsec2 = millis();
          digitalWrite(M2Enable,1);
          pixels.setPixelColor(2, pixels.Color(153,0,76)); 
          pixels.show();
          motorstate2 = 1;
        }
        if( (millis() - tmpsec2) >= 20000 ){
          digitalWrite(M2Enable,0);
          pixels.setPixelColor(2, pixels.Color(0,0,0)); 
          pixels.show();
           if((millis() - tmpsec2) >= 35000)
            motorstate2 = 0;
        }
      } break;
      default:digitalWrite(M2Enable,0);
          pixels.setPixelColor(2, pixels.Color(0,0,0)); 
          pixels.show();
          motorstate2 = 0;
      break;
          
  }
 
  Serial.println("\n______________________________________________________________________________________________________");
  delay(3000);

}

void RTC_clock()
{
  DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}
void DHT11_sensor(){

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  Serial.print("Current humidity = ");
  Serial.print(h);
  Serial.print("%  ");
  Serial.print("temperature = ");
  Serial.print(t);
  if(t<=15)
  {
  pixels.setPixelColor(6, pixels.Color(0,255,255)); 
  pixels.show();
  }
  else
  {
  pixels.setPixelColor(6, pixels.Color(0,0,0)); 
  pixels.show();
  }
  Serial.println("C  ");
}

  void Soil_sensor(){
    
  Serial.print("\nFirst plant soil: ");
  if(Soil1 < 300) {
    Serial.println("Very Wet");
    
      pixels.setPixelColor(3, pixels.Color(108,8,138)); 
      pixels.show();
      Soilsensor1 = 2;
    } 
     else if(Soil1 > 300 && Soil1<600){
        Serial.println("Wet");
        pixels.setPixelColor(3, pixels.Color(108,8,138)); 
        pixels.show();
        Soilsensor1 = 1;
       }
      else if(Soil1 > 600){
        Serial.println("Dry");
          pixels.setPixelColor(3, pixels.Color(0,0,0)); 
          pixels.show();
        Soilsensor1 = 0;
        }
}

  void Soil_sensor2(){
   Serial.print("Second plant soil: ");
  if(Soil2 < 300) {
    Serial.println("Very Wet\n");
    pixels.setPixelColor(4, pixels.Color(171,119,29)); 
    pixels.show();
    Soilsensor2 = 2;
   
    } 
     else if(Soil2 > 300 && Soil2<600){
        Serial.println("Wet\n");
        pixels.setPixelColor(4, pixels.Color(171,119,29)); 
        pixels.show();
        Soilsensor2 = 1;
       }
      else if(Soil2> 600){
        Serial.println("Dry\n");
        pixels.setPixelColor(4, pixels.Color(0,0,0)); 
        pixels.show();
        Soilsensor2 = 0;
        }
}

void Sun_light_LED(){
  
  if (sunstate == 0){
          suntime = millis();
          digitalWrite(Sunleds,1);
          pixels.setPixelColor(7, pixels.Color(153,0,153)); 
          pixels.show();
          sunstate = 1;
        }
        if( (millis() - suntime) >= 10000 ){
            digitalWrite(Sunleds,0);
            pixels.setPixelColor(7, pixels.Color(0,0,0)); 
          pixels.show();
           if((millis() - suntime) >= 15000)
            sunstate = 0;
        }
}

void Light_sensor(){
  
  Serial.println("Ambient Light");
  if (Light < 10) {
    Serial.print(" - Dark\n");
  } else if (Light < 200) {
    Serial.print(" - Dim\n");
  } else if (Light  < 500) {
    Serial.print(" - Light\n");
  } else if (Light  < 800) {
    Serial.print(" - Bright\n");
  } else {
    Serial.print(" - Very bright\n");
  }

}


void Water_level_sensor(){

  if (Water <= 600){ 
      Serial.println("Water Level: Empty\n");
      WaterError = 1;
    } 
    else if (Water>600 && Water<=700){
        Serial.println("Water Level: Low\n"); 
        pixels.setPixelColor(5, pixels.Color(0,0,0)); 
        WaterError = 0;
      } 
      else if (Water>700 && Water<=900){
              Serial.println("Water Level: Medium\n"); 
              WaterError = 0;
            }
        else if (Water>900){ 
              Serial.println("Water Level: High\n");
              WaterError = 0;
              }
}
