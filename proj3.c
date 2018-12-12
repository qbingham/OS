/*
Class: CPSC 346-02
Team Member 1: Quin Bingham
GU Username of project lead: qbingham
Pgm Name: proj3.c 
Pgm Desc: exploraiton of the proc file system 
Usage: 1) standard:  ./a.out -s 
Usage: 2) history:  ./a.out -h 
Usage: 3) load:  ./a.out -l 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void standard();
void history();
void load();
void printDDMMHHSS();

int main(int argc, char* argv[])
{
 if (argc != 2)
  {
   fprintf(stderr, "Error: Options required\n"); 
   fprintf(stderr, "usage: ./a.out -s|-h|-l\n\n"); 
   exit(EXIT_FAILURE);
  }
  
 if (!strcmp(argv[1],"-s"))
  standard();
 if (!strcmp(argv[1],"-h"))
  history();
 if (!strcmp(argv[1],"-l"))
  load();
}
/*
pre: none
post: displays CPU vendor_id, model name, and OS version
*/
void standard()
{
 char ch;
 FILE* ifp;
 char str[80];

 /*
 I've deliberately used two different mechanisms for writing to the console.  
 Use whichever suits you.
 strstr locates a substring
 */

 ifp = fopen("/proc/cpuinfo","r");
 while (fgets(str,80,ifp) != NULL)
  if (strstr(str,"vendor_id") || strstr(str,"model name"))
   puts(str); 
 fclose (ifp);

 ifp = fopen("/proc/version","r");
 while ((ch = getc(ifp)) != EOF)
  putchar(ch);
 fclose (ifp);
}

/*
pre: none
post: displays time since the last reboot (DD:HH:MM:SS), time when the system 
      was last booted, number of processes(proc/stat) that have been created since the
      last reboot.  
      Hint: strftime could be useful
*/
void history()
{
 char ch;
 FILE* ifp;
 char str[80];
 ifp = fopen("/proc/uptime", "r");

 fscanf(ifp, "%s", str);

 int seconds = atoi(str);
 printf("Time since last boot: ");
 printDDMMHHSS(seconds);
 time_t result = time(NULL);
 result -= seconds;
 printf("Date of last boot: ");
 printf("%s",ctime(&result));
 fclose (ifp);
 
 ifp = fopen("/proc/stat", "r");
 int num_processes;
 char process[10];
 int count=1;
 if ( ifp != NULL )
 {
   char line[80]; 
   while (fgets(line, 80, ifp) != NULL) {
            fscanf(ifp, "processes %d", &num_processes);
  }
 fclose(ifp);
 printf("Number of processes: ");
 printf("%d \n", num_processes);
}



}

/*
pre: none
post: displays total memory, available memory, load average (proc/meminfo, proc/loadavg) (avg. number of processes over the last minute) 
*/
void load()
{
 FILE* ifp;
 int memActive;
 int memAvailable;
 char line[80];

ifp = fopen("/proc/meminfo", "r");
 while (fgets(line,80, ifp) != NULL) {
     fscanf(ifp, "MemAvailable:    %d kB", &memAvailable);
 }

 ifp = fopen("/proc/meminfo", "r");
 while (fgets(line,80, ifp) != NULL) {
      fscanf(ifp, "Active:           %d",  &memActive);
 }

 printf("Total Memory: %d\n", (memAvailable+memActive));
 printf("Available Memory: %d\n", memAvailable);


 int loadAvg;
 char a[20];
 char b[20];
 char c[20];
 char d[20];

 ifp = fopen("/proc/loadavg", "r");
 fscanf(ifp, "%s %s %s %s %d",  a, b, c, d, &loadAvg);
 
 printf("Load Average: %d\n", loadAvg);

}

void printDDMMHHSS(int seconds)
{
 int days = (seconds/86400);
 int hours =(seconds/3600);
 seconds=seconds-(hours*3600);
 int minutes =(seconds/60);
 seconds=seconds-(minutes*60);

 if (days < 10){
 	printf("0%d:",days);
 } else { 
 printf("%d:",days);
 }
 
 if (hours < 10){
 	printf("0%d:",hours);
 } else { 
 printf("%d:",hours);
 }

 if (minutes < 10 ){
 	printf("0%d:",minutes);
 } else { 
 printf("%d:",minutes);
 }

 if (seconds < 10 ){
 	printf("0%d\n",seconds);
 } else { 
 printf("%d\n",seconds);
 }
}

