# LOVEBOX
## Description
A simple and quick ESP32 project to display messages written to a website developed in *drumroll* PHP! This project was inspired by Julisa99's Lovebox repo.
Do check that out here: https://github.com/julisa99/Lovebox/blob/master/Lovebox.ino. Thank you Julisa99!
Made this for my partner and had alot of fun building this. 


## Project Status
This project is still in development as i want to make use of FreeRTOS though it might not be essential.

## Project Requirments
* ESP32 Development board
* Any wooden box
* 9 or 5 gram Servo, anything that is capable of fitting inside the box
* LDR sensor of any kind
* 10K ohm resistor for LDR ADC reading
* OLED screen of any kind (i used a 128x64 SH1106G)
* Some wires and elbow grease 👍

## Known bugs
After 4 days of execution plugged in, the board resets quite often with a Guru Meditation Error. Which is why i will be posting a FreeRTOS version soon!

## How it works
The ESP32 Dev Board used has in-built WiFi, using the WiFi.h library one can very easily connect to their home network or anywhere else with just a SSID and Password. The HTTPClient.h library can enable the board to make HTTP request to any website and even get the content of the website! These libraries are the backbone of this project  
`#include <HTTPClient.h>`  
`#include <WiFi.h>`  
`HTTPClient http;`    
`Servo servo;`  

To make the OLED display work i used the `Adafruit_SH110X.h` library  
`#include <Adafruit_SH110X.h>`  
`Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);`


