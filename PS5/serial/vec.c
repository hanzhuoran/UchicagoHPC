#include "vec.h"


/** Vector set */
vec3d setvec(double x, double y, double z) {

	vec3d vec;

	vec.x = x;
	vec.y = y;
	vec.z = z;

	return vec;
}

/** Vector operation */
vec3d vecmul(vec3d vec1, double scaler){
	vec3d vec;

	vec.x = vec1.x * scaler;
	vec.y = vec1.y * scaler;
	vec.z = vec1.z * scaler;

	return vec;

}

vec3d vecadd(vec3d vec1, vec3d vec2){
	vec3d vec;

	vec.x = vec1.x + vec2.x ;
	vec.y = vec1.y + vec2.y ;
	vec.z = vec1.z + vec2.z;

	return vec;
}

vec3d vecsub(vec3d vec1, vec3d vec2){
	vec3d vec;

	vec.x = vec1.x - vec2.x ;
	vec.y = vec1.y - vec2.y ;
	vec.z = vec1.z - vec2.z;

	return vec;
}

/** Vector dot production */
double vecdot(vec3d vec1, vec3d vec2){
	double ans = 0;

	// printf("vx %lf \n", vec1.x);
	// printf("vy %lf \n", vec1.y);
	// printf("vz %lf \n", vec1.z);


	// printf("cx %lf \n", vec2.x);
	// printf("cy %lf \n", vec2.y);
	// printf("cz %lf \n", vec2.z);
	ans = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;

	return ans;
}

/** Vector Norm */
double norm(vec3d vec1){
	double ans = 0;

	ans = sqrt(vec1.x * vec1.x + vec1.y * vec1.y + vec1.z * vec1.z);

	return ans;
}

