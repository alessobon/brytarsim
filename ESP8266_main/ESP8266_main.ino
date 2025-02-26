#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "brytarsim1";
const char *password = "brytarsim1";
ESP8266WebServer server(80);

int franslagstidA = 40;
int tillslagstidA = 60;
bool debugMode = true;

void debugPrint(const String &message) {
    if (debugMode) {
        Serial.println(message);
    }
}

void handleRoot() {
    debugPrint("Serving root page");
    String page = "<!DOCTYPE html><html><head><title>Brytarsim 1</title></head><body>";
    page += "<h1>Brytarsim 1</h1>";
    page += "<form action='/set' method='POST' onsubmit='return submitForm();'>";
    page += "<label>FRÅNSLAGSTID A (ms):</label>";
    page += "<input type='number' name='tUMA' value='" + String(franslagstidA) + "'><br><br>";
    page += "<label>TILLSLAGSTID A (ms):</label>";
    page += "<input type='number' name='tTMA' value='" + String(tillslagstidA) + "'><br><br>";
    page += "<input type='submit' value='Set'>";
    page += "</form>";
    page += "<script>function submitForm() { fetch('/set', {method: 'POST', body: new FormData(document.forms[0])}).then(() => window.location.reload()); return false; }</script>";
    page += "</body></html>";
    server.send(200, "text/html", page);
}

void handleSet() {
    if (server.hasArg("tUMA") && server.hasArg("tTMA")) {
        franslagstidA = server.arg("tUMA").toInt();
        tillslagstidA = server.arg("tTMA").toInt();
        debugPrint("Updated FRÅNSLAGSTID A to: " + String(franslagstidA));
        debugPrint("Updated TILLSLAGSTID A to: " + String(tillslagstidA));
        server.send(200, "text/plain", "Value updated");
    } else {
        debugPrint("Invalid request to /set");
        server.send(400, "text/plain", "Bad Request");
    }
}

void setup() {
    Serial.begin(115200);
    debugPrint("Starting ESP8266...");
    WiFi.softAP(ssid, password);
    IPAddress myIP = WiFi.softAPIP();
    debugPrint("WiFi hotspot started with SSID: " + String(ssid));
    debugPrint("Access Point IP Address: " + myIP.toString());
    
    if (MDNS.begin("brytarsim")) {
        debugPrint("mDNS responder started at brytarsim.local");
    } else {
        debugPrint("Error setting up mDNS responder");
    }
    
    server.on("/", handleRoot);
    server.on("/set", HTTP_POST, handleSet);
    server.begin();
    debugPrint("HTTP Server started");
}

void loop() {
    MDNS.update();
    server.handleClient();
}
