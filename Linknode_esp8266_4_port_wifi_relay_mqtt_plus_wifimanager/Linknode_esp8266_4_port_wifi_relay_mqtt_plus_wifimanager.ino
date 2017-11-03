/*
 Copyright (C) 2017 LeRoy Miller.

 Linknode ESP8266 4 Port Wifi enable relay, using MQTT

 Publish to:
 Host: test.mosquitto.org
 Topic: relayin
 Commands are 1 to 4 (1, 2, 3, 4) - These will toggle the relay

Subscribe to:
Host: test.mosquitto.org
Topic: relayout
Shows the status of relay

 This is a experiment using MQTT. Currently using a public broker,
 setting can be changed below. And no security, use at your own risk
 Libraries required:
 ESP8266Wifi, ESP8266WebServer, WiFiManager PubSubClient

  License:  
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
 *               Added unique MQTT client ID.
 *               Moved Ack Updates to void updateStatus()
 *               
 * Things that could be improved:
 * Relay just listen for messages addressed to this module.
 * Change status, and relay information (relayout) to use json strings
 * 
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

//Set All Relays to OFF
boolean toggle1 = 0;
boolean toggle2 = 0;
boolean toggle3 = 0;
boolean toggle4 = 0;

//MQTT Variables
const char* mqtt_server = "test.mosquitto.org";
char* outTopic = "relayout";
char* inTopic = "relayin";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
char *robotStatus;

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


//Probably should have used #define for the pins...oh well 
   pinMode(12, OUTPUT); //D10 
   pinMode(13, OUTPUT); //D8
   pinMode(14, OUTPUT); //D4
   pinMode(16, OUTPUT); //D3
   
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

switch ((char)payload[0]) {
  case '1':
  client.publish(outTopic, "Relay One: ");
  toggle1 = !toggle1;
    updateStatus(toggle1);
      digitalWrite(12, toggle1);
  break;
  case '2':
  client.publish(outTopic, "Relay Two: ");
  toggle2 = !toggle2;
    updateStatus(toggle2);
      digitalWrite(13, toggle2);
  break;
  case '3':
  client.publish(outTopic, "Relay Three: ");
  toggle3 = !toggle3;
    updateStatus(toggle3);
      digitalWrite(14, toggle3);
  break;
  case '4':
  client.publish(outTopic, "Relay Four: ");
  toggle4 = !toggle4;
    updateStatus(toggle4);
      digitalWrite(16, toggle4);
  break;
  case 's':
  client.publish(outTopic,"Status: (0 off/1 on)");
  char temp[100];
  snprintf(temp,100,"Relay 1: %d, Relay 2: %d, Relay 3: %d, Relay 4: %d",toggle1,toggle2,toggle3,toggle4);
  client.publish(outTopic,temp);
  break;
  default:
  client.publish(outTopic, "Unknown Relay, Use # between 1 and 4, or s for status");
  
  }
}

void updateStatus(boolean toggle) {
  if (toggle == 0) { 
      client.publish(outTopic, "Off"); } else {
        client.publish(outTopic, "On");
      }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    uint32_t chipid=ESP.getChipId();
    char clientid[16];
    snprintf(clientid,16,"Relay%08X",chipid);
    Serial.println(clientid);
    if (client.connect(clientid)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(outTopic, "LinkNode Relay Online!");
      // ... and resubscribe
      client.subscribe(inTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}


