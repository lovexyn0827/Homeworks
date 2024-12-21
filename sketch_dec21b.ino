#include "head.h"

#define TIME_PER_DRIP 1000

#define TIME_BEFORE_CAPTURING 1000

#define COLOR_CHANGE_THRESHOLD 0.1
#define COLOR_CHANGE_TIME_THRESHOLD 30000

#define ADC_RECORD_INTERVAL TIME_PER_DRIP

#define ADC_PIN 3

#define TOTAL_VOLUME_UL 50000

#define USE_EXTREME_AS_INDICATOR true

#define EXPECTED_RESULT_COUNT 1

void setup() {
  Serial.begin(115200);
  delay(300);
  pinMode(ADC_PIN, INPUT);
  startServer(); 
  // It is up to the server to offer further insrtuctions.
}

void loop() {
}

float hsvDelta(struct HsvColor *c1, struct HsvColor *c2, float hWeight, float sWeight, float vWeight) {
  float dh = (c1->h - c2->h) * hWeight;
  float ds = (c1->s - c2->s) * sWeight;
  float dv = (c1->v - c2->v) * vWeight;
  return dh * dh + ds * ds + dv * dv;
}

void startCameraBased() {
  struct HsvColor original, current;
  initCamera();
  doCapturing(&original);
  resetCounter();
  int resultToCome = EXPECTED_RESULT_COUNT;
  while (true) {
    pumpAndWait(TIME_PER_DRIP, TIME_BEFORE_CAPTURING);
    doCapturing(&current);
    if (hsvDelta(&original, &current, 1, 0, 0) > COLOR_CHANGE_THRESHOLD) {
      delay(TIME_PER_DRIP);
      doCapturing(&current);
      if (hsvDelta(&original, &current, 1, 0, 0) > COLOR_CHANGE_THRESHOLD) {
        sendResult(getPumpedVolume());
        if (--resultToCome == 0) {
          return;
        } else {
          original.h = current.h;
          original.s = current.s;
          original.v = current.v;
        }
      }
    }
  }
}

double diffClock(clock_t t0, clock_t t1) {
    return (t0 - t1) / ((double) CLOCKS_PER_SEC);
}

void startPotentionalBased() {
  struct IntSequence timeOfMeasures, ins, dins, ddins;
  initIntSeq(&timeOfMeasures);
  initIntSeq(&ins);
  startPumping();
  clock_t t0 = clock();
  while (diffClock(clock(), t0) * 1000 < 50000 / UL_PER_MS) {
    double sumAdcIn = 0;
    int adcSampleCnt = 0;
    clock_t prev = clock();
    while (diffClock(clock(), prev) * 1000 < ADC_RECORD_INTERVAL) {
      sumAdcIn += analogRead(ADC_PIN);
      adcSampleCnt++;
    }

    append(&timeOfMeasures, (int) (diffClock(clock(), t0) * 1000));
    append(&ins, (int) (sumAdcIn / adcSampleCnt));
  }

  stopPumping();
  initIntSeq(&dins);
  initIntSeq(&ddins);
  if (USE_EXTREME_AS_INDICATOR) {
    differentiate(&ins, &dins);
    int start = 0;
    int end = nextFlatSegument(&dins, 0);
    for (int i = 0; i < EXPECTED_RESULT_COUNT; i++) {
      int end = nextFlatSegument(&dins, 0);
      int maxPos = findExtreme(&dins, start, end, true);
      int minPos = findExtreme(&dins, start, end, false);
      int extPos = (dins.values[maxPos] + dins.values[minPos] > 0) ? maxPos : minPos;
      sendResult(timeOfMeasures.values[extPos] * UL_PER_MS);
      start = end;
    }
  } else {
    differentiate2Deg(&ins, &ddins);
    int start = 0;
    int end = nextFlatSegument(&ddins, 0);
    for (int i = 0; i < EXPECTED_RESULT_COUNT; i++) {
      int end = nextFlatSegument(&ddins, 0);
      float zero = findValidZeroAsInflPoint(&ddins, start, end);
      sendResult(timeOfMeasures.values[(int) zero] * UL_PER_MS);
      start = end;
    }
  }
}