// #include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>

// DEFINE VARS
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET     -1 
#define LDR_INPUT 34
#define LDR_LIMIT 3700
#define SERVO_PIN 13

// HTTP Variables
String payload;
String url = "";
// WiFi Credentials
const char* ssid = "";
const char* pass = "";
// LDR Variables
float LDR_VAL = 0;
int pos = 90;
int increment = -1;
//Object init
HTTPClient http;  
Servo servo;
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  DispInit();
  servo.attach(SERVO_PIN);
  pinMode(LDR_INPUT, INPUT);
  WifiConnect();
  Serial.begin(9600);
  delay(100);
  Serial.println(ssid);
  servo.write(90);
}


void loop() {
  if(WiFi.status() != WL_CONNECTED){
    WifiConnect();
  }
  // get message from db, establish http connection
  payload = getMessage(); // get message from php script
  if(payload){
    while(!isLidOpen()){
      yield();
      rotateHeart();
    }
    servo.write(90);
    String delimiter = "<br>"; //seperate string by <br>
    String elements[100]; //store split elements in this array
    int numElements = 0; //number of elements in the array
    splitString(payload, delimiter, elements, numElements); 
    DispMsg(elements, numElements);
  
  }else{
    
  }
  
}


//------------------------------------------------------------------------------
/////////////////////FUNCTIONS///////////////////////////////////////////
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// rotateHeart() - rotate heart if message is recieved and unread
//------------------------------------------------------------------------------
void rotateHeart(){
  servo.write(pos);
  delay(50);
  if(pos == 75 || pos == 105){
    increment *= -1; //swings between 75 to 105 and vice versa
  }
  pos += increment;
}

//------------------------------------------------------------------------------
// isLidOpen() - returns true if box is open
//------------------------------------------------------------------------------
int isLidOpen(){
  LDR_VAL = analogRead(LDR_INPUT);
  if(LDR_VAL > LDR_LIMIT){
    return 0;
  }else{
    return 1;
  }
}

//------------------------------------------------------------------------------
// DispMsg() -  sorts message from DB and stores it in variables
//              displays the variables in the OLED display
//              makes a HTTP request to the updateReadAlrdy.php page
//------------------------------------------------------------------------------
void DispMsg(String elements[100], int numElements){
  for (int i = 0; i < numElements; i++) {
      String element = elements[i];
      if(element){
        // Extract ID, Time, and Message without regular expressions
        int idStart = element.indexOf("ID:") + 3;
        int idEnd = element.indexOf(",Time:");
        int timeStart = element.indexOf("Time:") + 5;
        int timeEnd = element.indexOf(",isBinary:");
        int isBinaryStart = element.indexOf("isBinary:") + 9;
        int isBinaryEnd = element.indexOf(",Message:");
        int messageStart = element.indexOf("Message:") + 8;
        
        String id = element.substring(idStart, idEnd);
        String time = element.substring(timeStart, timeEnd);
        String isBinary = element.substring(isBinaryStart, isBinaryEnd);
        String message = element.substring(messageStart);
        Serial.println(isBinary);
        delay(1000);
        if(id != ">"){
          if(isBinary == "0"){
            display.setTextSize(1);
            display.setTextColor(SH110X_WHITE);
            display.setCursor(0,1);
            display.print("Time: ");
            display.println(time);
            display.println(message);
          }else if (isBinary == "1") {
            byte binaryImage[SCREEN_WIDTH * SCREEN_HEIGHT / 8];
            for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT / 8; i++) {
              binaryImage[i] = strtol(message.substring(i * 8, (i + 1) * 8).c_str(), NULL, 2);
            }
            display.drawBitmap(0, 0, binaryImage, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
          }
          display.display();

          String updateURL = url+"/updateReadAlrdy.php?"+"ID="+id;
          Serial.println(updateURL);
          http.begin(updateURL);
          int httpCode = http.GET();
          if (httpCode > 0){
            if (httpCode == HTTP_CODE_OK){
              Serial.println(http.getString());
            }else{
              Serial.printf("HTTP Error: %f\n", http.errorToString(httpCode).c_str());
            }
          }
          http.end();
          delay(8000);
          display.clearDisplay();
          display.display();
        }
      }
    }
}
//------------------------------------------------------------------------------
// WifiConnect() - 
//------------------------------------------------------------------------------
void WifiConnect(){
  if(WiFi.status() != WL_CONNECTED){
    WiFi.begin(ssid, pass);
    // Waiting for a connection below
    while (WiFi.status() != WL_CONNECTED) {
      yield();
      delay(500);
    }
  }
}
//------------------------------------------------------------------------------
// DispInit() - 
//------------------------------------------------------------------------------
void DispInit(){
  display.begin(SCREEN_ADDRESS, true);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,28);
  display.println("HELLO BOOB!");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}
//------------------------------------------------------------------------------
// SplitString() - Function to split a string by a delimiter and store it in an array 
//------------------------------------------------------------------------------
void splitString(const String &input, String delimiter, String array[], int &arraySize) {
  int currentIndex = 0;
  int startIndex = 0;
  int endIndex = -1;
  
  while ((endIndex = input.indexOf(delimiter, startIndex)) != -1) {
    array[currentIndex++] = input.substring(startIndex, endIndex);
    startIndex = endIndex + 1;
  }
  
  // Add the last part of the string to the array
  if (startIndex < input.length()) {
    array[currentIndex++] = input.substring(startIndex);
  }
  
  arraySize = currentIndex;
}

//------------------------------------------------------------------------------
// getMessage() - get string from php string and return it to main()
//------------------------------------------------------------------------------
String getMessage() {
  String gatherURL = url+"/db-connection-esp.php";
  http.begin(gatherURL);
  // put your main code here, to run repeatedly:
  int httpCode = http.GET();
  if (httpCode > 0){
    if (httpCode == HTTP_CODE_OK){
      payload = http.getString();  
      if(payload != "0 results"){
        //DBG
        // Serial.println(payload);
        return payload;
      }
    }else{
      Serial.printf("HTTP Error: %f\n", http.errorToString(httpCode).c_str());
    }
  }
  http.end();
   
}