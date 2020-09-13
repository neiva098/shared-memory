#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

void callProcess(int numProcessos, char *argv[])
{
    pid_t pid = fork();

    if (pid == 0)
    {
        char procNumber[255];
        sprintf(procNumber, "%d", numProcessos);
        argv[0] = procNumber;

        execve("./calculo", argv, NULL);

        exit(0);
    }
}

double *createSharedMemory(char sharedMemName[])
{
    const int SIZE = 4096;
    const char *name = sharedMemName;

    int shm_fd;

    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    ftruncate(shm_fd, SIZE);

    return mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
}

void closeSharedMemory(char name[])
{
    if (shm_unlink(name) == -1)
    {
        printf("Error removing %s\n", name);
        exit(-1);
    }
}

void callSums(char *argv[], int procNumbers, double *ptr)
{
    for (int i = 0; i < procNumbers; i++)
    {
        *ptr = -1.0; // impossible value
        ptr++;
        callProcess(i, argv);
    }

    ptr -= procNumbers;
}

double receiveSums(int procNumbers, double *ptr)
{
    int sendedProcess = 0;
    double sum = 0.0;

    do
    {
        if (*ptr != -1.0)
        {
            sum += *ptr;
            ptr++;
            sendedProcess++;
        }
    } while (sendedProcess < procNumbers);

    return sum;
}

double calcPi(double sum)
{
    double h = 1.0 / 1000000000;

    return sum * h;
}

int main(int argc, char *argv[], char *envp[])
{
    int procNumbers = atoi(argv[1]);

    char memoryName[] = "shared_memory";

    double *ptr = createSharedMemory(memoryName);

    callSums(argv, procNumbers, ptr);

    double sum = receiveSums(procNumbers, ptr);

    printf("\nO valor de pi e: %.16f\n", calcPi(sum));

    closeSharedMemory(memoryName);
}