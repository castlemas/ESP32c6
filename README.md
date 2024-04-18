# Smart Home Automation

## Introduction
Welcome to the GitHub repository of our Smart Home Automation Project! This project is designed to automate 2 processes. 
1. Smart Irrigation Process for personal gardening purposes
2. Laptop Fan-Cooler with ThingSpeak and email alerts for high temperature.

## Basic Requirements
1. Visual Studio Code installed
2. ESP-IDF extension installed with version 1.71 and above

## 1. Smart Irrigation
### Features
- **Automated Watering**: Automatically waters your plant based on the soil moisture level.
<!-- - **Easy Configuration**: A user-friendly interface to set up and manage watering schedules. -->
- **Real-Time Monitoring**: Monitor soil moisture and weather updates in real-time via our mobile app or web dashboard.
<!-- - **Energy Efficient**: Designed to save water and reduce energy consumption by using advanced algorithms to determine the precise amount of water needed. -->

### Hardware Requirements
- Soil moisture sensor
- Microcontroller (ESP32C6)
- 12V Water pump
- Water pump tubing
- 12V DC Power Supply
- 5V Relay
- Power supply
- Jumper Wires


### Software Requirements
- ESP-IDF for programming the microcontroller
- Rainmaker Application

### Installation
#### Step 1
Assemble the hardware components as per the schematic diagram below
![Schematic Diagram of the Auto Irrigation System](https://raw.githubusercontent.com/castlemas/Smart-Home-Automation-ESP32c6/main/images/schematic-diagram.jpg
)

#### Step 2
Pull the repo from the 

#### Step 3
Open the folder in Visual Studio Code. Configure the ESP-IDF to ESP32C6 and the port that you are connected to. 

(Note: You can know which port you are using by checking the device manager)

#### Step 4
Click on **build** and wait for the build to be completed. Once the build is done, click on **flash**.

#### Step 5
For first time use, click on **monitor** and a QR code will be generated. Scan the QR code with the ESP RainMaker App and the device will be connected to the app.

### Configuration
- **Moisture Threshold**: Set the desired soil moisture level that triggers the irrigation system.
<!-- - **Watering Schedule**: Customize the watering intervals and duration according to your specific needs. -->

### Usage
After installation and configuration, the system will automatically start monitoring the soil moisture and begin irrigation when necessary. You can monitor the system's status and make any adjustments through the rainmaker app.

## 2. Smart Fan
### Features



### Hardware Requirements
![alt text](https://hackmd-prod-images.s3-ap-northeast-1.amazonaws.com/uploads/upload_8a2a35991a701750a73a1a32da6f2a31.png?AWSAccessKeyId=AKIA3XSAAW6AWSKNINWO&Expires=1713327807&Signature=fj6JDPdEWNvrKlpbK2AbJF2zkMI%3D)
![alt text](https://hackmd.io/_uploads/r18sZAne0.png)
- DHT 11
- Microcontroller (ESP32C6)
- 3V DC Fan 
- 1K Resistor
- Power supply
- 2N7000 Mosfet
- 1N4001 Diode

### Software Requirements
- ESP-IDF for programming the microcontroller
- Rainmaker Application
- ThingSpeak
- MATLAB
- Outlook

### Installation
For full details please refer to tutorial here: https://docs.google.com/document/d/1O-hQE0gVF4TNJDogpcM8xBmY2vE1PERtSCZabrK3UpY/edit?usp=sharing

## Contact
- Project Link: [https://github.com/castlemas/ESP32c6](https://github.com/castlemas/ESP32c6)

## Acknowledgements

Acknowledgment
https://esp32tutorials.com/esp32-esp-idf-thingspeak-send-sensor-readings/ 
https://esp32tutorials.com/bme280-esp32-esp-idf-oled/ 
https://github.com/UncleRus/esp-idf-lib?tab=readme-ov-file 
https://esp32tutorials.com/esp32-web-server-esp-idf/ 
