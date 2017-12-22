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
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct info {
    char times[30];
    double latitude;
    double longitude;
    double depth;
    double mag;
    char magType[10];
};

void bubbletofile(FILE* f, struct info eq[], int size);

int main(int argc, char *argv[])
{
    struct info eq[10000];
    FILE* fp, f;
    char buffer[500];
    char fileName[50];
    char *del = ",", *nl = "\n", *token;
    int i = 0;

    strcpy(fileName, argv[1]);
    printf("FILENAME: %s\n", fileName);

    if((fp = fopen(fileName, "r"))==NULL){
        fprintf (stderr, "\nError when opening file: %s\n", fileName);
    }

    if (strcmp(fileName, "all_monthaaa") == 0 || strcmp(fileName, "all_month.csv") == 0)
        fgets(buffer, sizeof(buffer), fp); /* skips the first line of the file, headers */


    while(fgets(buffer, sizeof(buffer), fp) != NULL)
    {   
        token = strtok(buffer, del);
        strcpy(eq[i].times, token);

        token = strtok(NULL, del);
        eq[i].latitude = atof(token);

        token = strtok(NULL, del);
        eq[i].longitude = atof(token);

        token = strtok(NULL, del);
        eq[i].depth = atof(token);

        token = strtok(NULL, del);
        eq[i].mag = atof(token);

        token = strtok(NULL, del);
        strcpy(eq[i].magType, token);

        token = strtok(NULL, nl);
        i++;
    }
    printf("There are %d listed earthquakes\n", i+1);
    bubbletofile(&f, eq, i);
    fclose(fp);
}

void bubbletofile(FILE* f, struct info eq[], int size)
{
    int i, sorted;
    struct info temp;

    do {
        sorted = 1;

        for(i = 0; i < size - 1; i++)
            if(eq[i].latitude > eq[i+1].latitude)
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
    for(int i = 0; i < size; i++)
        fprintf(f, "%f\n", eq[i].latitude);

    fclose(f);
}

