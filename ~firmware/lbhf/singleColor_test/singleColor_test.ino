#include <Adafruit_NeoPixel.h>

#define LED_PIN 4
#define LED_COUNT 1
#define LINEIN_PIN 3

Adafruit_NeoPixel leds(LED_COUNT, LED_PIN, NEO_GRB +NEO_KHZ800);
//800 and 400 kHz

float hueValue = 0;
uint32_t color = leds.ColorHSV(hueValue);

const byte logoColors[][3] ={{229, 48, 83}, {224, 49, 90}, {219, 47, 100}, {222, 34, 96}, {257, 15, 83}, {347, 19, 83}, {7, 31, 89}, {15, 49, 96}, {19, 56, 100}, {28, 56, 100}};
//hue is expressed as a 16bit -> 65536
//saturation and value are 8bit -> 256

void setup() {
  Serial.begin(9600);
  leds.begin();
  leds.setBrightness(255);
  leds.show();  
  pinMode(3, INPUT);  
  startupAnimation();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void startupAnimation(){
    uint16_t hue = 15;
    uint16_t saturation = 100;
    uint16_t value = 83;
    //convert from standard (360,100,100) to 16bit, 8bit, 8bit
    hue = hue*65536/361;
    saturation = saturation*256/101;
    value = value*256/101;
    Serial.print(hue);
    Serial.print(", ");
    Serial.print(saturation);
    Serial.print(", ");
    Serial.println(value);
    uint32_t fillColor = leds.ColorHSV(hue,saturation,value);
    Serial.println(fillColor);
    leds.fill(fillColor);
    leds.show();
    delay(10000);
  }
