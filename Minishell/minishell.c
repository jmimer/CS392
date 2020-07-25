/*Joshua Mimer and Mark Pipko
I pledge my honor that I have abided by the Stevens Honor System.*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <pwd.h>
#include <sys/types.h>
#include <limits.h>
#include <stdbool.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>

#define BUFSIZE 8192

sigjmp_buf jmpbuf;
int currentSpeed;
int speedLimit;
double time_spent;

volatile sig_atomic_t signal_val = 0;

void catch_signal(int sig) {
    if(!signal_val){
      write(STDOUT_FILENO, "\n", 1);
      siglongjmp(jmpbuf, 1);
    }
}

int sensor(){
    return rand()%81;
}

int main(int argc, char *argv[]) {
  char text[4096];
  clock_t begin = clock();
  bool EngineStatus = true;
  FILE *log;
  log = fopen("CruiseControl.log","a");

  if(argc != 2){
    fprintf(stderr, "Usage: %s <speed>\n", argv[0]);
    return EXIT_FAILURE;
  }
  struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = catch_signal;
    action.sa_flags = SA_RESTART; /* Restart syscalls if possible */

    if (sigaction(SIGINT, &action, NULL) < 0) {
        fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));
        return EXIT_FAILURE;
    }
    currentSpeed = atoi(argv[1]);
    speedLimit = 30 + rand()%81;

    time_t t;
    time(&t);

  printf("Cruise Control System has been activated.\n"
  "   -b: Specifies the Cruise Control that brake has been activated.\n"
	"   -c: Specifies the Cruise Control to change Speed.\n"
	"   -e: Specifies the Cruise Control system to deactivate.\n");
    sprintf(text,"Cruise Control System has been activated.\n");
    fputs(text,log);
    do {
      sigsetjmp(jmpbuf, 1);
      char input[BUFSIZE];
      printf("\n");
      printf("The current date and time is: %s\n", ctime(&t));
      printf("The current speed is: %d\n", currentSpeed);
      printf("Please select an option: ");
      fflush(stdout);
      // memset(input, 0, BUFSIZE);
      ssize_t bytes_read = read(STDIN_FILENO, input, sizeof(input)-1);
      if (bytes_read < 0){
        fprintf(stderr, "Error: read() failed. %s.\n", strerror(errno));
        return EXIT_FAILURE;
      }
      input[bytes_read-1] = '\0';
      // printf("Printing input after read: %s", input);
      if (bytes_read == 1) {
        continue;
      }
      char **arguments;
      if(( arguments = (char **)malloc(2048*sizeof(char*))) == NULL){
        fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
        return EXIT_FAILURE;
      }
      char *temp = strtok(input, " ");
      int word_counter = 0;
      int i = 0;
      while (temp != NULL){
        if (( arguments[i] = (char*)malloc((strlen(temp)+1)*sizeof(char))) == NULL){
          fprintf(stderr, "Error: malloc() failed. %s.\n", strerror(errno));
          return EXIT_FAILURE;
        }
        strcpy(arguments[i], temp);
        temp = (char*)strtok(NULL, " ");
        word_counter++;
        i++;
      }
      if (strcmp(arguments[0], "e") == 0) {
        for (int j = 0; j < word_counter; j++){
          free(arguments[j]);
        }
        free(arguments);
        EngineStatus = false;
        printf("The cruise control have been deactivated.\n");
        sprintf(text,"The car has stopped and Cruise Control is deactivated.\n");
        fputs(text,log);
        break;
    }
    else if(strcmp(arguments[0], "c") == 0){
      if(word_counter > 2){
        fprintf(stderr, "Error: Too many arguments.\n");
      }
      else{
        bool good = true;
          if(atoi(arguments[1]) > speedLimit){
            printf("The speed you have entered is greater than the allowed SpeedLimit.\n");
            currentSpeed = speedLimit;
            sprintf(text,"[The Current Speed has been changed to %d at %s].\n",currentSpeed, ctime(&t));
            fputs(text,log);
            good = false;
          }
          int temp = sensor();
          if(atoi(arguments[1]) > temp){
            currentSpeed = temp;
            printf("The car must slow down due to road conditions.\n");
            sprintf(text,"[The Current Speed has been changed to %d at %s].\n",currentSpeed, ctime(&t));
            fputs(text,log);
            good = false;
          }
          if(good){
            currentSpeed = atoi(arguments[1]);
            sprintf(text,"[The Current Speed has been changed to %d at %s].\n",currentSpeed, ctime(&t));
            fputs(text,log);
          }
      }
  }
    else if(strcmp(arguments[0], "b") == 0){
      currentSpeed = 0;
      for (int j = 0; j < word_counter; j++){
        free(arguments[j]);
      }
      free(arguments);
      printf("The car has stopped and Cruise Control is deactivated.\n");
      sprintf(text,"The car has stopped and Cruise Control is deactivated.\n");
      fputs(text,log);
      EngineStatus = false;
      break;
    }
    for (int j = 0; j < word_counter; j++){
      free(arguments[j]);
    }
    free(arguments);
}while (EngineStatus);
  clock_t end = clock();
  time_spent = ((double)(end - begin) / CLOCKS_PER_SEC) * 10000;
  sprintf(text," The engine has been on for: %f seconds.\n", time_spent);
  fputs(text,log);
  fclose(log);
  return EXIT_SUCCESS;
}
