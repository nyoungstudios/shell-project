#include <cstdio>

#include "shell.hh"
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int yyparse(void);

void Shell::prompt() {
	/*
	const char *name = "PROMPT";
	char *value = getenv(name);

	const char *error = "ON_ERROR";
	char *errorValue = getenv(error);

	const char *exitCode = "$?";
	char *exitValue = getenv(exitCode);
	char *zero = (char*) "0";

	if (errorValue != NULL && strcmp(exitValue, zero)) {
		printf("%s\n", errorValue);
	}
	*/

	if (isatty(0)) {
		printf("myshell>");
		fflush(stdout);
	}
}

void controlC(int sig) {
	printf("\n");
	Shell::_currentCommand.clear();
	Shell::prompt();
	
}

void zombie(int sig) {
	while (waitpid(-1, NULL, WNOHANG) > 0);
}


int main() {

	int error;

	struct sigaction sa1;
	sa1.sa_handler = controlC;
	sigemptyset(&sa1.sa_mask);
	sa1.sa_flags = SA_RESTART;
	error = sigaction(SIGINT, &sa1, NULL);
	if (error == -1) {
		perror("sigaction");
		exit(1);
	}

	struct sigaction sa2;
	sa2.sa_handler = zombie;
	sa2.sa_flags = SA_RESTART;
	sigemptyset(&sa2.sa_mask);
	error = sigaction(SIGCHLD, &sa2, NULL);
	if (error == -1) {
		perror("sigaction");
		exit(1);
	}

	FILE* fd = fopen(".shellrc", "r");
	if (fd) {
		yyrestart(fd);
		yyparse();
		yyrestart(stind);
		fclose(fd);
	} else {


	  Shell::prompt();
	}
  yyparse();
}

Command Shell::_currentCommand;
