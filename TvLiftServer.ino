#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

char ssid[100] = "crocodil";
char password[100] = "*";

ESP8266WebServer server(80);

const int ENGINE_RELAY_1 = D0;
const int ENGINE_RELAY_2 = D5;
const int ENGINE_CUT_OFF_RELAY = D4;
const int LED_RELAY_BACKLIGHT = D2;
const int LED_RELAY_FORELIGHT = D3;
const int TV_UP_WRITE = 1;
const int TV_DOWN_WRITE = 0;
const int RELAY_OFF = 1;
const int RELAY_ON = 0;

int ENGINE_WORK_TIME_MS = 1000 * 10; // 10 seconds default

int g_ForeLightStatus = RELAY_ON;
int g_BackLightStatus = RELAY_OFF;

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}

void handleTvUp() {
  digitalWrite(LED_RELAY_FORELIGHT, RELAY_OFF);

  digitalWrite(LED_RELAY_BACKLIGHT, RELAY_ON);
  changeTV(TV_UP_WRITE);
  server.send(200, "text/plain", "up");
  powerEngine();
  
  delay(5000);
  digitalWrite(LED_RELAY_BACKLIGHT, RELAY_OFF);
}

void handleTvDown() {
  changeTV(TV_DOWN_WRITE);
  server.send(200, "text/plain", "down");
  powerEngine();
  digitalWrite(LED_RELAY_FORELIGHT, RELAY_ON);
}

void changeTV(int direction) {
  digitalWrite(ENGINE_RELAY_1,direction);
  digitalWrite(ENGINE_RELAY_2,direction);
}

void powerEngine(){
    digitalWrite(ENGINE_CUT_OFF_RELAY,RELAY_ON);
    delay(ENGINE_WORK_TIME_MS);
    digitalWrite(ENGINE_CUT_OFF_RELAY,RELAY_OFF);
}
void foreLight() {
  if (g_ForeLightStatus == RELAY_ON){
    g_ForeLightStatus = RELAY_OFF;
  } else {
    g_ForeLightStatus = RELAY_ON;
  }
  digitalWrite(LED_RELAY_FORELIGHT, g_ForeLightStatus);

  server.send(200, "text/plain", "");
}
void backLight() {
  if (g_BackLightStatus == RELAY_ON){
    g_BackLightStatus = RELAY_OFF;
  } else {
    g_BackLightStatus = RELAY_ON;
  }
  digitalWrite(LED_RELAY_BACKLIGHT, g_BackLightStatus);
  server.send(200, "text/plain", "");
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
  server.send(200, "text/plain", "server will try to connect to new wifi");
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
  server.on("/foreLight", foreLight);
  server.on("/backLight", backLight);
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
    
   digitalWrite(ENGINE_RELAY_1, RELAY_OFF);
   digitalWrite(ENGINE_RELAY_2, RELAY_OFF);
   digitalWrite(ENGINE_CUT_OFF_RELAY, RELAY_OFF);
   digitalWrite(LED_RELAY_BACKLIGHT, RELAY_OFF);
   digitalWrite(LED_RELAY_FORELIGHT, RELAY_ON);
}
