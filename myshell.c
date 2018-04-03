/****************************************************************
 * Name        : Junjie Lu                                      *
 * Class       : CSC 415                                        *
 * Date  	     : Feb 26, 2018                                   *
 * Description :  Writting a simple bash shell program          *
 * 	        	    that will execute simple commands. The main   *
 *                goal of the assignment is working with        *
 *                fork, pipes and exec system calls.            *
 ****************************************************************/

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFERSIZE 256
#define ARGVMAX 64
#define PIPECNTMAX 10

char input_buffer[BUFFERSIZE];
int input_buffer_len;
char prompt[256];
char cwd[256];
char *myargv[ARGVMAX];
int myargc;
pid_t pid;
#define true 1
#define false 0


void clear_commands ();
void prompt_display ();
void seperate_commands ();
void execute_commands (char *myargv[], int myargc);
void run_command(char *myargv[]);
void change_directory();
void execute_command_background (char *myargv[], int myargc);
void redirect_output_overwrite(char *myargv[], int myargc);
void redirect_output_append(char *myargv[], int myargc);
void redirect_input(char *myargv[], int myargc);
void execute_multiple_commands (char *myargv[], int myargc);
int check_sign_function(char* myargv[], int myargc, char *sign);


void clear_commands () {
  // clear myargv[] empty
	int i = 0;
	while(myargv[i] != NULL) {
		myargv[i] = NULL;
		i++;
	}	
}


void prompt_display () {
	// show the prompt with current working diretory 
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		strcpy(prompt, "MyShell ");
		strcat(prompt, cwd);
		strcat(prompt, " >> ");
		printf("%s", prompt);
	}
}


void seperate_commands () {
	// reset myarc everytime
	myargc = 0;

	// seperate input_buffer into myargv
	char *token = strtok(input_buffer, " ");
  while(token != NULL) {
    myargv[myargc++] = token;
    token = strtok(NULL, " ");
  }
}


void execute_commands (char *myargv[], int myargc) {
  // If user input "exit", then quit the program
  // Else chck for if myargv contend any signs
  if (myargv[0] != NULL) {
    if (strcmp(myargv[0], "exit") == 0) {
      exit(0);
    } else if (strcmp(myargv[0], "cd") == 0) {
      change_directory();
    } else if (check_sign_function(myargv,myargc, "|") == true){
      execute_multiple_commands (myargv, myargc);
    } else if (check_sign_function(myargv, myargc, "&") == true) {
      execute_command_background (myargv, myargc);
    } else if (check_sign_function(myargv, myargc, ">") == true) {
      redirect_output_overwrite(myargv, myargc);
    } else if (check_sign_function(myargv, myargc, ">>") == true) {
      redirect_output_append(myargv, myargc);
    } else if (check_sign_function(myargv, myargc, "<") == true){
      redirect_input(myargv, myargc);
    } else {
      run_command(myargv);
    }
	}
}


int check_sign_function(char* myargv[], int myargc, char *sign) {
  // check for if myargv[] contend any sign
  int check_sign = false;
  for (int i = 0; i < myargc; i++) {
    if(strcmp(myargv[i],sign) == 0) {
      check_sign = true;
    }
  }
  return check_sign;
}


void execute_command_background (char *myargv[], int myargc) {
  // show the process ID at the top
  printf("Process ID: %d\n", getpid());
  myargv[myargc-1] = NULL;
  // execute user input command
  run_command(myargv);
  // show the precess ID at the end
  printf("ProcessID: %d  Done",getpid());
  printf("\t\t");
  // print all of user input
  for(int i = 0; i < myargc - 1; i++) {
    printf("%s", myargv[i]);
  }
  printf("\n");

}

void redirect_output_overwrite(char *myargv[], int myargc) {
	// record the filename 
	char *filename = myargv[myargc-1];
	int stdout_dup = dup(STDOUT_FILENO);
  // open file for write
  int output_file = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);

  // check if the output file create successful
  if(output_file < 0) {
    fprintf(stderr, "Error: fail to open or create a new file %s.\n", filename);
  } else {
    //duplicate stdout file descriptor to output file
  	dup2(output_file, STDOUT_FILENO);
  	// make rest command to NULL
   	int i = myargc - 2;
  	while(myargv[i] != NULL) {
  		myargv[i] = NULL;
  		i++;
   	}

   	run_command(myargv);
   	close(output_file);
   	//restore stdout file descriptor
   	dup2(stdout_dup, STDOUT_FILENO);
 	}
}

void redirect_output_append(char *myargv[], int myargc) {
	// record the filename 
	char *filename = myargv[myargc-1];
	int stdout_dup = dup(STDOUT_FILENO);
  	//open file for write
  	int output_file = open(filename, O_RDWR | O_CREAT | O_APPEND, 0644);
    // check if the output file create successful
  	if(output_file < 0) {
    	fprintf(stderr, "Error: fail to open or create a new file %s.\n", filename);
  	} else {
    	// duplicate stdout file descriptor to output file
    	dup2(output_file, STDOUT_FILENO);

    	// make rest command to NULL
    	int i = myargc - 2;
    	while(myargv[i] != NULL) {
    		myargv[i] = NULL;
    		i++;
    	}

    	run_command(myargv);
    	close(output_file);
    	//restore stdout file descriptor
    	dup2(stdout_dup, STDOUT_FILENO);
  	}
}

void redirect_input(char *myargv[], int myargc) {
	// record the filename 
	char *filename = myargv[myargc-1];

	//duplicate stdin file descriptor
  	int stdin_dup = dup(STDIN_FILENO);
  	// open a file for readonly
  	int input_file = open(filename, O_RDONLY);
    // check if the output file create successful
  	if(input_file < 0) {
    fprintf(stderr, "Error: fail to open a new file %s.\n", filename);
  	} else {
    	// duplicate stdin file descriptor to input file
    	dup2(input_file, STDIN_FILENO);

    	// make rest command to NULL
    	int i = myargc - 2;
    	while(myargv[i] != NULL) {
    		myargv[i] = NULL;
    		i++;
    	}

    	run_command(myargv);
    	close(input_file);
    	//restore stdin file descriptor
    	dup2(stdin_dup, STDIN_FILENO);
    }
}

void execute_multiple_commands (char *myargv[], int myargc) {
  char *left_myargv[ARGVMAX];
  int left_myargc;
  int pipe_start;
  int previous_pipe_index = -1;
  int fds[2];
  int fd_in  = -1;
  int fd_out = -1;

  // look up the "|" inside myargc
  for(int i = 0; i < myargc; i++) {
    if(strcmp(myargv[i], "|") == 0 || i == myargc - 1) {
      //process left command
      left_myargc = 0;

      if(previous_pipe_index > 0) {
        //update pipe_start
        pipe_start = previous_pipe_index + 1;
      } else {
        // set up pipe start from beginnning 
        pipe_start = 0;
      }

      if(i < myargc - 1) {
        //update prev_pipe_index
        previous_pipe_index = i;
        // copy left size of pipe into left_myargv
        while(pipe_start < i) {
          left_myargv[left_myargc++] = myargv[pipe_start];
          pipe_start++;
        }

        left_myargv[left_myargc] = NULL;
        // create a new pipe
        pipe(fds);
        // setup fd_out to be the write end of pipe
        fd_out = fds[1];
      } else {      
        // copy the last index of left_myargv
        while (pipe_start <= i) {
          left_myargv[left_myargc++] = myargv[pipe_start];
           pipe_start++;
        }   
        left_myargv[left_myargc] = NULL;
        //set fd_out to be -1 when final input is read
        fd_out = -1;
      }

      // create a new process
      pid = fork();
      if(pid < 0) {
        //creating new process fails
        printf("Creating new process failed.");
      } else if(pid == 0) {
        // dup the stdin to read end of pipe when not first command
        if(fd_in != -1 && fd_in != 0) {      
          dup2(fd_in, STDIN_FILENO);
          close(fd_in);
        }
        // dup the stdout to write end of pipe when not last command
        if(fd_out != -1 && fd_out != 1) {         
          dup2(fd_out, STDOUT_FILENO);
          close(fd_out);
        }
        //execute commands
        execute_commands(left_myargv, left_myargc);
        exit(0);
      } else {
        //parent process waits for child process
        waitpid(pid, NULL, 0);
        close(fd_in);
        close(fd_out);
        fd_in = fds[0];
      }
    } // The end of for loop  
  }
}
  
void run_command(char *myargv[]) {
  // creat a new process
	pid = fork();
  // check for precess create successful
  if(pid < 0) {
    printf("Creating new process failed.");
  } else if(pid == 0) {
    // execute command using execvp()
		execvp(myargv[0], myargv);
		printf("\n");
	} else {
    // waiting for child process done
		waitpid(pid, 0, 0);
	}
}

void change_directory() {
  // make default hoem diretory
	char *home_diretory = "/Users/Jangey";  
  // process command to home diretory
	if(myargv[1] == NULL || 
      (strcmp(myargv[1], "~") == 0) || 
      (strcmp(myargv[1], "~/")==0)) {
        chdir(home_diretory);
	} else if(chdir(myargv[1])<0) {
    // check for error and change current directory to myargv[1] 
   	printf("-bash: cd: %s: No such file or directory\n", myargv[1]);
	}
}

int main(int argc, char** argv)
{
	while (1) {
		// clear myargv[myargc] between successive commands
		clear_commands();
		// display the prompot with current working diretory
		prompt_display();
		// check EOF
		if(fgets(input_buffer, BUFFERSIZE, stdin) == NULL) {
			return 0;
		} 		
		// add empty sign at the end for input_buffer
		input_buffer_len = strlen(input_buffer); 
		input_buffer[input_buffer_len - 1] = '\0';
		// seperate commands line into myargv[myargc]
		seperate_commands();
		// execute the commands line using "myargv"
		execute_commands(myargv, myargc);
	}
	return 0;
}
