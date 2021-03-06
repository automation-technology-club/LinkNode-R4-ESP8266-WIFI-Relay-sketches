/*
 *  
 *    http://server_ip/relay#totoggle
 *  http://hostname.local/relay#totoggle
 *  
 *  Sometimes hostname doesn't show up on the network quickly,
 *  pinging the IP address seems to help.
 *  
 *  (C) LeRoy Miller, 2017
 *  
 *   License:  
 *  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses>
 
*/

/*
 * Nov 3, 2017 - Added status update.
 *
 */

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager


const char* host = "wifirelay";
/*
const char* ssid = "Motel6";
const char* password = "";
*/

//Set All Relays to OFF
boolean toggle1 = 0;
boolean toggle2 = 0;
boolean toggle3 = 0;
boolean toggle4 = 0;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);


void setup() {
   Serial.begin(115200);
  delay(10);

//WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    
    //set custom ip for portal
    //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("AutoConnectAP");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");


 
   pinMode(12, OUTPUT); //D10 
   pinMode(13, OUTPUT); //D8
   pinMode(14, OUTPUT); //D4
   pinMode(16, OUTPUT); //D3
   
  
  // Connect to WiFi network
 // Serial.println();
 // Serial.println();
 // Serial.print("Connecting to ");
 // Serial.println(ssid);
  
  WiFi.hostname(host);
  //WiFi.begin(ssid, password);
  MDNS.begin(host);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  client.println("Wifi Relay v1.1");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  req.replace("%20", " ");
  req.remove(0,5);
  req.remove(req.length()-9, 9);
  Serial.println(req);
  //client.println(req);
  if (req == "1") {
    client.print("Relay One: ");
    toggle1 = !toggle1;
    if (toggle1 == 0) { 
      client.print("Off"); } else {
        client.print("On");
      }
    digitalWrite(12, toggle1);
  }
  if (req == "2") {
    client.print("Relay Two: ");
    toggle2 = !toggle2;
    if (toggle2 == 0) { 
      client.print("Off"); } else {
        client.print("On");
      }
    digitalWrite(13, toggle2);
  }
  if (req == "3") {
    client.print("Relay Three: ");
    toggle3 = !toggle3;
    if (toggle3 == 0) { 
      client.print("Off"); } else {
        client.print("On");
      }
    digitalWrite(14, toggle3);
  }
  if (req == "4") {
    client.print("Relay Four: ");
    toggle4 = !toggle4;
    if (toggle4 == 0) { 
      client.print("Off"); } else {
        client.print("On");
      }
    digitalWrite(16, toggle4);
  }
  if (req == "s") {
    client.print("Status: (0 off/1 on)\n");
    char temp[100];
  snprintf(temp,100,"Relay 1: %d, Relay 2: %d, Relay 3: %d, Relay 4: %d",toggle1,toggle2,toggle3,toggle4);
  client.print(temp);
  }
  client.flush();
  
  
  Serial.println("Client disonnected");
delay(100);
  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}


