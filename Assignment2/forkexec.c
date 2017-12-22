/*
Michael Vu
Dr. Levine
CSE 3320 - Operating Systems
Due Sunday, October 8, 2017
Assignment 2: 
PART 1:
    Create processes, run processes in parallel, and pass info between processes. The data to be processed are lines of CSV separated values, we wish to sort these.
    Data set: https://earthquake.usgs.gov/earthquakes/feed/v1.0/csv.php
    Sort by latitude, in ascending order.
    Time program
    Create a program that will, in turn, run multiple processes "concurrently" using fork() and exec().

    TO COMPILE:
        gcc -Wall -g -std=c99 -Werror forkexec.c -o forkexec
        gcc -Wall -g -std=c99 -Werror vu-hw02.c -o vu-hw02
    TO RUN:
        ./forkexec
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glob.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

int ceiling(int number, int value);

int main(int argc, char *argv[])
{
    clock_t begin = clock();
    glob_t paths;     
    int process, ch, lines = 0, k, csource, earthquakes = 0, i, sorted;
    char cmdbuf[256];
    char **p, **files;
    char *fileName = "all_month.csv";

    printf("Enter how many processes you want to run: ");
    scanf ("%d", &process);

    while (process < 0){
        printf("Error: invalid number of processes, can't be negative.\n");
        printf("Please enter how many processes you want to run: ");
        scanf ("%d", &process);
    }

    // Opens File
    FILE* fp;
    if((fp = fopen(fileName, "r"))==NULL)
        fprintf (stderr, "\nError when opening file\n");

    while(!feof(fp)) {
      ch = fgetc(fp);
      if(ch == '\n')
        lines++; // get total lines in file
    }

    k = ceiling(lines, process); // gets lines of each file process is going to run
    snprintf(cmdbuf, sizeof(cmdbuf),"split -l %d all_month.csv all_montha", k); // splits file into all_monthaaa, all_monthaab, all_monthaac, ..., etc.
    system(cmdbuf);

    /*  Find all "all_montha" files in given directory  */
    csource = glob("all_montha*", 0, NULL, &paths);
    if (csource == 0) { // stores all the files in a 2D char array
        files = malloc(process * sizeof(char*));
        int n = 0;
        for (p = paths.gl_pathv; *p != NULL; ++p){
            files[n] = malloc(strlen(*p) * sizeof(char) + 1);
            strcpy(files[n], *p);
            n++;
        }
        globfree(&paths);  /* function that frees the memory of the matching strings */
    }   

    for (int i = 0; i < process; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed.");
            exit(1);
        }
        else if (pid == 0){
            // CHILD  process
            char* args[] = {"./vu-hw02", files[i], NULL};
            execv(args[0], args);
            exit(0);
        }
    }
    // PARENT process
    for (int i = 0; i < process; i++) {
        wait(NULL);
    }

    // Reads earthquakes.txt
    FILE* f;
    double array[10000], temp;

    if((f = fopen("earthquakes.txt", "r"))==NULL)
        fprintf (stderr, "\nError when opening file\n");

    // Stores all latitudes in earthquakes.txt into 2D array
    i = 0;
    while(!feof(f))
    {
        fscanf(f, "%lf", &array[i]);
        i++;
    }
    earthquakes = i;

    // Sorts the final array
    do {
        sorted = 1;
        for(i = 0; i < earthquakes - 1; i++)
            if(array[i] > array[i+1])
            {
                temp = array[i];
                array[i] = array[i+1];
                array[i+1] = temp;
                sorted = 0;
            }
    } while( !sorted );

    printf("SORTED EARTHQUAKES BY LATITUDE\n");
    // Print out sorted array
    for(i = 0; i < earthquakes - 1; i++)
        printf("[%d]: %f\n", i+1, array[i]);

    // Close files
    fclose(f);
    fclose(fp);

    // Remove files
    if (remove("earthquakes.txt") == 0)
        printf("earthquakes.txt deleted successfully\n");
    else
        printf("Unable to delete the file\n");

    for (int i = 0; i < process; i++) {
        if (remove(files[i]) == 0)
            printf("%s deleted successfully\n", files[i]);
        else
            printf("Unable to delete the file\n");
    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("The program took %lf seconds to run\n", time_spent);

    return 0;
}

int ceiling(int number, int value){
    double a = (double) number/value;
    int b = number/value;
    double c = a - b;
    if(c != 0)
        b = b+1;
    return b;
}
