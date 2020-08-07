#include <Adafruit_NeoPixel.h>

//ATTACH AREF to VIN

#define BTLED_PIN 2 //BT LED attached to D2
#define LINEIN_PIN 4 //sound sensor attached to D4
#define LED_PIN 3 //rgb led attached to D3
#define LED_COUNT 1 //number of leds in that array
#define BATT_IND 5 //battery button attached to  D5
#define MODE_PIN 6 //mode button attached to D6
#define VBATT_PIN A1 //voltage divider attached to A1
#define BTDETECT_PIN A0 //Photoresistor attached to A0


Adafruit_NeoPixel leds(LED_COUNT, LED_PIN, NEO_GRB +NEO_KHZ800);
//800 and 400 kHz

uint16_t hueValue = 0;
uint32_t color = leds.ColorHSV(hueValue);
uint8_t mode = 1;

const byte logoColors[][3] ={{229, 48, 83}, {224, 49, 90}, {219, 47, 100}, {222, 34, 96}, {257, 15, 83}, {347, 19, 83}, {7, 31, 89}, {15, 49, 96}, {19, 56, 100}, {28, 56, 100}};
//hue is expressed as a 16bit -> 65536
//saturation and value are 8bit -> 256

void setup() {
  leds.begin();
  leds.setBrightness(255);
  leds.show();  
  pinMode(LINEIN_PIN, INPUT); 
  pinMode(VBATT_PIN, INPUT); 
  pinMode(BATT_IND, INPUT);
  pinMode(BTLED_PIN, OUTPUT);
  pinMode(BTDETECT_PIN, INPUT);
  startupAnimation();
}

void loop() {
  bool ledActivation = digitalRead(LINEIN_PIN);
  ledActivation = !ledActivation;
  if(digitalRead(BATT_IND) == 1) //detect batt button
  {
    displayVoltage();
  }
  else if(digitalRead(MODE_PIN)== 1) //detect mode button
  {
    delay(250);
    if(mode<5)
      mode++;
    else
      mode = 1; 

    for(int j=0; j<mode; j++){
      for(int i=0; i<250; i+=3){
        uint32_t fillColor = leds.gamma32(leds.ColorHSV(50000,200,i));
        leds.fill(fillColor);
        delay(2);
        leds.show();
      }
      delay(100);
      for(int i=250; i>1; i-=3){
        uint32_t fillColor = leds.gamma32(leds.ColorHSV(50000,200,i));
        leds.fill(fillColor);
        delay(2);
        leds.show();
      }
    }
    delay(250);
  }
  else if(mode == 1){// std sound reactive
    if(ledActivation){
      color = leds.ColorHSV(hueValue);
      leds.fill(color);
      leds.show();
      hueValue +=30;
    }
    else{
      leds.clear();
      leds.show();
    }
  }
  else if(mode == 2){ //inverse std sound reactive
    if(!ledActivation){
      color = leds.ColorHSV(hueValue);
      leds.fill(color);
      leds.show();
      hueValue += 5;
    }
    else{
      leds.clear();
      leds.show();
      delay(25);
    }
  }
  else if(mode == 3){ //sound reactive fade, speed adj
    if(ledActivation){
      color = leds.ColorHSV(hueValue);
      leds.fill(color);
      leds.show();
      hueValue += 3000;
    }
    else{
      color = leds.ColorHSV(hueValue);
      leds.fill(color);
      leds.show();
      hueValue += 1;
    }
  }
  else if(mode == 4){ //lamp mode
    if(ledActivation){
      color = leds.ColorHSV(hueValue);
      leds.fill(color);
      leds.show();
      hueValue += 55;
    }
    else{
      color = leds.ColorHSV(hueValue);
      leds.fill(color);
      leds.show();
      hueValue += 2;
    }
  }
  else if(mode == 5){ //led off
     leds.clear();
     leds.show();
  }
 
  //bluetoothHandler();
}


void bluetoothHandler(){
  uint16_t btdetect= analogRead(BTDETECT_PIN);
  if(btdetect>950)
    digitalWrite(BTLED_PIN, HIGH);
  else{
    digitalWrite(BTLED_PIN, LOW);
  }
}

//this function reads the 10 bit result from vbatt and returns it as a voltage with correction factors
//voltage divider = R1 = 50k, R2 = 100k
float readVoltage(){
  //calculated and calibrated factor from resistor tolerances Rtot = R1 *3 
  const float correctionFactor = 3;
  //analogRead 5v, 10bit resolution
  float voltage = (analogRead(VBATT_PIN)/1023.0)*5.0*correctionFactor;
  return voltage;
} 

void displayVoltage(){
  //conv voltage to a 10^-2 volt to allow for integer math in map function
  int augmentedVoltage = readVoltage()*100;
  uint8_t percentage;
  //map voltage 10.4 to 12.45 to percentage 
   //keep percentage in range
  if(augmentedVoltage<1041)
    percentage = 0;
  else if(augmentedVoltage>1230)
    percentage = 100;
  else
    percentage = map(augmentedVoltage,1040,1230,0,100);

  //batt critically low
  if(percentage <= 10){
    for(int j=0; j<3; j++){
      for(int i=0; i<250; i++){
        uint32_t fillColor = leds.gamma32(leds.ColorHSV(0,200,i));
        leds.fill(fillColor);
        delay(4);
        leds.show();
      }
      delay(200);
      for(int i=250; i>1; i--){
        uint32_t fillColor = leds.gamma32(leds.ColorHSV(0,200,i));
        leds.fill(fillColor);
        delay(2);
        leds.show();
      }
    }
  }
  else if(augmentedVoltage >= 1245){
     for(int j=0; j<3; j++){
      for(int i=0; i<250; i++){
        uint32_t fillColor = leds.gamma32(leds.ColorHSV(0,0,i));
        leds.fill(fillColor);
        delay(4);
        leds.show();
      }
      delay(200);
      for(int i=250; i>1; i--){
        uint32_t fillColor = leds.gamma32(leds.ColorHSV(0,0,i));
        leds.fill(fillColor);
        delay(2);
        leds.show();
      }
    }
  }
  else{
   //map to hue 
    uint16_t hue = map(percentage,0,100,0,100);
    //fade in and out
    for(int i=0; i<250; i++){
      uint32_t fillColor = leds.gamma32(leds.ColorHSV(hue*65536/361,200,i));
      leds.fill(fillColor);
      delay(4);
      leds.show();
    }
    delay(1000);
    for(int i=250; i>1; i--){
      uint32_t fillColor = leds.gamma32(leds.ColorHSV(hue*65536/361,200,i));
      leds.fill(fillColor);
      delay(2);
      leds.show();
    }
  }

  
}


void startupAnimation(){
  uint16_t hue = 230;
  uint8_t saturation = 100;
  uint8_t value = 255; 
  
  for(int i=0; i<250; i++){
    uint32_t fillColor = leds.gamma32(leds.ColorHSV(hue*65536/361,250,i));
    leds.fill(fillColor);
    delay(4);
    leds.show();
  }

  for(int i=250; i>1; i--){
    uint32_t fillColor = leds.gamma32(leds.ColorHSV(hue*65536/361,i,250));
    leds.fill(fillColor);
    delay(4);
    leds.show();
  }

  hue = 30;

   for(int i=0; i<250; i++){
    uint32_t fillColor = leds.gamma32(leds.ColorHSV(hue*65536/361,i,250));
    leds.fill(fillColor);
    delay(4);
    leds.show();
  }

  for(int i=250; i>0; i--){
    uint32_t fillColor = leds.gamma32(leds.ColorHSV(hue*65536/361,250,i));
    leds.fill(fillColor);
    delay(4);
    leds.show();
  }
  
//  uint16_t hue = logoColors[i][0];
//  uint8_t saturation = 100;
//  uint8_t value = 255; 
  //convert from standard (360,100,100) to 16bit, 8bit, 8bit
//    hue = hue*65536/361;
//    saturation = saturation*256/101;


//  for(byte i=0; i<10; i++){
//    uint16_t hue = logoColors[i][0];
//    uint8_t saturation = logoColors[i][1];
//    uint8_t value = logoColors [i][2]; 
//    //convert from standard (360,100,100) to 16bit, 8bit, 8bit
//    hue = hue*65536/361;
//    saturation = saturation*256/101;
//    value = 200;
////    Serial.print(hue);
////    Serial.print(", ");
////    Serial.print(saturation);
////    Serial.print(", ");
////    Serial.println(value);
//    uint32_t fillColor = leds.gamma32(leds.ColorHSV(hue,saturation,value));
////    Serial.println(fillColor);
//    leds.fill(fillColor);
//    leds.show();
//    delay(200); 
//  }
}
