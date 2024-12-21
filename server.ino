#include <WiFi.h>
#include "head.h"

#define SSID "TestWiFi"
#define PASSWORD "12345678"

void runServer() {
  // TODO
  delay(1000);
  //startCameraBased();
  testCamera();
}

void testCamera() {
  Serial.println("Initializing Camera");
  initCamera();
  Serial.println("Initialized Camera");
  while (true) {
    struct HsvColor hsv;
    doCapturing(&hsv);
  }  
}

void startServer() {
  //WiFi.begin(SSID, PASSWORD);
  //while (WiFi.status() != WL_CONNECTED) {delay(500)}
  //Serial.println("Connected");
  runServer();
}

void sendResult(double uL) {
  // TODO
  Serial.printf("%lf\n", uL);   // Debugging
}