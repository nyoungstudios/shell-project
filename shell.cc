#include <cstdio>

#include "shell.hh"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int yyparse(void);

extern void source(FILE *file, bool firstTry);

extern int lastExitCode;

char *shellpath;

void Shell::prompt() {
	if (isatty(0)) {
		
		char *value = getenv("PROMPT");

		char *errorValue = getenv("ON_ERROR");

		if (errorValue != NULL && lastExitCode != 0) {
			printf("%s\n", errorValue);
		}
	
		if (value != NULL) {
			printf("%s", value);
		} else {
			printf("myshell>");
		}
		fflush(stdout);
	}
}

void controlC(int sig) {
	printf("\n");
	Shell::_currentCommand.clear();
	Shell::prompt();
	printf("\r");
	
}

void zombie(int sig) {
	while (waitpid(-1, NULL, WNOHANG) > 0);
}


int main(int argc, char **argv) {

	int error;

	//signal handler for control c
	struct sigaction sa1;
	sa1.sa_handler = controlC;
	sigemptyset(&sa1.sa_mask);
	sa1.sa_flags = SA_RESTART;
	error = sigaction(SIGINT, &sa1, NULL);
	if (error == -1) {
		perror("sigaction");
		exit(1);
	}

	//signal handler for zombie processes
	struct sigaction sa2;
	sa2.sa_handler = zombie;
	sa2.sa_flags = SA_RESTART;
	sigemptyset(&sa2.sa_mask);
	error = sigaction(SIGCHLD, &sa2, NULL);
	if (error == -1) {
		perror("sigaction");
		exit(1);
	}

	//reads default source
	std::string path = getenv("HOME");
	
	path += "/.shellrc";

	FILE *fp = fopen(path.c_str(), "r");

	if (fp != NULL) {
		source(fp, true);
	}

	//for shell real path
	char actualpath [256];
	char *ptr;
	ptr = realpath(argv[0], actualpath);
	shellpath = actualpath;
	//printf("%s\n", shellpath);



	//prints prompt and parses
	if (!Shell::_currentCommand._source) {
  	Shell::prompt();
	}
  yyparse();
}

Command Shell::_currentCommand;
