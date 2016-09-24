/*
 *  Copyright 2016 SmartThings
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 *
 */

// Include libraries for communication, LED control, and SmartThings integration
#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#include <SmartThings.h>

// Set arduino input/output pins
#define PIN_THING_RX    3
#define PIN_THING_TX    2
#define LED_PIN         6

// Create smartthing object for communicating with SmartThings Shield
SmartThingsCallout_t messageCallout;
SmartThings smartthing(PIN_THING_RX, PIN_THING_TX, messageCallout);

// Create neopixel object for controlling LED strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(3, LED_PIN, NEO_GRB + NEO_KHZ800);

// ------- GLOBAL VARIABLES -------
// Enable debug for additional logging
bool isDebugEnabled;

// Determines if LED is on or off
int stateLED;

// Sound level
int soundLevel = 0;


// ------- SETUP -------
// Initial setup of the Arduino and SmartThings shield
void setup() {
  
  // Set up default state of global variables
  isDebugEnabled = true;
  stateLED = 0;

  // Initialize all LEDs to off
  strip.begin();
  strip.show();

  // Log setup to serial output for debugging
  if (isDebugEnabled) {
    Serial.begin(9600);
    Serial.println("setup..");
  }
  
}

// ------- MAIN LOOP -------
// This defines the main behavior of the device
void loop() {

  // TODO: Check the mic for input level and update the LEDs
  /*
  soundLevel += 1;
  if (soundLevel > 100) soundLevel = 0;
  
  Serial.print("Level: ");
  Serial.print(soundLevel);
  Serial.println("' ");
  displayLights (soundLevel);
   */
  // Initiate the shield to start waiting for events
  smartthing.run();
  
}

// ------- MESSAGE CALLOUT -------
// Act on a message from the cloud
void messageCallout(String message) {
  
  // If debug is enabled print out the received message
  if (isDebugEnabled) {
    Serial.print("Received message: '");
    Serial.print(message);
    Serial.println("' ");
  }

  // Run appropriate function depending on the command received from SmartThings
  if (message.equals("on")) {
    on();
  }
  else if (message.equals("off")) {
    off();
  }
  else if (message.equals("calibrate")) {
    calibrate();
  }
  
}

// ------- TURN ON -------
// Turn on the device
void on() {
  
  // Turn on the LED and set it to blue
  stateLED = 1;
  digitalWrite(LED_PIN, HIGH);
  smartthing.shieldSetLED(0, 0, 1);
  
  // Send "on" event to the SmartThings cloud
  smartthing.send("on");
  
  // Record state for debugging
  Serial.println("on");
  
}

// ------- TURN OFF -------
// Turn off the device
void off() {
  
  // Turn off the LED
  stateLED = 0;
  digitalWrite(LED_PIN, LOW);
  smartthing.shieldSetLED(0, 0, 0);
  
  // Send "off" event to the SmartThings cloud
  smartthing.send("off");
  
  // Record state for debugging
  Serial.println("off");
  
}

// ------- CALIBRATE -------
// Calibrate the device
void calibrate() {

  // Temporary - call test() function
  test();
  
  // Send "test" event to SmartThings cloud
  smartthing.send("calibrate");
  
  // Log success
  Serial.println("Calibration success!");

  // Respond with sound level
  smartthing.send("0");
  
}

// ------- TEST -------
// Test by cycling the shield's LED through colors
void test() {

  // If the device is on, cycle the LED
  while(stateLED) {
    
    // Cycle through colors
    for (int i = 0; i < 24; i++) {
      setThingLEDRainbow(i, smartthing);
      delay(50);
    }
  }

  // Log success
    Serial.println("Test success!");
  
}

// ------- DISPLAY STATUS -------
// Animate the lights based on sound level from 0-100
void displayLights (int level) {

  // Ensure value is within appropriate limits
  if (level < 0) level = 0;
  if (level > 100) level = 100;

  // Working RGB values
  int r = 0;
  int g = 0;
  int b = 0;

  // Determine correct color for the level value
  r = (level / 100) * 255;
  g = ((100 - level) / 100) * 255;

  // Set light colors
  //theaterChase(strip.Color(r, g, 0), 50);

/*
  for(uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(level));
    strip.show();
    delay(50);
  }
*/

  for (int q=0; q < 3; q++) {
    for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
      strip.setPixelColor(i+q, Wheel(level));    //turn every third pixel on

    }
    strip.show();

    delay((level * 2) + 50);

    for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
      strip.setPixelColor(i+q, 0);        //turn every third pixel off

    }
  }
}

// ------- RAINBOW -------
// Set the ThingShield's LED to a color of the rainbow
// angle = 0 to 23, where 0 = red and increasing numbers cycle through colors in ROYGBIV order back to red
void setThingLEDRainbow (int angle, SmartThings thing) {
  
  // Working RGB values
  int r = 0;
  int g = 0;
  int b = 0;

  bool first = true;

  // Figure out what we're doing
  if (angle < 8) // red > green
  {
    // red decreases from 8, green increases to 8
    r = 8 - angle;
    g = angle;

  }
  if (angle >= 8 && angle < 16) // green > blue
  {
    // green decreases from 8, blue increases to 8
    angle -= 8;
    g = 8 - angle;
    b = angle;
  }
  if (angle >= 16) // blue > red
  {
    // blue decreases from 8, red increases to 8
    angle -= 16;
    b = 8 - angle;
    r = angle;
  }

  // Set RGB LED color
  thing.shieldSetLED(r, g, b);

}

// ------- THEATER CHASE -------
//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// ------- COLOR WHEEL -------
// Sets the strip to a color of the rainbow from 0-255
// 0 is red, up to 255 in the order red > orange > yellow, etc.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

