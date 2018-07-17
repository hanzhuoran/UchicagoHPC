#include "utils.h"

double rand_double(double start, double end) {

	/*
	* return a random double between start and end.
	*/

	double r = rand() / (double) RAND_MAX;
	double x = (start < end) ? (start) : (end);
	double y = (start < end) ? (end) : (start);
	return (y - x) * r + x;
}


vec3d randomDirection() {

	/*
	* return a unit vector of random direction. Alg 1
	*/

	double angle_psi, angle_theta;
	vec3d dest;

	angle_psi = rand_double(0, 2 * PI);
	angle_theta = rand_double(0, PI);

	dest.x = sin(angle_theta) * cos(angle_psi);
	dest.y = sin(angle_theta) * sin(angle_psi);
	dest.z = cos(angle_theta);

	return dest;
}

void save_grid(double * data, int px, int py, char * fname) {

	FILE * fp = fopen(fname, "w");
	int i, j;

	for (i=0; i<py; i++) {
		for (j=0; j<px; j++) {
			fprintf(fp, "%lf ", data[i * px + j]);
		} fprintf(fp, "\n");
	}

	fclose(fp);
}
