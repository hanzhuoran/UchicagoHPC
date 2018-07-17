#ifndef VEC_H
#define VEC_H
#include <math.h>

typedef struct {
	double x, y, z;
} vec3d;

/** Vector set */
vec3d setvec(double x, double y, double z);

/** Vector operation */
vec3d vecmul(vec3d vec1, double scaler);
vec3d vecadd(vec3d vec1, vec3d vec2);
vec3d vecsub(vec3d vec1, vec3d vec2);

/** Vector dot production */
double vecdot(vec3d vec1, vec3d vec2);
double norm(vec3d vec1);

#endif