#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Servo.h>
#include <SimpleTimer.h>

SimpleTimer timer;

Servo servo;

int delay_time=5000;

/* Set these to your desired credentials. */
const char *softAP_ssid = "KBD_AP";
const char *softAP_password = "00000000";
/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

ESP8266WebServer server(80);


void setup(void) {
  
  servo.attach(15);
  Serial.begin(9600);

  timer.setInterval(delay_time,feed);    

  setupAp();

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/auto", handleAuto);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void setupAp() {
  Serial.println(softAP_ssid);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(softAP_ssid, softAP_password);
  delay(500); // Without delay I've seen the IP address blank
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void loop(void) {
  servo.write(160);
  server.handleClient();
  timer.run();
  
  
}
void feed(){
  servo.write(160);
  delay(15);
  servo.write(40);
  delay(300);
  servo.write(160);
  delay(15);
  Serial.println("1");
}

void handleRoot() {
  
  String s; 
  s="<meta name='viewport' content='width=device-width, initial-scale=1.0'/>";
  //s=s+"<meta http-equiv='refresh' content='5'/>";
  s=s+"<meta http-equiv='Content-Type' content='text/html;charset=utf-8' />";
  s=s+"&nbsp&nbsp";
  s=s+"<h1 align='center'>자동 배식기</h1>";
  s=s+"&nbsp&nbsp";
  s=s+"<br></br>";
  s=s+"<a href=\"on\"><button style='background-color: #4CAF50; border: none;color: white; padding: 42px; text-align: center; text-decoration: none; display: inline-block; font-size: 19px; margin: 4px 2px; cursor: pointer; border-radius: 12px;'>수동 배식</button></a>";
  s=s+"&nbsp&nbsp&nbsp&nbsp";
  s=s+"<a href=\"auto\"><button style='background-color: #66CCFF; border: none;color: white; padding: 42px; text-align: center; text-decoration: none; display: inline-block; font-size: 19px; margin: 4px 2px; cursor: pointer; border-radius: 12px;'>자동 배식</button></a>";
  server.send(200, "text/html", s);
}

void handleOn() {
  feed();
  timer.disable(0);
  GoHome();
}

void handleAuto() {

  timer.enable(0);
  GoHome();
  
}



void GoHome() {
  //IPAddress ip = WiFi.localIP();
  IPAddress ip = apIP;
  String s,ipS;
  ipS=String(ip[0])+"."+String(ip[1])+"."+String(ip[2])+"."+String(ip[3]);
    s="<meta http-equiv='refresh' content=\"0;url='http://"+ipS+"/'\">";
  server.send(200, "text/html", s);
}

void handleNotFound() {

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  
}
