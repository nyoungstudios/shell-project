#include <cstdio>

#include "shell.hh"

int yyparse(void);

void Shell::prompt() {
	if (isatty(0)) {
		printf("myshell>");
		fflush(stdout);
	}
}

void controlC(int sig) {
	printf("\n");
	Shell::_currentCommand.clear();
	Shell::_prompt();
	
}

void zombie(int sig) {
	while (waitpid(-1, NULL, WNOHANG) > );
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



  Shell::prompt();
  yyparse();
}

Command Shell::_currentCommand;
