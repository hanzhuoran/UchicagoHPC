#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <omp.h>
#include "vec.h"
#include "utils.h"

#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif

typedef struct {
	vec3d pos;
	double height, width;
} Camera;

/** Ray Tracer */
void rayTrace(int px, int py, int nrays, Camera camera);
void setArgs(int argc, char** argv);

/** Global variables */
int num_pixel;
int num_ray;
int num_thread;

int main(int argc, char** argv) {
	
	//Default Set Ups
	num_pixel = 1000;
	num_ray = 1e7;
	num_thread = 1;
	Camera cam;

	cam.pos = setvec(0.0, 10.0, 0.0);
	cam.height = 20.0;
	cam.width = 20.0;

	//Set input arguments
	setArgs(argc, argv);

	double dt = omp_get_wtime();
	rayTrace(num_pixel, num_pixel, num_ray, cam);
	dt = omp_get_wtime() - dt;

	printf("total num of pxiels: %d \n", num_pixel*num_pixel);
	printf("total num of rays: %d \n", num_ray);
	printf("time: %lf \n",dt);
}

void rayTrace(int px, int py, int nrays, Camera camera) {

	/** Basic variables */
	int n;

	/** Model parameters */
	double radius = 6.0;
	vec3d vec_c = setvec(0.0, 12.0, 0.0); // position of sphere centre
	vec3d vec_l = setvec(4.0, 4.0, -1.0); // position of light source

	/** Ray tracer variables global */
	double dotp_cc;

	/** Window display */
	double w_max_x, w_min_x, w_max_z, w_min_z;
	double * mat_grid; // vision grid

	/** Initialization */
	mat_grid = (double*) malloc(px * py * sizeof(double));
	memset(mat_grid, 0.0, px * py);
	w_max_x = camera.pos.x + camera.width * 0.5;
	w_min_x = camera.pos.x - camera.width * 0.5;
	w_max_z = camera.pos.z + camera.height * 0.5;
	w_min_z = camera.pos.z - camera.height * 0.5;
	dotp_cc = vecdot(vec_c, vec_c);

	omp_set_num_threads(num_thread);
	#pragma omp parallel for schedule(guided) shared(mat_grid) private(n)
	for (n=0; n<nrays; n++) {

		/** Ray tracer variables local */
		int i, j;
		double delta, t, brightness;
		double dotp_vc;
		double scaler;
		vec3d vec_v; // view ray vector
		vec3d vec_i; // position of intersection
		vec3d vec_s; // direction of light source at I
		vec3d vec_n; // unit normal vector at I
		vec3d vec_w; // camera vector

		do { // sample random V from unit sphere
			vec_v = randomDirection();

			scaler = camera.pos.y/vec_v.y;
			// printf("scaler is %lf\n", scaler);
			vec_w = vecmul(vec_v, scaler);

			// printf("vx %lf \n", vec_v.x);
			// printf("vy %lf \n", vec_v.y);
			// printf("vz %lf \n", vec_v.z);


			// printf("cx %lf \n", vec_c.x);
			// printf("cy %lf \n", vec_c.y);
			// printf("cz %lf \n", vec_c.z);
			
			dotp_vc = vecdot(vec_v, vec_c);
			// printf("v dot c = %lf \n", dotp_vc);
			delta = dotp_vc*dotp_vc + radius*radius - dotp_cc;
		} while (vec_w.x < w_min_x || vec_w.x > w_max_x || vec_w.z < w_min_z || vec_w.z > w_max_z || delta < 0);

		t = dotp_vc - sqrt(delta);
		vec_i = vecmul(vec_v, t);
		vec_n = vecsub(vec_i, vec_c);
		vec_n = vecmul(vec_n, 1.0 / norm(vec_n));
		vec_s = vecsub(vec_l, vec_i);
		vec_s = vecmul(vec_s, 1.0 / norm(vec_s));
		// printf("%lf \n", vec_s.x);

		brightness = vecdot(vec_s, vec_n);
		// printf("brightness: %lf \n",brightness);
		brightness = MAX(brightness,0);

		j = px - 1 - (int) ((double) px * (vec_w.x - w_min_x) / (camera.width));
		i = (int) ((double) py * (vec_w.z - w_min_z) / (camera.height));
		
		/** To check progress */
		//if (n%(nrays/100)==0) {printf("."); fflush(stdout);}
		// if (n%(nrays/100)==0) write(STDOUT_FILENO, ".", 2);

		#pragma omp atomic update
		mat_grid[i * px + j] += brightness;
	}

	save_grid(mat_grid, px, py, "output.cpu.out");

	free(mat_grid);
}


void setArgs(int argc, char** argv) {

	/*
	* getopt_long stores the option index here.
	*/

	int option_index = 0;
	int ch;
	char *optstring = "n:r:t:";

	static struct option long_options[] = {
		{"pixel", required_argument, NULL, 'n'},
		{"ray", required_argument, NULL, 'r'},
		{"thread", required_argument, NULL, 't'},
		{0, 0, 0, 0}
	};

	/* Detect the end of the options. */
	while ( (ch = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1 ) {
		switch (ch) {
			case 'n':
				num_pixel = atoi(optarg);
				break;
			case 'r':
				num_ray = atoi(optarg);
				break;
			case 't':
				num_thread = atoi(optarg);
				break;
			case '?':
				printf("Unknown option\n");
				break;
			case 0:
				break;
		}
	}
}
