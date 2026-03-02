#ifndef FLOAT_MAP_GUARD
#define FLOAT_MAP_GUARD

float map_float(float x, float in_min, float in_max, float out_min, float out_max) {
  return out_min + (x - in_min) * (out_max - out_min) / (in_max - in_min);
}

double map_double(double x, double in_min, double in_max, double out_min, double out_max) {
  return out_min + (x - in_min) * (out_max - out_min) / (in_max - in_min);
}

#endif