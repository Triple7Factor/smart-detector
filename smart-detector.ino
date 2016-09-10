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

// Include libraries for communication & SmartThings integration
#include <SoftwareSerial.h>
#include <SmartThings.h>

// Set arduino input/output pins and create smartthing object for communicating with SmartThings Shield
#define PIN_THING_RX    3
#define PIN_THING_TX    2
#define LED_PIN         13

SmartThingsCallout_t messageCallout;
SmartThings smartthing(PIN_THING_RX, PIN_THING_TX, messageCallout);

// Enable debug for additional logging
bool isDebugEnabled;

// Determines if LED is on or off
int stateLED;

// ------- SETUP -------
// Initial setup of the Arduino and SmartThings shield
void setup() {
  
  // Set up default state of global variables
  isDebugEnabled = true;
  stateLED = 0;
  
  // Set up LED pins so LED is off to match stateLED 
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Log setup to serial output for debugging
  if (isDebugEnabled) {
    Serial.begin(9600);
    Serial.println("setup..");
  }
  
}

// ------- MAIN LOOP -------
// This defines the main behavior of the device
void loop() {
  
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


