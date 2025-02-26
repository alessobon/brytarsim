#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

const char *ssid = "brytarsim1";
const char *password = "1234";
const byte DNS_PORT = 53;
DNSServer dnsServer;
ESP8266WebServer server(80);

int franslagstidA = 40;

void handleRoot() {
    String page = "<!DOCTYPE html><html><head><title>Brytarsim 1</title>";
    page += "<meta http-equiv='refresh' content='5'></head><body>";
    page += "<h1>Brytarsim 1</h1>";
    page += "<form action='/set' method='POST'>";
    page += "<label>FRÅNSLAGSTID A (ms):</label>";
    page += "<input type='number' name='time' value='" + String(franslagstidA) + "'><br><br>";
    page += "<input type='submit' value='Set'>";
    page += "</form></body></html>";
    server.send(200, "text/html", page);
}

void handleSet() {
    if (server.hasArg("time")) {
        franslagstidA = server.arg("time").toInt();
        server.send(200, "text/html", "<h1>Value updated! Returning...</h1>");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, password);
    dnsServer.start(DNS_PORT, "brytarsim.local", WiFi.softAPIP());
    server.on("/", handleRoot);
    server.on("/set", HTTP_POST, handleSet);
    server.begin();
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
}
