#include "head.h"

#define PUMP_PIN 4

#define INERTIA_ADJUSTMENT 15

int counter;

void resetCounter() {
  counter = 0;
}

void pump(int ms) {
  digitalWrite(PUMP_PIN, HIGH);
  delay(ms);
  digitalWrite(PUMP_PIN, LOW);
  counter += ms + INERTIA_ADJUSTMENT;
}

void pumpAndWait(int ms, int wait) {
  pump(ms);
  delay(wait);
}

double getPumpedVolume() {
  return counter * UL_PER_MS;
}

void startPumping() {
  digitalWrite(PUMP_PIN, HIGH);
}

void stopPumping() {
  digitalWrite(PUMP_PIN, LOW);
}