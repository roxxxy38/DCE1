/*
* Brian R Taylor
* brian.taylor@bolderflight.com
* 
* Copyright (c) 2021 Bolder Flight Systems Inc
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the “Software”), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

/*Note that the original code, as mentioned in the copyright statement, was modified by us so that it has the necessary features for our modules.*/

/*
This code uses MPU acceleration data to detect quick movements of a phone case to send to an digital invigilator
that punished you if you pick up your phone recklessly. 
to do:
send data to data foundry through OOCSI
alter code to work correctly with invigilator module
*/
#include <Wire.h>
#include "mpu6500.h"
#include <WiFi.h>
#include "OOCSI.h"

// Wi-Fi
const char* ssid = "iotroam";
const char* password = "IOTROAM_PASSWORD"; //add your iotroam password here

// OOCSI
const char* OOCSIName = "UNIQUE_NAME"; //add a unique oocsi name here
const char* hostserver = "oocsi.id.tue.nl";
OOCSI oocsi;

// Mpu6500 object
bfs::Mpu6500 imu;

float totalAccel;
float prevTotalAccel = 0;
float jerk;

bool movementDetected = false;

unsigned long movementDetectedTimer = 3000;
unsigned long movementDetectedBuffer = 500;
unsigned long stillMillis;

void connectWiFi() {
  Serial.print("Connecting to Wi-Fi ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
}

void getValues();
float calcTotalAccel();

void setup() {
  // Serial to display data
  Serial.begin(115200);
  delay(1000);

  while(!Serial) {}
  // Start the I2C bus
  Wire.begin();
  Wire.setClock(400000);
  // I2C bus,  0x68 address
  imu.Config(&Wire, bfs::Mpu6500::I2C_ADDR_PRIM);
  // Initialize and configure IMU
  if (!imu.Begin()) {
    Serial.println("Error initializing communication with IMU");
    while(1) {}
  }
  //Set the sample rate divider
  if (!imu.ConfigSrd(19)) {
    Serial.println("Error configured SRD");
    while(1) {}
  }

  connectWiFi();

  // Connect to OOCSI
  oocsi.connect(OOCSIName, hostserver);
  Serial.println("Connected to OOCSI");

}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting WiFi...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    delay(2000);
  }
//read the MPU data and collect acceleration data with function
  if (imu.Read()) {
    totalAccel = calcTotalAccel(imu.accel_x_mps2(), 
                                imu.accel_y_mps2(), 
                                imu.accel_z_mps2()
                               );
  }
// Prepare the OOCSI message 
  oocsi.newMessage("OOCSI_CHANNEL_NAME"); //add your oocsi channel name here

// Replace the second string below with your Physical ID from the Data Foundry Devices tab
  oocsi.addString("device_id", "YOUR_DEVICE_ID"); 



//detect acceleration relative to previous reading
  jerk = totalAccel - prevTotalAccel;
  prevTotalAccel = totalAccel;

//if the jerk is high enough, motion is detected and a timer is started
  if (jerk > 1.8 and 
      movementDetected == false
  ) {
    Serial.println("Hold still!!!");
    oocsi.addInt("danger_level", 1);
    stillMillis = millis();
    movementDetected = true;
  }

//if there is movement before the timer ends, you have been noticed. if you keep still, the situation resets
  if (movementDetected == true and 
      millis() - stillMillis < movementDetectedTimer and 
      millis() - stillMillis >= movementDetectedBuffer and
      jerk > 1
    ) {
    Serial.println("ts was NOT the wind");
    oocsi.addInt("danger_level", 2);
    movementDetected = false;
    delay(500);
  } else if (movementDetected == true and 
             millis() - stillMillis > movementDetectedTimer
            ) {
    Serial.println("must have been the wind");
    oocsi.addInt("danger_level", 0);
    movementDetected = false;
    delay(500);
  }
    else{
      oocsi.addInt("danger_level", 0);
    }

  //send to oocsi
  oocsi.sendMessage();
  oocsi.keepAlive();
  delay(100); // Sends data every 0.5 seconds
}

//calculates total acceleration in any direct to detect movement
float calcTotalAccel(float xAccel, float yAccel, float zAccel) {
  totalAccel = sqrt(xAccel*xAccel + yAccel*yAccel + zAccel*zAccel) - 9.81;
  return abs(totalAccel);
}

