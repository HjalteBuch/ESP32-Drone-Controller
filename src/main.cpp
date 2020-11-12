
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUDP.h>

int status = WL_IDLE_STATUS;
char ssid[] = "OnePlus5T"; //  your network SSID (name)
char pass[] = "123321hej";    // your network password (use for WPA, or use as key for WEP)


unsigned int localPort = 4000;      // local port to listen on
IPAddress ip;

WiFiUDP UDP;
AsyncUDP udp;

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

  }

  Serial.println("Connected to wifi");

  UDP.begin(localPort);
}


void loop() {
  if(UDP.parsePacket()){
    Serial.println("alksjdf");
  }
}