This program written in C language to implement own Shell.
Compile the program command:
	gcc myshell.c -o myshell
Execute the program command:
	./myshell



The read lines of user input is size of 256-byte into a series 
of strings that are stored into "char myargv**" array. And along 
with a count of the number of string in an "int myargc" variable. 



The program using following command:
"pid_t pid = fork()" 	to make child and parent process.
"eyecvp()"		to process "char myargv**" array.
"waitpid()"		to wait for the child class done the process. 
"getcwd()" 		to see the current working directory,
"chdir()" 		to execute "cd" commands to change current working directory.
"pipe()"		to create stdin -> stdout in a pipe
"dup()"			to duplicate an existing file
"dup2()"		to duplicate an new file from old file



The shell program could support the following function. 

• Execute a single command with up to four command line arguments
	(including command line arguments with associated flags). 
	For example:
		 	Myshell> ls –l
			Myshell> cat myfile
			Myshell> ls –al /usr/src/linux

• Execute a command in background. 
	For example:
			Myshell> ls -­­l &
			Myshell> ls –al /usr/src/linux &

• Redirect the standard output of a command to a file. 
	For example:
			Myshell> ls -­­l > outfile
			Myshell> ls -­­l >> outfile
			Myshell> ls –al /usr/src/linux > outfile2 
			Myshell> ls –al /usr/src/linux >>outfile2

• Redirect the standard input of a command to come from a file. 
	For example: 
			Myshell> grep disk < outfile
			Myshell> grep linux < outfile2

• Execute multiple commands connected by a single shell pipe. 
	For example:
			Myshell> ls –al /usr/src/linux | grep linux

• Execute the cd and pwd commands.
	For example:
			Myshell> cd some_path
			Myshell> pwd

• Implement the shell so the current working directory is shown on the prompt. 
	For example:
			OLD PROMPT: 
				myshell >>

			NEW PROMPT: 
				myshell ~/ >>
				myshell ~/hw3 >> myshell ~/hw3/build >>
