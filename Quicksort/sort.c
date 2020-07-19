/*******************************************************************************
 * Name        : sort.c
 * Author      : Joshua Mimer
 * Date        : 02/11/2020
 * Description : Uses quicksort to sort a file of either ints, doubles, or
 *               strings.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <errno.h> 
#include <getopt.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "quicksort.h"

#define MAX_STRLEN     64 // Not including '\0'
#define MAX_ELEMENTS 1024

typedef enum {
    STRING,
    INT,
    DOUBLE
} elem_t;

void display_message(){
printf("Usage: ./sort [-i|-d] [filename]\n"
	"   -i: Specifies the file contains ints.\n"
	"   -d: Specifies the file contains doubles.\n"
	"   filename: The file to sort.\n"
	"   No flags defaults to sorting strings.\n");
}
int i_flag = 0;
int d_flag = 0;
char buf[MAX_ELEMENTS];
char charcter[MAX_ELEMENTS];
int line_number = 0;

void display_array_int(int *array, const int length){
	if(length > 0){
		printf("%d", *array); // first element
	}
	for (int i = 1; i < length; i++){
		printf("\n%d", *(array + i));
	}
	printf("\n");
}
void display_array_double(double *array, const int length){
	if(length > 0){
		printf("%f", *array); // first element
	}
	for (int i = 1; i < length; i++){
		printf("\n%f", *(array + i));
	}
	printf("\n");
}
void display_array_strings(char **array, const int length){
	if(length > 0){
		printf("%s", *array); // first element
	}
	for (int i = 1; i < length; i++){
		printf("\n%s", *(array + i));
	}
	printf("\n");
}
/**
 * Basic structure of sort.c:
 *
 * Parses args with getopt.
 * Opens input file for reading.
 * Allocates space in a char** for at least MAX_ELEMENTS strings to be stored,
 * where MAX_ELEMENTS is 1024.
 * Reads in the file
 * - For each line, allocates space in each index of the char** to store the
 *   line.
 * Closes the file, after reading in all the lines.
 * Calls quicksort based on type (int, double, string) supplied on the command
 * line.
 * Frees all data.
 * Ensures there are no memory leaks with valgrind. 
 */
int main(int argc, char **argv) {
	int opt;
	while ((opt = getopt(argc, argv, ":id")) != -1)
	{
		switch (opt)
		{
			case 'i':
				i_flag = 1;
				break;
			case 'd':
				d_flag = 1;
				break;
			case '?':
				printf("Error: Unknown option: '%s' received.\n", argv[1]);
				display_message();
				return EXIT_FAILURE;

     }
  }
	if(argc == 1){
		display_message();
	}
	if(argc == 2){
		FILE *fp = fopen(argv[1], "r");
		if (fp == NULL){
			fprintf(stderr, "Error: Cannot open '%s'. No such file or directory.\n", argv[1]);
			return EXIT_FAILURE;
	}
	else{
		char **temp = (char **)malloc(MAX_ELEMENTS * sizeof(char*));
    	int ind = 0;
		while(fgets(buf, MAX_STRLEN+2, fp)){
			char *eoln = strchr(buf, '\n'); // stripping the /n
			if (eoln == NULL) {
            // This should not happen.
            buf[MAX_STRLEN] = '\0';
        	} else {
            *eoln = '\0';
        	} 
        	temp[ind] = (char *)malloc(MAX_STRLEN * sizeof(char));
        	strcpy(*(temp+ind), buf);
        	ind++;
        	line_number++;
		}
		fclose(fp);
		quicksort(temp, line_number, sizeof(char *), str_cmp);
		display_array_strings(temp, line_number);
		for(int i = 0; i <= line_number; i++){
			free(temp[i]);
		}
		free(temp);
	}
}
  if(i_flag == 1){
  	FILE *fp = fopen(argv[2], "r");
	if (fp == NULL){
		fprintf(stderr, "Error: Cannot open '%s'. No such file or directory. \n", argv[2]);
		return EXIT_FAILURE;
	}
	else{
		int ind = 0;
		int *integer = malloc(sizeof(int)*MAX_ELEMENTS);
		while(fgets(buf, MAX_STRLEN+2, fp)){
			char *eoln = strchr(buf, '\n'); // stripping the /n
			if (eoln == NULL) {
            // This should not happen.
            buf[MAX_STRLEN] = '\0';
        	} else {
            *eoln = '\0';
        	}
			int n = atoi(buf);
			*(integer + ind) = n;
			ind++;
			line_number++;
		}
		fclose(fp);
		quicksort(integer, line_number, sizeof(int), int_cmp);
		display_array_int(integer, line_number);
		free(integer);
	}
  }
  else if(d_flag == 1){
  	FILE *fp = fopen(argv[2], "r");
	if (fp == NULL){
		fprintf(stderr, "Error: Cannot open '%s'. No such file or directory. \n", argv[2]);
		return EXIT_FAILURE;
	}
	else{
		int ind = 0;
		double *dub = malloc(sizeof(double)*MAX_ELEMENTS);
		while(fgets(buf, MAX_STRLEN+2, fp)){
			char *eoln = strchr(buf, '\n'); // stripping the /n
			if (eoln == NULL) {
            // This should not happen.
            buf[MAX_STRLEN] = '\0';
        	} else {
            *eoln = '\0';
        	}
			double n = atof(buf);
			*(dub + ind) = n;
			ind++;
			line_number++;
		}
		fclose(fp);
		quicksort(dub, line_number, sizeof(double), dbl_cmp);
		display_array_double(dub, line_number);
		free(dub);
	}
  }
  return EXIT_SUCCESS;
  
}
