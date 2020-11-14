
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include <MPU6050_tockn.h>
#include <iostream>


MPU6050 mpu6050(Wire);

//Pin numbers bliver defineret her
const int joystickXPin = 36;
const int potentiometerPin = 32;
const int buttonPinWhite = 23;
const int buttonPinBlue = 5;

//til potentiometeret
int prevalue = 0;
const int tollerance = 250;

int rollDeadzone;
int pitchDeadzone;
int yawDeadzone;
int buttonState = 0;

int status = WL_IDLE_STATUS;
char ssid[] = "OnePlus5T"; //  your network SSID (name)
char pass[] = "123321hej";    // your network password (use for WPA, or use as key for WEP)


unsigned int outgoinglocalPort = 4000;      // local port to listen on
unsigned int incomminglocalPort=4004;
IPAddress ip;

WiFiUDP UDP;



char packetBuffer[255];
char reply[] = "connectionSucces";

String cmd;
using namespace std; 

//Solution found at https://stackoverflow.com/questions/17853988/convert-string-to-const-char-issue
int convertChar() 
{ 
const char* command = cmd.c_str(); 
string str;
int i; 
for(i=0;i<sizeof(command);i++) 
    { 
        str[i]=command[i]; 
        cout<<str[i]; 
    } 
return 0; 
}

boolean isconnected = false;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);

  pinMode(joystickXPin, INPUT);
  pinMode(potentiometerPin, INPUT);
  pinMode(buttonPinWhite, INPUT_PULLUP);
  pinMode(buttonPinBlue, INPUT_PULLUP);

  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);


  mpu6050.update();
  rollDeadzone = mpu6050.getAngleX();
  pitchDeadzone = mpu6050.getAngleY();
  yawDeadzone = analogRead(joystickXPin);


  while (status != WL_CONNECTED) {

    Serial.print("Attempting to connect to WPA SSID: ");

    Serial.println(ssid);

    // Connect to WPA/WPA2 network:

    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:

    delay(2000);
    Serial.println(WiFi.localIP());

  }

  Serial.println("Connected to wifi");

  UDP.begin(incomminglocalPort);
}

void rc(){
  mpu6050.update();
  int roll = mpu6050.getAngleX() * -1;
  int pitch = mpu6050.getAngleY() * -1;
  int throttle = map(analogRead(potentiometerPin), 0, 4095, -100, 100);
  if(throttle < 5 && throttle > -5){
    throttle = 0;
  }
  int yaw = map(analogRead(joystickXPin)+160, 0, 4095, -100, 100);
  if(yaw < yawDeadzone+15 && yaw > yawDeadzone-5){
    yaw=0;
  }
cmd = "rc " + String(throttle) + " " + String(roll) + " " + String(pitch) + " " + String(yaw);
Serial.println(cmd);
}

  void establishConnection(){
    if(!isconnected){
    // if there's data available, read a packet
      int packetSize = UDP.parsePacket();
      if(packetSize){
        Serial.print("Received packet of size ");
        Serial.println(packetSize);
        

        UDP.read(packetBuffer, 255);
          if(strcmp(packetBuffer,"connectionAttempt")==0){
            delay(1000);
            UDP.beginPacket(UDP.remoteIP(), outgoinglocalPort);
            //UDP.write would not sende char array
            //Solution code found at:https://forum.arduino.cc/index.php?topic=586641.0 - Comment from lesept
            int i = 0;
            while (reply[i] != 0)
            UDP.write((uint8_t)reply[i++]);
            
            UDP.endPacket();
            delay(1000);
            isconnected=true;
      }
    }
  }
}

void sendCommands(){
 if(isconnected){
    rc();
    UDP.beginPacket(UDP.remoteIP(), outgoinglocalPort);
   //UDP.write would not sende char array
   //Solution code found at:https://forum.arduino.cc/index.php?topic=586641.0 - Comment from lesept
    int i = 0;
    while (cmd[i] != 0)
      UDP.write((uint8_t)cmd[i++]);

    UDP.endPacket(); 
    }
}

void disconnect(){
  buttonState = digitalRead(buttonPinBlue);
  if(!buttonState){
    delay(1000);
    Serial.println("Disconnected!");
    isconnected = false;
  }
}

void loop() {

  establishConnection();

  sendCommands();

  disconnect();
}