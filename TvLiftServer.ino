#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

char ssid[100] = "****";
char password[100] = "****";

ESP8266WebServer server(80);

const int ENGINE_RELAY_1 = D0;
const int ENGINE_RELAY_2 = D5;
const int ENGINE_CUT_OFF_RELAY = D4;
const int LED_RELAY_BACKLIGHT = D2;
const int LED_RELAY_FORELIGHT = D3;
const int TV_UP_WRITE = 1;
const int TV_DOWN_WRITE = 0;

int ENGINE_WORK_TIME_MS = 1000 * 10; // 10 seconds default

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}

void handleTvUp() {
  digitalWrite(LED_RELAY_BACKLIGHT, 1);

  digitalWrite(LED_RELAY_BACKLIGHT, 0);
  changeTV(TV_UP_WRITE);
  server.send(200, "text/plain", "up");
  powerEngine();
  
  delay(5000);
  digitalWrite(LED_RELAY_BACKLIGHT, 1);
}

void handleTvDown() {
  changeTV(TV_DOWN_WRITE);
  server.send(200, "text/plain", "down");
  powerEngine();
  digitalWrite(LED_RELAY_BACKLIGHT, 0);
}

void changeTV(int direction) {
  digitalWrite(ENGINE_RELAY_1,direction);
  digitalWrite(ENGINE_RELAY_2,direction);
}

void powerEngine(){
    digitalWrite(ENGINE_CUT_OFF_RELAY,0);
    delay(ENGINE_WORK_TIME_MS);
    digitalWrite(ENGINE_CUT_OFF_RELAY,1);
}
void handleLights() {
  String action = server.arg(0);
}

void setEngineWorkTime(){
    String timeStr = server.arg(0);
    if (timeStr.toInt() > 0){
       ENGINE_WORK_TIME_MS = timeStr.toInt();
    }
      server.send(200, "text/plain", timeStr);
}

void handleNotFound(){
  server.send(404, "text/plain", "File Not Found");
}

void changeWifiPass(){
  server.arg(0).toCharArray(ssid,100);
  server.arg(1).toCharArray(password,100);
  setup();
}

void setup(void){
  setRelays();
 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");
  int noConnTime = 0;
  
  // Wait for connection
  while ((WiFi.status() != WL_CONNECTED) && (noConnTime < 20000)) {
    delay(500);
    noConnTime += 500;
    Serial.print(WiFi.status());

    if (noConnTime > 20000) {
      WiFi.softAP("TheTreasureIsExposed", "123456");
    }
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/tvUp", handleTvUp);
  server.on("/tvDown", handleTvDown);
  server.on("/light", handleLights);
  server.on("/setEngineWorkTime", setEngineWorkTime);
  server.on("/changeWifiPass", setEngineWorkTime);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}

void setRelays(){
   pinMode(ENGINE_RELAY_1, OUTPUT);
   pinMode(ENGINE_RELAY_2, OUTPUT);
   pinMode(ENGINE_CUT_OFF_RELAY, OUTPUT);
   pinMode(LED_RELAY_BACKLIGHT, OUTPUT);
   pinMode(LED_RELAY_FORELIGHT, OUTPUT);
    
   digitalWrite(ENGINE_RELAY_1, 1);
   digitalWrite(ENGINE_RELAY_2, 1);
   digitalWrite(ENGINE_CUT_OFF_RELAY, 1);
   digitalWrite(LED_RELAY_BACKLIGHT, 1);
   digitalWrite(LED_RELAY_FORELIGHT, 0);
}
