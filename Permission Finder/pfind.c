/*******************************************************************************
 * Name        : pfind.c
 * Author      : Joshua Mimer
 * Date        : 03/04/2020
 * Description : Permission Finder implementation.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int h_flag = 0;
int d_flag = 0;
int p_flag = 0;

int perms[] = {S_IRUSR, S_IWUSR, S_IXUSR,
               S_IRGRP, S_IWGRP, S_IXGRP,
               S_IROTH, S_IWOTH, S_IXOTH};
char* get_permissions(struct stat *statbuf) {
    int ind = 0;
    char *useme =(malloc(sizeof(char)*10)); 
    int permission_valid;
    for (int i = 0; i < 9; i += 3) {    
        permission_valid = statbuf->st_mode & perms[i];
        if (permission_valid) {
            useme[ind] = 'r';
            ind++;    
        } 
        else 
        {
            useme[ind] = '-';
            ind++;    
        }    
        permission_valid = statbuf->st_mode & perms[i+1];
        if (permission_valid) {
            useme[ind] = 'w'; 
            ind++;   
        } 
        else {
            useme[ind] = '-'; 
            ind++;   
        }    
        permission_valid = statbuf->st_mode & perms[i+2];
        if (permission_valid) {
            useme[ind] = 'x';
            ind++;    
        } 
        else {
            useme[ind] = '-';
            ind++;   
        }
    }
    return useme;
}               

void list_dir(char* dir_name, char* permission){
  char path[PATH_MAX];
  if (realpath(dir_name, path) == NULL) {
    fprintf(stderr, "Error: Cannot stat '%s'. %s.\n", dir_name, strerror(errno));
    return;
  }
  DIR *dir;
  if ((dir = opendir(dir_name)) == NULL) {
      fprintf(stderr, "Error: Cannot open directory '%s'. %s.\n", dir_name, strerror(errno));
      return;
    }
	struct dirent *entry;
  struct stat sb;
	char full_filename[PATH_MAX];
	size_t pathlen = 0;

	// full_filename[0] = '\0';

	if (strcmp(path, "/")){
		//If path is not the root - '/', then ...
		strncpy(full_filename, path, PATH_MAX-1);
	}
	// Add + 1 for the trailing '/' that we're going to add
	pathlen = strlen(full_filename) + 1;
	full_filename[pathlen - 1] = '/';
	full_filename[pathlen] = '\0';
	while((entry = readdir(dir)) != NULL){
		if(strcmp(entry->d_name, ".") == 0 || 
			strcmp(entry->d_name, "..") == 0){
				continue;
			}
			strncpy(full_filename + pathlen, entry->d_name, PATH_MAX - pathlen);
			lstat(full_filename, &sb);
			char *perms = get_permissions(&sb);
			if(entry ->d_type == DT_DIR){
				if(strcmp(permission, perms) == 0){
					printf("%s\n",full_filename);
					}		
				list_dir(full_filename, permission);
			}
			else{
				if(strcmp(permission, perms) == 0){
				printf("%s\n",full_filename);
				}	
			}
			free(perms);
		}
	closedir(dir);
}


void display_message(){
printf("Usage: ./pfind -d <directory> -p <permissions string> [-h]\n");
}

bool permission_string(char* check){
	size_t total_size = strlen(check);
	if(total_size != 9){
		return false;
	}
	for(size_t i = 0; i <= 6; i += 3) {
		if((check[i] != 'r') && (check[i] != '-')){
			return false;
		}
		if((check[i+1] != 'w') && (check[i+1] != '-')){
			return false;
		}
		if((check[i+2] != 'x') && (check[i+2] != '-')){
			return false;
		}
	}
	return true;
}


int main(int argc, char **argv) {
	if(argc == 1){
		display_message();
		return EXIT_FAILURE;
	}
	int opt;
  extern char *optarg;
  char* dname;
  char* pname;
	while ((opt = getopt(argc, argv, ":d:p:h")) != -1)
	{
		switch (opt)
		{
			case 'd':
				d_flag = 1;
        dname = optarg;
				break;
			case 'h':
				h_flag = 1;
				break;
			case 'p':
				p_flag = 1;
        pname = optarg;
				break;	
			case '?':
				fprintf(stderr, "Error: Unknown option '-%c' received.\n", optopt);
				return EXIT_FAILURE;

     }
  }
  if(h_flag){
  	display_message();
  	return EXIT_SUCCESS;
  }
  if(!d_flag){
  	printf("Error: Required argument -d <directory> not found.\n");
  	return EXIT_FAILURE;
  }
  if(!p_flag){
  	printf("Error: Required argument -p <permissions string> not found.\n");
  	return EXIT_FAILURE;
  }
  if(d_flag){
  	struct stat statbuf;
  	char buf[PATH_MAX];
  	if(stat(dname, &statbuf) < 0){
  		fprintf(stderr, "Error: Cannot stat '%s'. %s.\n", dname, strerror(errno));
  		return EXIT_FAILURE;
  	}
  	if(!S_ISDIR(statbuf.st_mode)){
  		fprintf(stderr, "Error: '%s' is not a directory.\n",dname);
  		return EXIT_FAILURE;
  	}
  	realpath(dname, buf);
  	DIR *dp;
  	if ((dp = opendir(dname)) == NULL) {
  		fprintf(stderr, "Error: Cannot open directory '%s'. %s.\n", buf, strerror(errno));
  		closedir(dp);
  		return EXIT_FAILURE;
  	}
  	closedir(dp);

  }
  if(p_flag){
  	if(!permission_string(pname)){
  		fprintf(stderr, "Error: Permissions string '%s' is invalid.\n", pname);
  		return EXIT_FAILURE;
  	}
  }
  	list_dir(dname, pname);
  	return EXIT_SUCCESS;
	}