#include "nbody_header.h"

#ifdef MPI
void run_parallel_problem(long nBodies, double dt, long nIters, char * fname) {

	int mype, nprocs;
	long nglobal, nlocal, nremote, offset;
	MPI_Status status;
	MPI_File fh;

	// MPI Issues
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &mype);

	// Create an MPI type for Body
	const int num_member = 7;
	MPI_Datatype TYPE_BODY;
	
	MPI_Aint blockoffsets[7];
	blockoffsets[0] = offsetof(Body, x);
	blockoffsets[1] = offsetof(Body, y);
	blockoffsets[2] = offsetof(Body, z);
	blockoffsets[3] = offsetof(Body, vx);
	blockoffsets[4] = offsetof(Body, vy);
	blockoffsets[5] = offsetof(Body, vz);
	blockoffsets[6] = offsetof(Body, mass);
	int blocklength[7] = {1, 1, 1, 1, 1, 1, 1};
	MPI_Datatype types[7] = {MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE};
	
	MPI_Type_create_struct(num_member, blocklength, blockoffsets, types, &TYPE_BODY);
	MPI_Type_commit(&TYPE_BODY);

	// Get this rank's job size and offset
	nglobal = nBodies / nprocs + 1;
	nlocal = jobs_manage(nBodies, mype, nprocs, &offset);

	// Allocate Bodies
	Body * locals    = (Body *) calloc(nglobal, sizeof(Body));
	Body * incomings = (Body *) calloc(nglobal, sizeof(Body));
	Body * outgoings = (Body *) calloc(nglobal, sizeof(Body));
	Body * switching;

	// Generate a series of random particles identical against time
	if (mype == 0) {
		// Rank 0 generates for itself
		parallel_randomizeBodies(locals, nBodies, mype, nprocs);
		// Rank 0 generates for rest of ranks
		for (int i = 1; i < nprocs; i++) {
			parallel_randomizeBodies(outgoings, nBodies, mype, nprocs);
			MPI_Send(outgoings, nglobal, TYPE_BODY, i, 1, MPI_COMM_WORLD);
		}
	} else {
		// Rank i receives particles from Rank 0
		MPI_Recv(locals, nglobal, TYPE_BODY, 0, 1, MPI_COMM_WORLD, &status);
	}

	fh = initialize_IO(nBodies, nIters, fname, mype);
	double start = get_time();

	// Loop over timesteps
	for (long iter = 0; iter < nIters; iter++) {

		// if (mype == 0 && (iter*100) % nIters == 0 ) write(STDOUT_FILENO, ".", 2);

		// Output body positions to file
		distributed_write_timestep(locals, nBodies, iter, nIters, nprocs, mype, &fh);

		// Copy local particles to remote buffer
		memcpy(incomings, locals, nlocal * sizeof(Body));

		for (int rank = 0; rank < nprocs; rank++) {
			// Now it's time to calculate particles in Rank i and Rank i+rank
			nremote = jobs_manage(nBodies, (mype + rank) % nprocs, nprocs, NULL);
			// Compute new forces & velocities for all particles
			compute_forces_multi_set(locals, incomings, dt, nlocal, nremote);
			// Switch buffers: incomings <=> outgoings
			switching = incomings; incomings = outgoings; outgoings = switching;
			// Roll the buffer: (i-1) <= i <= (i+1)
			int prev = (mype - 1 + nprocs) % nprocs;
			int next = (mype + 1) % nprocs;
			MPI_Sendrecv(
				outgoings, nglobal, TYPE_BODY, prev, 1,
				incomings, nglobal, TYPE_BODY, next, 1,
				MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		// Update positions of all particles
		for (int i = 0 ; i < nlocal; i++) {
			locals[i].x += locals[i].vx*dt;
			locals[i].y += locals[i].vy*dt;
			locals[i].z += locals[i].vz*dt;
		}
	}

	// Close data file
	MPI_File_close(&fh);
	MPI_Type_free(&TYPE_BODY);

	double stop = get_time();

	double runtime = stop-start;
	double time_per_iter = runtime / (double) nIters;
	long interactions = nBodies * nBodies;
	double interactions_per_sec = (double) interactions / time_per_iter;

	if (mype == 0) {
		printf("SIMULATION COMPLETE\n");
		printf("Runtime [s]:              %.3le\n", runtime);
		printf("Runtime per Timestep [s]: %.3le\n", time_per_iter);
		printf("interactions:             %ld\n", nIters);
		printf("Interactions per sec:     %.3le\n", interactions_per_sec);
	}

	free(locals);
	free(incomings);
	free(outgoings);
}

void compute_forces_multi_set(Body * local, Body * remote, double dt, long nlocal, long nremote) {

	double G = 6.67259e-3;
	double softening = 1.0e-5;

	// For each particle in the local set
	#ifdef OPENMP
	#pragma omp parallel for schedule(static) shared(local, remote, nlocal, nremote, dt, G, softening)
	#endif
	for (long i = 0; i < nlocal; i++) {

		double Fx = 0.0;
		double Fy = 0.0;
		double Fz = 0.0;

		for (long j = 0; j < nremote; j++) {

			double dx = remote[j].x - local[i].x;
			double dy = remote[j].y - local[i].y;
			double dz = remote[j].z - local[i].z;

			double distance = sqrt(dx*dx + dy*dy + dz*dz + softening);
			double distance_cubed = distance * distance * distance;

			double m_j = local[j].mass;
			double mGd = G * m_j / distance_cubed;
			Fx += mGd * dx;
			Fy += mGd * dy;
			Fz += mGd * dz;
		}

		local[i].vx += dt*Fx;
		local[i].vy += dt*Fy;
		local[i].vz += dt*Fz;
	}
}

void parallel_randomizeBodies(Body * bodies, long nBodies, int mype, int nprocs) {

	// velocity scaling term
	double vm = 1.0e-3;

	long nBodies_per_rank = jobs_manage(nBodies, mype, nprocs, NULL);

	#ifdef OPENMP
	#pragma omp parallel for schedule(static) shared(bodies, nBodies)
	#endif
	for (long i = 0; i < nBodies_per_rank; i++) {
		// Initialize position between -1.0 and 1.0
		bodies[i].x = 2.0 * (rand() / (double)RAND_MAX) - 1.0;
		bodies[i].y = 2.0 * (rand() / (double)RAND_MAX) - 1.0;
		bodies[i].z = 2.0 * (rand() / (double)RAND_MAX) - 1.0;

		// Intialize velocities
		bodies[i].vx = vm * (2.0*(rand() / (double)RAND_MAX) - 1.0);
		bodies[i].vy = vm * (2.0*(rand() / (double)RAND_MAX) - 1.0);
		bodies[i].vz = vm * (2.0*(rand() / (double)RAND_MAX) - 1.0);

		// Initialize masses so that total mass of system is constant
		// regardless of how many bodies are simulated
		bodies[i].mass = 1.0 / nBodies;
	}
}

// Opens MPI file, and writes header information (nBodies, iterations)
MPI_File initialize_IO(long nBodies, long nIters, char * fname, int mype)
{
	MPI_File fh;
	FILE * datafile = fopen(fname,"w");
	fprintf(datafile, "%+.*le %+.*le %+.*le\n", 10, (double)nBodies, 10, (double) nIters, 10, 0.0);
	MPI_File_open(MPI_COMM_WORLD, fname, MPI_MODE_CREATE|MPI_MODE_RDWR, MPI_INFO_NULL, &fh);
	return fh;
}

// Writes all particle locations for a single timestep
void distributed_write_timestep(Body * local_bodies, long nBodies,
	long iter, long nIters, int nprocs, int mype, MPI_File * fh) {

	if (!fh) {
		perror("Null MPI file pointer");
		return;
	}

	long offset, length;
	length = jobs_manage(nBodies, mype, nprocs, &offset);
	MPI_Offset byte_offset;
	long totallength = length*54;
	char * ptr;

	// printf("mype is %d with offset of %ld has length %ld \n", mype, offset,length);
	// printf("total length is %ld \",totallength);
	byte_offset = (iter * nBodies + offset+1) * 54 *sizeof(char);
	ptr = (char *) malloc(totallength*sizeof(char));

	for (int i=0; i<length; i++) {
		sprintf(&ptr[54*i], "%+.*le %+.*le %+.*le\n", 10, 
			local_bodies[i].x, 10, local_bodies[i].y, 10, local_bodies[i].z);

	}

	MPI_File_set_view(*fh, byte_offset, MPI_CHAR, MPI_CHAR, "native", MPI_INFO_NULL) ;
	MPI_File_write_all(*fh, ptr, strlen(ptr), MPI_CHAR, MPI_STATUS_IGNORE);

	free(ptr);
}

// Rank job size management
long jobs_manage(long nBodies, int mype, int nprocs, long * iptr) {

	long offset, length, watershed, base;

	watershed = nBodies % nprocs;
	base = nBodies / nprocs;

	if (mype < watershed) {
		length = base + 1;
		offset = length * mype;
	} else {
		length = base;
		offset = (base + 1) * watershed + base * (mype - watershed);
	}

	if (iptr) *iptr = offset;
	return length;
}

#endif
