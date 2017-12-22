                                                                                                                                                                                                                                                                                                                                                                                                                                                                            /*
Michael Vu
CSE 3320 - Operating Systems
Dr. Levine
Assignment 1
Due Sunday, September 10th, 2017

REFERENCES: 
Dr. Levine's code, link: https://elearn.uta.edu/bbcswebdav/pid-6168730-dt-content-rid-58865979_2/courses/2178-OPERATING-SYSTEMS-81170-001/ProgAssignment1.pdf

READ: 
I managed to complete everything except the sorting part. I knew I had to use stat and <sys/stat.h> and store the info in a struct with time_t time and off_t size
but I simply could not do it in time.
*/

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    pid_t child;
    DIR * d;
    struct dirent * de;
    int i, c, c1, k;
    char s[UCHAR_MAX], cmd[UCHAR_MAX];
    time_t t;
    int startf = 0, finishf = 4; //start/finish counter for files
    int x,n;
    char** array = (char**)malloc(1*sizeof(char*)); //stores files
    char** array1 = (char**)malloc(1*sizeof(char*)); //stores directory

    struct stats{
        char name[UCHAR_MAX];
        time_t time;
        off_t size;
    };

    while (1) {
        //DISPLAYS TIME
        t = time( NULL );
        printf( "Time: %s\n", ctime( &t ));

        //SHOWS CURRENT DIRECTORY
        getcwd(s, UCHAR_MAX);
        printf( "Current Directory: %s\n", s);
        printf("-----------------------------------------------\n" );

        //BEGINS TO STORE THE NAME OF DIRECTORIES IN THE CURRENT DIRECTORY
        d = opendir( "." );
        c1 = 0;
        printf ("Directories:\n");
        while ((de = readdir(d)) ){
            if ((de->d_type) & DT_DIR){
                c1++;
                array1 = (char**)realloc(array1,c1*sizeof(char*));
                array1[c1-1] = (char*)malloc(UCHAR_MAX * sizeof(char));
                strcpy(array1[c1-1],de->d_name);
                printf( " %d.  %s \n", c1-1, array1[c1-1]);
            }
        }
        closedir( d );
        printf( "-----------------------------------------\n" );

        //BEGINS TO STORE THE NAME OF FILES IN THE CURRENT DIRECTORY
        d = opendir( "." );
        c = 0;
        while ((de = readdir(d)) ){
            if (((de->d_type) & DT_REG)){
                c++;
                array = (char**)realloc(array, c*sizeof(char*));
                array[c-1] = (char*)malloc(UCHAR_MAX * sizeof(char));
                strcpy(array[c-1],de->d_name);
            }

        }
        closedir( d );
        //DISPLAYS 5 FILES IN DIRECTORY
        int asd = startf;
        if (5 > c){
            finishf = c-1;
        }
        printf("Files:\n");
        for (asd; asd <= finishf; asd++){
            printf( " %d  %s \n", asd, array[asd]);
        }

        printf( "-----------------------------------------\n" );
        printf("Operations:\n");
        printf("n - Next Files\np - Previous Files\n");
        printf("e - Edit\nr - Run\nc - Change Directory\ns - Sort Directory Listing\nq - Quit\n");
        
        char a;
        scanf("%s",&a);
        switch (a) {
            case 'n'://NEXT FIVE FILES
                    if(finishf == (c-1)){
                        printf("ERROR: CAN NOT GO FORWARD ANY MORE\n");
                        break;
                    }
                    if ((finishf+5) > c-1){
                        startf = startf+5;
                        finishf = finishf + ((c-1) - finishf);
                        break;
                    }
                    else{
                        finishf = finishf+5;
                        startf = startf+5;
                    }

                    break;
            case 'p'://PREVIOUS 5 FILES
                    if(startf <= 0){
                        printf("ERROR: CAN NOT GO BACK ANY MORE\n");
                        break;
                    }
                    if (finishf+5 > c-1){
                        int tfinishf = (finishf - startf) + 1;
                        finishf = finishf-tfinishf;
                        startf = startf-5;
                        break;
                    }
                    else{
                        startf = startf-5;
                        finishf = finishf-5;
                    }
                    break;

            case 'q': exit(0); /* quit */
            case 'e': //EDIT A FILE
                    printf( "Edit what?:" );
                    scanf( "%s", s );
                    int fce = 0;
                    for(x = 0; x <=c-1; x++ ){
                        if((n = strcmp(array[x],s))== 0){
                            fce = 1;
                        }
                    }
                    //IF FILE NOT IN DIRECTORY
                    if(fce == 0){
                        printf("ERROR: FILE NOT FOUND\n\n");
                        break;
                    }
                    strcpy( cmd, "pico ");
                    strcat( cmd, s );
                    system( cmd );
                    break;
            case 'r'://RUN A PROGRAM
                    printf( "Run what?:" );
                    scanf( "%s", cmd );
                    int fcr = 0;
                    for(x = 0; x <=c-1; x++ ){
                        if((n = strcmp(array[x],cmd))== 0){
                            fcr = 1;
                        }
                    }
                    //IF FILE TO RUN NOT IN DIRECTORY
                    if(fcr == 0){
                        printf("ERROR: FILE NOT FOUND\n\n");
                        break;
                    }
                    system(cmd );
                    break;
            case 'c'://CHANGING DIRECTORY
                    printf( "Change To?:" );
                    scanf( "%s", cmd );
                    int fc = 0;
                    for(x = 0; x <=c1-1; x++ ){
                        if((n = strcmp(array1[x],cmd))== 0){
                            fc = 1;
                        }
                    }
                    //IF DIRECTORY NOT IN CURRENT DIRECTORY
                    if(fc == 0){
                        printf("ERROR: DIRECTORY NOT FOUND\n\n");
                        break;
                    }
                    chdir( cmd );
                    break;
            case 's'://SORTING DIRECTORY LISTING
                    printf( "Sort By Size or Date?:" );
                    scanf( "%s", cmd );
                    if(((strcmp("Size",cmd))== 0) || (strcmp("size",cmd))== 0){
                        system("ls -S");
                    }
                    if(((strcmp("Date",cmd))== 0) || (strcmp("date",cmd))== 0){
                        system("ls -t");
                    }
                    break;
            default://ERROR INPUT
                printf("ERROR: NOT AN INPUT\n");
                break;
        }
    }
}
