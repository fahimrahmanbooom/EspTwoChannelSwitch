
// Include the libraries that we'll be using throughout the code

#include <WiFiManager.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

Ticker ticker;

ESP8266WebServer server(8001);


const int ledPin = LED_BUILTIN;

//const int ledPin = D1;


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








void handleSetLed() {

  String ledStatus = server.arg("led");

  bool url_check = false;

  if ((ledStatus == "on") || (ledStatus == "off")) {

    url_check = true;
  }

  if (ledStatus == "on") {
    digitalWrite(ledPin, HIGH);
  }
  else if (ledStatus == "off") {
    digitalWrite(ledPin, LOW);
  }

  if (url_check) {

    if (ledStatus == "on") {

      server.send(200, "text/plain", "LED ON");
    }

    else {

      server.send(200, "text/plain", "LED OFF");
    }

  }
  else {
    server.send(200, "text/plain", "LED status unchanged!");
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


  // Set the LED pins to act as digital outputs and set them to a LOW
  // state initially.
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);





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
  Serial.println("connected...yeey :)");
  ticker.detach();
  //keep LED on
  digitalWrite(LED, HIGH);




  // Associate the URLs with the functions that will be handling the requests

  server.on("/set", HTTP_GET, handleSetLed);
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
