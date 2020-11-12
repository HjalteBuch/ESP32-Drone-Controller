
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUDP.h>
#include <MPU6050_tockn.h>

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

int status = WL_IDLE_STATUS;
char ssid[] = "OnePlus5T"; //  your network SSID (name)
char pass[] = "123321hej";    // your network password (use for WPA, or use as key for WEP)


unsigned int outgoinglocalPort = 4000;      // local port to listen on
unsigned int incomminglocalPort=4004;
IPAddress ip;

WiFiUDP UDP;

char packetBuffer[255];
char reply[] = "connectionSucces";
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
  int yaw = map(analogRead(joystickXPin)+160, 0, 4095, -100, 100);
String cmd = "rc " + String(roll) + " " + String(pitch) + " " + String(throttle) + " " + String(yaw);
Serial.println(cmd);
}

void loop() {
  // if there's data available, read a packet
  int packetSize = UDP.parsePacket();
  if(packetSize)
  {
    
    Serial.print("Received packet of size ");
    Serial.println(packetSize);

    UDP.read(packetBuffer, 255);
    
    UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());

   //UDP.write would not sende char array
   //Solution code found at:https://forum.arduino.cc/index.php?topic=586641.0 - Comment from lesept
    int i = 0;
    while (reply[i] != 0)
    UDP.write((uint8_t)reply[i++]);

    UDP.endPacket();

    isconnected=true;
    
  }
  if(isconnected){
      rc();
      
    }

}