
// http://fahimrahman.xyz:8001/set?switch1=off&switch2=off
// 192.168.0.197:8001/set?switch1=off&switch2=off
// ipOfEsp:port/set?switch1=off&switch2=off

// TO setup wifi ssid and password just connect to esp hotspot and hit on 192.168.4.1 

// Include the libraries that we'll be using throughout the code

#include <WiFiManager.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

Ticker ticker;

ESP8266WebServer server(8001);


const int switch1 = D5;
const int switch2 = D7;


#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

int LED = LED_BUILTIN;


void tick()
{
  //toggle state
  digitalWrite(LED, !digitalRead(LED));     // set pin to the opposite state
}


//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}


void handleSetSwitch() {

  String switch1Status = server.arg("switch1");
  String switch2Status = server.arg("switch2");

  bool url_check = false;

  if((switch1Status == "on")||(switch1Status == "off")||(switch2Status == "on")||(switch2Status == "off")) {

    url_check = true;
  }



  if (switch1Status == "on") {
    digitalWrite(switch1, HIGH);
    Serial.println("Switch 1 Turned ON");
  }
  else if (switch1Status == "off") {
    digitalWrite(switch1, LOW);
    Serial.println("Switch 1 Turned OFF");
  }
  if (switch2Status == "on") {
    digitalWrite(switch2, HIGH);
    Serial.println("Switch 2 Turned ON");
  }
  else if (switch2Status == "off") {
    digitalWrite(switch2, LOW);
    Serial.println("Switch 2 Turned OFF");
  }

  

  if (url_check) {

    if ((switch1Status == "on") && (switch2Status == "on")) {
      server.send(200, "text/plain", "Switch 1 : ON\nSwitch 2 : ON");
    }
    if ((switch1Status == "off") && (switch2Status == "off")) {
      server.send(200, "text/plain", "Switch 1 : OFF\nSwitch 2 : OFF");
    }
    if ((switch1Status == "on") && (switch2Status == "off")) {
      server.send(200, "text/plain", "Switch 1 : ON\nSwitch 2 : OFF");
    }
    if ((switch1Status == "off") && (switch2Status == "on")) {
      server.send(200, "text/plain", "Switch 1 : OFF\nSwitch 2 : ON");
    }

  }
  else {
    server.send(200, "text/plain", "Switch Status Unchanged!");
  }

}



void handleNotFound() {
  // We construct a message to be returned to the client
  String message = "File Not Found\n\n";
  // which includes what URI was requested
  message += "URI: ";
  message += server.uri();
  // what method was used
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  // and what parameters were passed
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  // the response, as expected, is a "Not Found" (404) error
  server.send(404, "text/plain", message);
}



void setup(void) {


  // Set the pins to act as digital outputs and set them to a LOW
  // state initially.
  pinMode(switch1, OUTPUT);
  digitalWrite(switch1, LOW);
  pinMode(switch2, OUTPUT);
  digitalWrite(switch2, LOW);



  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // put your setup code here, to run once:
  Serial.begin(115200);

  //set led pin as output
  pinMode(LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  //reset settings - for testing
  // wm.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wm.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("Connected!");
  ticker.detach();
  //keep LED off
  digitalWrite(LED, HIGH);



  // Associate the URLs with the functions that will be handling the requests

  server.on("/set", HTTP_GET, handleSetSwitch);
  server.onNotFound(handleNotFound);

  // Start running the webserver
  server.begin();
  Serial.println("HTTP server started");
}



// The loop function is straight-forward, simply handle any incoming requests to the
// our ESP8266 host!
void loop(void) {
  server.handleClient();
}
