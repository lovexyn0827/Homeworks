#ifndef HEAD_INCLUDED

#define HEAD_INCLUDED

struct IntSequence {
  int *values;
  int length;
  int capacity;
};

struct HsvColor {
  float h;
  float s;
  float v;
} HsvColor;

#define UL_PER_MS 0.008333

#endif