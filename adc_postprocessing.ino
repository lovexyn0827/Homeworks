#include <stdlib.h>
#include "head.h"

void onOutOfMemory() {
  // TODO
}

void initIntSeq(struct IntSequence *p) {
  p->values = (int*) malloc(256 * sizeof(int));
  if (p->values == NULL) {
    onOutOfMemory();
  }

  p->length = 0;
  p->capacity = 256;
}

void append(struct IntSequence *p, int val) {
  if (p->length == p->capacity) {
    p->values = (int*) realloc(p->values, (p->capacity + 256) * sizeof(int));
    if (p->values == NULL) {
      onOutOfMemory();
    }
  }

  p->values[p->length++] = val;
}

void differentiate(struct IntSequence *p, struct IntSequence *dst) {
  int prev = 0;
  bool firstLoop = true;
  for (int i = 0; i < p->length; i++) {
    if (firstLoop) {
      firstLoop = false;
      append(dst, 0);
    } else {
      append(dst, p->values[i] - prev);
    }

    prev = p->values[i];
  }
}

void differentiate2Deg(struct IntSequence *p, struct IntSequence *dst) {
  int prev = 0;
  int pprev = 0;
  int loopCnt = 0;
  for (int i = 0; i < p->length; i++) {
    if (loopCnt < 2) {
      append(dst, 0);
      loopCnt++;
    } else {
      append(dst, p->values[i] - 2 * prev + pprev);
    }

    pprev = prev;
    prev = p->values[i];
  }
}

int findExtreme(struct IntSequence *p, int minIdx, int maxIdx, bool findMax) {
  int extreme = p->values[minIdx];
  int extremeIdx = minIdx;
  for (int i = minIdx + 1; i < maxIdx; i++) {
    if (findMax ^ (p->values[i] <= extreme)) {
      extreme = p->values[i];
      extremeIdx = i;
    }
  }

  return extremeIdx;
}

float findFirstZeroWithInterpolation(struct IntSequence *p, int minIdx, int maxIdx) {
  for (int i = minIdx + 1; i < maxIdx; i++) {
    if (((p->values[minIdx] < 0) ^ (p->values[i] < 0)) || (p->values[i] == 0)) {
      // Doing some linear regression may help
      return i - p->values[i] / ((float) (p->values[i] - p->values[i - 1]));
    }
  }

  return -1;
}

float findValidZeroAsInflPoint(struct IntSequence *p, int minIdx, int maxIdx) {
  int minPos = findExtreme(p, minIdx, maxIdx, false);
  int maxPos = findExtreme(p, minIdx, maxIdx, true);
  int formerPeak = (minPos < maxPos) ? minPos : maxPos;
  int latterPeak = (minPos > maxPos) ? minPos : maxPos;
  return findFirstZeroWithInterpolation(p, formerPeak, latterPeak + 1);
}

int nextFlatSegument(struct IntSequence *p, int from) {
  return p->length;  // TODO
}
