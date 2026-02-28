# Creating the Phone module
## 1.	Connect the ESP32 to the MPU6500
Connect the following pins:
*	ESP32 3V3 -> MPU6500 VCC
*	ESP32 GND -> MPU6500 GND
*	ESP32 GPIO8 -> MPU6500 SDA
*	ESP32 GPIO9 -> MPU6500 SCL
Note: If you have a different ESP32, the SDA and SCL pins might also be different. The correct pins can usually be found through a quick online search.
## 2.	Upload the code to the ESP32
Connect the ESP32 to a laptop (using a USB C cable). In Arduino IDE, paste the code: Module1_code.ino 
Within the code, change the following: 
*	Add your Iotroam password in line 42
*	Add a unique OOCSI name in line 45
*	Add your OOCSI channel name in line 121
*	In line 124, you will later add your device ID (see “Invigilator Module”); you can leave it as it is for now
Upload this code to your ESP32. If you check OOCSI, you should see that it is sending messages to your channel.
If the MPU6500 is moving slowly enough, it sends a 0. If it starts moving faster than the given threshold, it sends a 1. If it continues to move fast after sending a 1, it will send a 2.

# Creating the Invigilator and connecting it to the Phone module
## 1.	Create an IOT dataset
*	Create a new Data Foundry Project.
*	Click “Add dataset” and choose the “IOT” option (also called “add data from connected devices).
*	Once you have created the dataset, scroll down to “Configuration” and click on the “Data from OOCSI” tab.
*	In the section called “Channel name”, type the name of your OOCSI channel (the same one added in the ESP32 code earlier), then click “save”.

## 2. Add a new device and add the ID to the Arduino code
*	Go back to the main page of your Data Foundry project.
*	Click on the tab at the top called “Sources”, and in the “Devices” section, click “add device”. Add your ESP32 by putting in all the necessary information and click “Add”.
*	Once you have done that, you will see your device in the “Devices” section. Next to the device name, you will see the device ID (a long string of letters and numbers). Copy this and insert it into the Arduino code, in line 124.
*	Upload the Arduino code to the ESP32.

## 3. Add a new Script connected to OOCSI
*	Go back to the main project page in Data Foundry. Scroll down to “Scripts” and click “Add script”.
*	In the script, paste the following code snippet:
````Java
DF.print("Script triggered");
DF.print(JSON.stringify(data));

data.timestamp = Date.now();
DF.eventData.log(devices[0].id, 'danger_level', data);
````
*	Click “Save”.
*	On the right side of the screen, there is a small menu bar with two options: “Testing” and “Live”. Click on “Live”, and type your channel name in the “Input OOCSI Channel” section. Click “Enable”.
*	Now, in the box underneath, the live data from your ESP32, sent through OOCSI, should appear.

## 4. Create the Invigilator
*	Go back to the main project page. Click “Add Dataset” and then “Existing” (“One or more files of an existing dataset”).
*	Once you have created the dataset, click on “upload file” on the right side of the screen and upload the file called “invigilator.html”
*	Scroll to the section “Dataset files”, find the file you just added, and click “edit” on the right.
*	Edit the code as follows:
*	  In line 350-add a unique name
*	  In line 354-Add your channel name
*	Click “Save” and then “Reload”.
*	The console at the bottom should show the live data coming from the ESP32. The visuals on the right side of the screen should react to both sound data from your laptop’s microphone as well as data from the gyroscope.
*	Note: if the sound data is not registering, go to your microphone setting and manually allow access to the laptop’s microphone.



