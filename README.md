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




