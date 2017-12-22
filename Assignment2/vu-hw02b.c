/*
Michael Vu
Dr. Levine
CSE 3320 - Operating Systems
Due Thursday, October 19, 2017
Assignment 2: 
PART 2:
    Create threads, run in parallel, and pass info between threads. Use synchronization operations to avoid overwriting shared resources.
    The data to be threaded are lines of CSV separated values, we wish to sort these.
    Data set: https://earthquake.usgs.gov/earthquakes/feed/v1.0/csv.php
    Sort by latitude, in ascending order.
    Time program
    Create a program that will, in turn, run multiple threads "concurrently" using a kernel level threading system.
    
    TO COMPILE:
        gcc -Wall -g -std=c99 -Werror -pthread vu-hw02b.c -o vu-hw02b
    TO RUN:
        ./vu-hw02b
*/

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glob.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

struct info {
    char times[30];
    double latitude;
    double longitude;
    double depth;
    double mag;
    char magType[10];
};

int ceiling(int number, int value);
void* extractFile(void* arg);
void increaseStruct(struct info ***eq, int *size);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[])
{
    clock_t begin = clock();
    glob_t paths;     
    int thread, ch, lines = 0, k, csource, earthquakes = 0, i, sorted, rc;
    char cmdbuf[256];
    char **p, **files;
    char *fileName = "all_month.csv";

    printf("Enter how many threads you want to run: ");
    scanf ("%d", &thread);

    while (thread < 0){
        printf("Error: invalid number of threads, can't be negative.\n");
        printf("Please enter how many threads you want to run: ");
        scanf ("%d", &thread);
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
    // Close File
    fclose(fp);

    k = ceiling(lines, thread); // gets lines of each file thread is going to run
    snprintf(cmdbuf, sizeof(cmdbuf),"split -l %d all_month.csv all_montha", k); // splits file into all_monthaaa, all_monthaab, all_monthaac, ..., etc.
    system(cmdbuf);

    /*  Find all "all_montha" files in given directory  */
    csource = glob("all_montha*", 0, NULL, &paths);
    if (csource == 0) { // stores all the files in a 2D char array
        files = malloc(thread * sizeof(char*));
        int n = 0;
        for (p = paths.gl_pathv; *p != NULL; ++p){
            files[n] = malloc(strlen(*p) * sizeof(char) + 1);
            strcpy(files[n], *p);
            n++;
        }
        globfree(&paths);  /* function that frees the memory of the matching strings */
    } 

    // Launch thread
    pthread_t tids[thread];
    for (int i = 0; i < thread; i++) {
        printf("In main: creating thread %d\n", i);
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        rc = pthread_create(&tids[i], &attr, extractFile, files[i]);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
         }
    }
    
    // Wait until thread is done its work
    for (int i = 0; i < thread; i++) {
        pthread_join(tids[i], NULL);
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

    // Close file
    fclose(f);

    // Remove files
    if (remove("earthquakes.txt") == 0)
        printf("earthquakes.txt deleted successfully\n");
    else
        printf("Unable to delete the file\n");

    for (int i = 0; i < thread; i++) {
        if (remove(files[i]) == 0)
            printf("%s deleted successfully\n", files[i]);
        else
            printf("Unable to delete the file\n");
    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("The program took %lf seconds to run\n", time_spent);

    pthread_exit(NULL);
}

void* extractFile(void* arg)
{
    struct info **eq;
    FILE *fp, *f;
    char buffer[500];
    char *del = ",", *nl = "\n", *token;
    int i = 0;

    char* fileName = arg;

    if((fp = fopen(fileName, "r"))==NULL){
        fprintf (stderr, "\nError when opening file: %s\n", fileName);
    }

    if (strcmp(fileName, "all_monthaaa") == 0 || strcmp(fileName, "all_month.csv") == 0)
        fgets(buffer, sizeof(buffer), fp); /* skips the first line of the file, headers */

    int size = 500; // initial structure size
    // Start critical section
    pthread_mutex_lock(&mutex);

    eq = (struct info **) malloc(size * sizeof (struct info *)); // allocate array of 500 structures

    while(fgets(buffer, sizeof(buffer), fp) != NULL)
    {   
        eq[i] = malloc (sizeof (*eq[i]));
        token = strtok(buffer, del);
        strcpy(eq[i]->times, token);

        token = strtok(NULL, del);
        eq[i]->latitude = atof(token);

        token = strtok(NULL, del);
        eq[i]->longitude = atof(token);

        token = strtok(NULL, del);
        eq[i]->depth = atof(token);

        token = strtok(NULL, del);
        eq[i]->mag = atof(token);

        token = strtok(NULL, del);
        strcpy(eq[i]->magType, token);

        token = strtok(NULL, nl);
        i++;

        if(i >= (size + 1)){
            increaseStruct(&eq, &size);
        }
    }
    printf("There are %d listed earthquakes\n", i+1);
    int earthquakes = i;

    // BUBBLE SORT to sort array of structures
    int sorted;
    struct info *temp;
    do {
        sorted = 1;
        for(i = 0; i < earthquakes - 1; i++)
            if((eq[i])->latitude > (eq[i+1])->latitude)
            {
                temp = eq[i];
                eq[i] = eq[i+1];
                eq[i+1] = temp;
                sorted = 0;
            }
    } while( !sorted );
    
    f = fopen("earthquakes.txt", "a");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    for(int i = 0; i < earthquakes - 1; i++)
        fprintf(f, "%f\n", (eq[i])->latitude);

    fclose(f);
    fclose(fp);

    // End critical section
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void increaseStruct(struct info ***eq, int* size){
    *size = (*size) * 2;
    struct info **tmp = realloc(*eq, (*size) * sizeof(*eq));
    if (tmp == NULL) { 
        // handle failed realloc
        printf("unable to reallocate\n");
        exit(1);
    } else {
        // everything went ok, update the original pointer (tmp)
        *eq = tmp;
    }
}

int ceiling(int number, int value){
    double a = (double) number/value;
    int b = number/value;
    double c = a - b;
    if(c != 0)
        b = b+1;
    return b;
}
