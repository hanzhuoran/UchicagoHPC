#ifndef UTILS_H
#define UTILS_H

#include "vec.h" 
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#ifndef PI
#define PI 3.1415926535897
#endif

/** Utils */
double rand_double(double x, double y);
vec3d randomDirection();
void save_grid(double * data, int px, int py, char * fname);

#endif