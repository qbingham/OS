/**
Class: CPSC 346-02
Team Member 1: Quin Bingham
GU Username of project lead: qbingham
Pgm Name: proj5.c 
Pgm Desc: implementation for the Peterson algorithm written in C.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

void child(char*, int, int);
void parent(char*, int, int, int);
void cs(char, int);
void non_crit_sect(int);



int main(int argc, char** argv)
{
    char turn = '0';
    char flag0 = '0';
    char flag1 = '0';
    int time_parent, time_child, time_parent_non_cs, time_child_non_cs;
    
    // get number of arguments
    int arg_count = 0; 
    while(argv[++arg_count]);
    // check for proper arguments
    if(arg_count == 5 || arg_count == 1)
    {
        if (arg_count == 5)
        {
            time_parent         = argv[1][0] - '0';
            time_child          = argv[2][0] - '0';
            time_parent_non_cs  = argv[3][0] - '0';
            time_child_non_cs   = argv[4][0] - '0';

            if (time_parent < 1 || time_parent > 9)
            {
                printf("Error: Invalid arguments1\n");
                return -1;
            }
        }
        else
        {
            time_parent         = 2;
            time_child          = 3;
            time_parent_non_cs  = 4;
            time_child_non_cs   = 5;
        }
    }
    else
    {
        printf("Error: Enter 1 or 5 arguments\n");
        printf("You entered %d.\n", arg_count);
        int i;
        printf("argv[0]: ./a.out\n");
        for (i = 1; i < arg_count; i++)
            printf("argv[%d]: %d\n", i, argv[i][0]-'0');
        return -1;
    }
    
    char*  address;
    int    shmid;
    //create a shared memory segment
    shmid = shmget(0, 3, 0777 | IPC_CREAT);

    //attach it to the process, cast its address, and 
    address = (char*)shmat(shmid,0,0); 

    //address[0] = char turn
    //address[1] = char flag0
    //address[2] = char turn1
    *address = turn;
    *(address+1) = flag0;
    *(address+2) = flag1;
    
    // fork here
    if (fork() == 0)
    {
        child(address, time_child, time_child_non_cs);
    }
    else 
    {
        parent(address, shmid, time_parent, time_parent_non_cs);
    }
    return 0;
}

//pass turn, flag0, and flag1 by reference so the variables can be shared between parent and child functions
void child(char* address, int time_crit_sect, int time_non_crit_sect)
{
    

    for (int i = 0; i < 5; i++)
    {
        *(address+1) = '1';
        *address = '1';

        while(*(address+2) == '1' && *(address) == '1'){}

        // critical section
        cs('c', time_crit_sect);

        // return child flag to 0
        *(address+1) = '0';

        // non-critical section
        non_crit_sect(time_non_crit_sect); 
    }

    //detach the shared memory from the process*/
    printf("detach child\n");
    shmdt(address);
}

//pass turn, flag0, and flag1 by reference so the variables can be shared between parent and child functions
void parent(char* address, int shmid, int time_crit_sect, int time_non_crit_sect)
{
    

    for (int i = 0; i < 5; i++)
    {
        *(address+2) = '1';
        *address = '0';

        while(*(address+1) == '1' && *address == '0'){}

        // critical section
        cs('p', time_crit_sect);

        // return parent flag to 0
        *(address+2) = '0';

        // non-critical section
        non_crit_sect(time_non_crit_sect); 
    }

    // wait for child to be done before disassociating memory
    wait(NULL);

    //detach shared memory from the process
    printf("detach parent\n");
    shmdt(address);
   
    //remove it 
    printf("remove parent\n");
    shmctl(shmid,IPC_RMID,0);
}

void cs(char process, int time_crit_sect)
{
    if (process == 'p')
    {
        printf("parent in critical section\n");
        sleep(time_crit_sect);
        printf("parent leaving critical section\n");
    }
    else
    {
        printf("child in critical section\n");
        sleep(time_crit_sect);
        printf("child leaving critical section\n");
    }
}

void non_crit_sect(int time_non_crit_sect)
{
    sleep(time_non_crit_sect);
}