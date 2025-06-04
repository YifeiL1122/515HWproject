#include "arduino_secrets.h"
/*

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  float windspeed;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/
// arduino_secrets.h
#ifndef ARDUINO_SECRETS_H
#define ARDUINO_SECRETS_H
#define SECRET_SSID      // replace with your WiFi network name
#define SECRET_OPTIONAL_PASS   // replace with your WiFi password
#define SECRET_DEVICE_KEY "zM4L?IlYn2QA5HCO5DqKuc1RD"   // replace with your Arduino IoT Cloud device key
#endif

#include "thingProperties.h"

#include <Adafruit_GFX.h>


// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 1000;    // the debounce time; increase if the output flickers

int pinInterrupt = 32; //D5

int Count = 0;

void onChange()
{
  if ( digitalRead(pinInterrupt) == LOW )
    Count++;
}



void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
  */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  pinMode( pinInterrupt, INPUT_PULLUP);// set the interrupt pin

  //Enable
  attachInterrupt( digitalPinToInterrupt(pinInterrupt), onChange, FALLING);

  delay(500);
}

void loop() {
  ArduinoCloud.update();
  // Your code here

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    lastDebounceTime = millis();
    windspeed = ((Count * 8.75) / 100);

    if (windspeed > 25) windspeed = 25;
    Serial.print(windspeed);
    Serial.println("m/s");


    Count = 0;

  }
  delay(1);


}





/*
  Since Windspeed is READ_WRITE variable, onWindspeedChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onWindspeedChange()  {
  // Add your code here to act upon Windspeed change
}