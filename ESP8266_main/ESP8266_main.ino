#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "brytarsim1";
const char *password = "brytarsim1";
ESP8266WebServer server(80);

int franslagstidA = 40;
int tillslagstidA = 60;
int franslagstidB = 40;
int tillslagstidB = 60;
bool A_till = false;
bool A_fran = true;
bool B_till = false;
bool B_fran = true;
bool debugMode = true;

void debugPrint(const String &message) {
    if (debugMode) {
        Serial.println(message);
    }
}

void handleRoot() {
    debugPrint("Serving root page");
    String page = "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>Brytarsim 1</title></head><body>";
    page += "<h1>Brytarsim 1</h1>";
    page += "<br>Konfigurera brytare<br>";
    page += "<form action='/set' method='POST' onsubmit='return submitForm(0);'>";
    page += "<label>FRÅNSLAGSTID A (ms):</label>";
    page += "<input type='number' name='tUMA' value='" + String(franslagstidA) + "'><br><br>";
    page += "<label>TILLSLAGSTID A (ms):</label>";
    page += "<input type='number' name='tTMA' value='" + String(tillslagstidA) + "'><br><br>";
    page += "<label>FRÅNSLAGSTID B (ms):</label>";
    page += "<input type='number' name='tUMB' value='" + String(franslagstidB) + "'><br><br>";
    page += "<label>TILLSLAGSTID B (ms):</label>";
    page += "<input type='number' name='tTMB' value='" + String(tillslagstidB) + "'><br><br>";
    page += "<input type='submit' value='Skicka'>";
    page += "</form>";
    page += "<br><br>Forcera lägen<br>";
    page += "<form action='/force' method='POST' onsubmit='return submitForm(1);'>";
    page += "<label>A TILL:</label>";
    page += "<input type='checkbox' name='A_till' value='checked'";
    if (A_till) {
      page += " checked ";
    }
    page += "><br><br>";
    page += "<label>A FRÅN:</label>";
    page += "<input type='checkbox' name='A_fran' value='checked'";
    if (A_fran) {
      page += " checked ";
    }
    page += "><br><br>";
    page += "<label>B TILL:</label>";
    page += "<input type='checkbox' name='B_till' value='checked'";
    if (B_till) {
      page += " checked ";
    }
    page += "><br><br>";
    page += "<label>B FRÅN:</label>";
    page += "<input type='checkbox' name='B_fran' value='checked'";
    if (B_fran) {
      page += " checked ";
    }
    page += "><br><br>";
    page += "<input type='submit' value='Forcera'>";
    page += "</form>";
    page += "<script>function submitForm(formid) { fetch('/force', {method: 'POST', body: new FormData(document.forms[formid])}).then(() => window.location.reload()); return false; }</script>";
    page += "</body></html>";
    server.send(200, "text/html", page);
}

void handleSet() {
    if (server.hasArg("tUMA") && server.hasArg("tTMA") && server.hasArg("tUMB") && server.hasArg("tTMB")) {
        franslagstidA = server.arg("tUMA").toInt();
        tillslagstidA = server.arg("tTMA").toInt();
        franslagstidB = server.arg("tUMB").toInt();
        tillslagstidB = server.arg("tTMB").toInt();
        debugPrint("Updated FRÅNSLAGSTID A to: " + String(franslagstidA));
        debugPrint("Updated TILLSLAGSTID A to: " + String(tillslagstidA));
        debugPrint("Updated FRÅNSLAGSTID B to: " + String(franslagstidB));
        debugPrint("Updated TILLSLAGSTID B to: " + String(tillslagstidB));
        server.send(200, "text/plain", "Value updated");
    } else {
        debugPrint("Invalid request to /set");
        server.send(400, "text/plain", "Bad Request");
    }
}
void handleForce() {
    if (server.hasArg("A_till")) {
        A_till = true;
    } else {
        A_till = false;
    }
    if (server.hasArg("A_fran")) {
        A_fran = true;
    } else {
        A_fran = false;
    }
    if (server.hasArg("B_till")) {
        B_till = true;
    } else {
        B_till = false;
    }
    if (server.hasArg("A_fran")) {
        B_fran = true;
    } else {
        B_fran = false;
    }
    debugPrint("A TILL = " + String(A_till));
    debugPrint("A FRÅN = " + String(A_fran));
    debugPrint("B TILL = " + String(B_till));
    debugPrint("B FRÅN = " + String(B_fran));
    server.send(200, "text/plain", "Value forced");
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
    server.on("/force", HTTP_POST, handleForce);
    server.begin();
    debugPrint("HTTP Server started");

    pinMode(LED_BUILTIN, OUTPUT); // init built in led
}

void loop() {
    MDNS.update();
    server.handleClient();
    if (A_till) {
      digitalWrite(LED_BUILTIN, LOW);
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
    }
}
