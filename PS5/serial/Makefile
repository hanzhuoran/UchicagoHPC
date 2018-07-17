OMPFLAG = -fopenmp
CPUCOMPILER = gcc-8
FLAGS = -W
# FLAGS += -I
CC = $(CPUCOMPILER) $(FLAGS) $(OMPFLAG)

##### CPU version #####
CPU_RT: raytracer_cpu.o vec_cpu.o utils_cpu.o
	$(CC) *_cpu.o -o $@ 

raytracer_cpu.o: raytracer_cpu.c utils.h vec.h
	$(CC) -c $< -o $@ 

utils_cpu.o: utils.c utils.h vec.h
	$(CC) -c $< -o $@ 

vec_cpu.o: vec.c vec.h
	$(CC) -c $< -o $@

clean:
	rm *.o