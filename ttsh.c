/*
 * The Tiny Torero Shell (TTSH)
 *
 * Authors: Megan Bailey and Jake Wahl
 *
 * Simulates basic shell
 */

#define _XOPEN_SOURCE 600

// Imports
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>

#include "parse_args.h"
#include "history_queue.h"

// Forward Declarations
void execute(char *argv[MAXARGS], char cmdline[], unsigned int *id);
void call(char *argv[MAXARGS], char cmdline[]);
void addEntry(char cmdline[], unsigned int *id);
void zombieKiller(__attribute__((unused)) int sig);
void changeDirectory(char *argv[MAXARGS], char cmdline[]);
void executeHistory(char *argv[MAXARGS], char cmdline[], unsigned int *id);


// Main
int main() { 

	char *argv[MAXARGS];
	unsigned int id = 1;
	// Setting up signal handler
	struct sigaction sa;
	sa.sa_handler = zombieKiller;
	sa.sa_flags = 0;   
	sigaction(SIGCHLD, &sa, NULL);

	// Loop that continuously takes in user input and executes user commands
	while(1) {

		// (1) print the shell prompt
		printf("ttsh> ");  
		fflush(stdout);
		// (2) read in the next command entered by the user
		char cmdline[MAXLINE];
		if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)) {
			// fgets could be interrupted by a signal.
			// This checks to see if that happened, in which case we simply
			// clear out the error and restart our loop so it re-prompts the
			// user for a command.
			clearerr(stdin);
			continue;
		}
		// Check to see if we reached the end "file" for stdin
		if (feof(stdin)) {
			fflush(stdout);
			exit(0);
		}
		// (3) make a call to parseArguments function to parse it into its argv
		// format
		parseArguments(cmdline, argv);
		if (argv[0] == NULL) 
		{ continue; }
		// (4) Call a function that will determine how to execute the command
		// that the user entered, and then execute it
		execute(argv, cmdline, &id);
	}
	return 0;
}



/*
 * Executes commands for shell
 *
 * @param back Whether the command should be run in the background or not
 * @param argv An array of parsed arguments from command line input
 * @param cmdline Raw user entered input
 * @param id ID number for command we are running
 *
 * @return void
 */
void execute(char *argv[MAXARGS], char cmdline[], unsigned int *id) {
	
	parseArguments(cmdline, argv);

	// Case for "!num" command
	if (argv[0][0] == '!') {
		executeHistory(argv, cmdline, id);
	}
	// Case for "exit" command
	else if (strncmp(argv[0], "exit", MAXLINE) == 0) {
		// PRint out message, flush, then exit
		printf("Behind Every Great Programmer Is A Great Shell...\n");	
		exit(0);
	}
	// Case for "history" command
	else if (strncmp(argv[0], "history", MAXLINE) == 0) {	
		// Add command to Q and print Q
		addEntry(cmdline, id);
		print();			
	}
	// Case for "cd" comamnd
	else if (strncmp(argv[0], "cd", MAXLINE) == 0) {
		addEntry(cmdline, id);
		changeDirectory(argv, cmdline);
	} 	
	// Case for all other commands
	else  { 
		addEntry(cmdline, id);
		call(argv, cmdline);
	}
}



/*
 * Executes non-built-in commands
 * 
 * @param argv Array of parsed arguments
 * @param cmdline The user-entered command line
 *
 * @return void
 */
void call(char *argv[MAXARGS], char cmdline[]) {
	pid_t pid;
	int status;

	int back = parseArguments(cmdline, argv);
					
	if ((pid = fork()) == 0) { // child
		if ((execvp(argv[0], argv)) == -1) {
			printf("ERROR: Command not found\n");
			exit(0);
		}
		exit(0);	
	}
	else if ((pid > 0) && !back) { //parent & wait for child
		if((waitpid(pid, &status, 0)) == -1) {
			printf("Child did not exit normally.\n");		
		}
	}
}



/*
 * Function to add an entry to out history Q
 *
 * @param cmdline Commandline entered in by User
 * @param id The id for the command
 *
 * @return void 
 */
void addEntry(char cmdline[], unsigned int *id) {
	HistoryEntry new;
	strncpy(new.cmdline, cmdline, MAXLINE);
	new.cmd_num = *id;
	add(new);
	++*id;
}



/*
 * Function executes a specified command from the history queue
 *
 * @param argv Array that stores the parsed arguments
 * @param cmdline The user-entered command line 
 * @param id The id number of the the command in history to be executed
 *
 * @return void
 */
void executeHistory(char *argv[MAXARGS], char cmdline[], unsigned int *id) {

	parseArguments(cmdline, argv);
	// Creating struct to hold result and searchign history Q
	unsigned int n = (unsigned int) strtol(argv[0] + 1, NULL, 10);
	HistoryEntry target = search(n);

	// Testign if number not found
	if (target.cmd_num == 4294967295) {
		printf("\tERROR: not found\n");
		//fflush(stdout);
		return;
	}
	// Excecuting command from history
	execute(argv, target.cmdline, id);
}



/*
 * Changes the directory the User is in
 *
 * @param argv Array of parsed arguments
 * @cmdline The user-entered command line
 *
 * @return void
 */
void changeDirectory(char *argv[MAXARGS], char cmdline[]) {
	int error;

	parseArguments(cmdline, argv);

	// Chacking if no path specified
	if (argv[1] == NULL) {
		chdir(getenv("HOME"));
	} 
	else {
		error = chdir(argv[1]);
		if (error == -1) {
			printf("Invaild directory\n");
			//fflush(stdout);
			return;
		}
	}
}



/*
 * Handler to deal with zombies
 *
 * @param sig Incomming signal id
 *
 * @return void
 */
void zombieKiller(__attribute__((unused)) int sig) {	
	while ((waitpid(-1, NULL, WNOHANG)) > 0){}
}
