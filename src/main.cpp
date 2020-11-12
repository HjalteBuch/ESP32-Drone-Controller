
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUDP.h>

int status = WL_IDLE_STATUS;
char ssid[] = "OnePlus5T"; //  your network SSID (name)
char pass[] = "123321hej";    // your network password (use for WPA, or use as key for WEP)


unsigned int outgoinglocalPort = 4000;      // local port to listen on
unsigned int incomminglocalPort=4004;
IPAddress ip;

WiFiUDP UDP;

char packetBuffer[255];
char reply[] = "connectionSucces";


void setup() {
  Serial.begin(9600);
  while(!Serial);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);

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

void loop() {
  // if there's data available, read a packet
  int packetSize = UDP.parsePacket();
  if(packetSize)
  {
    
    Serial.print("Received packet of size ");
    Serial.println(packetSize);

    UDP.read(packetBuffer, 255);
    
    UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());

    int i = 0;
    while (reply[i] != 0)
    UDP.write((uint8_t)reply[i++]);

    UDP.endPacket();

  }

}