#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

void *openSharedMemory()
{
	const char *name = "shared_memory";
	const int SIZE = 4096;

	int shm_fd;

	shm_fd = shm_open(name, O_RDWR, 0666);
	if (shm_fd == -1)
	{
		printf("shared memory failed\n");
		exit(-1);
	}

	void *ptr;

	return mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
}

void *writePi(double pi, double *ptr, int procNumber)
{
	ptr += procNumber;

	*ptr = pi;

	return ptr;
}

void saveSum(double pi, int procNumber)
{
	double *ptr = openSharedMemory();

	writePi(pi, ptr, procNumber);
}

double generateRandomNumber()
{
	return (double)rand() / (double)RAND_MAX;
}

float calcSum(int procNumber, int totalProc)
{
	int passos = 1000000000 / totalProc;
	int initial = passos * procNumber;
	double sum = 0.0;
	double x;
	double h = 1.0 / 1000000000;

	for (int i = 1; i <= passos; i += 1)
	{
		x = h * ((double)(initial + i) - 0.5);

		sum += 4.0 / (1.0 + x * x);
	}

	return sum;
}

int main(int argc, char *argv[], char *envp[])
{
	int processNumber = atoi(argv[0]);
	int totalProcess = atoi(argv[1]);

	double pi = calcSum(processNumber, totalProcess);

	saveSum(pi, processNumber);
}
