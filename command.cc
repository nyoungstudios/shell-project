/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 * DO NOT PUT THIS PROJECT IN A PUBLIC REPOSITORY LIKE GIT. IF YOU WANT 
 * TO MAKE IT PUBLICALLY AVAILABLE YOU NEED TO REMOVE ANY SKELETON CODE 
 * AND REWRITE YOUR PROJECT SO IT IMPLEMENTS FUNCTIONALITY DIFFERENT THAN
 * WHAT IS SPECIFIED IN THE HANDOUT. WE OFTEN REUSE PART OF THE PROJECTS FROM  
 * SEMESTER TO SEMESTER AND PUTTING YOUR CODE IN A PUBLIC REPOSITORY
 * MAY FACILITATE ACADEMIC DISHONESTY.
 */

#include <cstdio>
#include <cstdlib>

#include <iostream>

#include "command.hh"
#include "shell.hh"
#include "string.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>

Command::Command() {
    // Initialize a new vector of Simple Commands
    _simpleCommands = std::vector<SimpleCommand *>();

    _outFile = NULL;
    _inFile = NULL;
    _errFile = NULL;
    _background = false;
		_append = 0;
		_inCounter = 0;
		_outCounter = 0;

}

void Command::insertSimpleCommand( SimpleCommand * simpleCommand ) {
    // add the simple command to the vector
    _simpleCommands.push_back(simpleCommand);
}

void Command::clear() {
    // deallocate all the simple commands in the command vector
    for (auto simpleCommand : _simpleCommands) {
        delete simpleCommand;
    }

    // remove all references to the simple commands we've deallocated
    // (basically just sets the size to 0)
    _simpleCommands.clear();

		if (_outFile == _errFile) {
				if ( _outFile ) {
					delete _outFile;
			}
			_outFile = NULL;
			_errFile = NULL;

		} else {
			if ( _outFile ) {
					delete _outFile;
			}
			_outFile = NULL;
			if ( _errFile ) {
					delete _errFile;
			}
			_errFile = NULL;

		}

    if ( _inFile ) {
        delete _inFile;
    }
    _inFile = NULL;

    _background = false;

		_append = 0;	

		_inCounter = 0;

		_outCounter = 0;

	




}

void Command::print() {
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    int i = 0;
    // iterate over the simple commands and print them nicely
    for ( auto & simpleCommand : _simpleCommands ) {
        printf("  %-3d ", i++ );
        simpleCommand->print();
    }

    printf( "\n\n" );
    printf( "  Output       Input        Error        Background\n" );
    printf( "  ------------ ------------ ------------ ------------\n" );
    printf( "  %-12s %-12s %-12s %-12s\n",
            _outFile?_outFile->c_str():"default",
            _inFile?_inFile->c_str():"default",
            _errFile?_errFile->c_str():"default",
            _background?"YES":"NO");
    printf( "\n\n" );
}

void Command::execute() {
    // Don't do anything if there are no simple commands
    if ( _simpleCommands.size() == 0 ) {
        Shell::prompt();
        return;
    }
	
		//quit shell
		if (!strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "exit") || !strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "quit")) {
			printf("Good bye!!\n");
			exit(1);
		}

		//clear
		if (!strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "clear")) {
			std::system("clear");
			clear();
			Shell::prompt();
			return;
		}

		//setenv
		if (!strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "setenv")) {
			int error = setenv(_simpleCommands[0]->_arguments[1]->c_str(), _simpleCommands[0]->_arguments[2]->c_str(), 1);
			if (error < 0) {
				perror("setenv");
			}
			clear();
			Shell::prompt();
			return;
		}

		//unsetenv
		if (!strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "unsetenv")) {
			int error = unsetenv(_simpleCommands[0]->_arguments[1]->c_str());
			if (error < 0) {
				perror("unsetenv");
			}
			clear();
			Shell::prompt();
			return;
		}


		//cd
		if (!strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "cd")) {
			int error;
			if (_simpleCommands[0]->_arguments.size() == 1 || !strcmp(_simpleCommands[0]->_arguments[1]->c_str(), "~")) {
				error = chdir(getenv("HOME"));
				std::string pwd = getenv("PWD");
				std::string home = getenv("HOME");
				
				if (error >= 0) {
					setenv("PWD", home.c_str(), 1);
					setenv("OLDPWD", pwd.c_str(), 1);
				}

			} else if ((const char) *_simpleCommands[0]->_arguments[1]->c_str() == '~') {
				std::string newPath = getenv("HOME");
				
				std::string secondPart = (const char *) (_simpleCommands[0]->_arguments[1]->c_str() + 1);

				if ((const char) *(_simpleCommands[0]->_arguments[1]->c_str() + 1) == '/') {
					newPath += secondPart;
				} else {
					newPath += "/";
					newPath += secondPart;
				}
				error = chdir(newPath.c_str());
				
				std::string pwd = getenv("PWD");
				if (error >= 0) {
					setenv("PWD", newPath.c_str(), 1);
					setenv("OLDPWD", pwd.c_str(), 1);
				}



			} else if(!strcmp(_simpleCommands[0]->_arguments[1]->c_str(), "-")) {
				error = chdir(getenv("OLDPWD"));
				std::string pwd = getenv("PWD");
				std::string oldpwd = getenv("OLDPWD");
				
				if (error >= 0) {
					setenv("PWD", oldpwd.c_str(), 1);
					setenv("OLDPWD", pwd.c_str(), 1);
				}

			} else {
				error = chdir(_simpleCommands[0]->_arguments[1]->c_str());
				std::string pwd = getenv("PWD");
				std::string pwdString = pwd;
				pwdString.append("/");
				pwdString.append(_simpleCommands[0]->_arguments[1]->c_str());

				if (error >= 0) {
					setenv("PWD", pwdString.c_str(), 1);
					setenv("OLDPWD", pwd.c_str(), 1);
				}
			}


			if (error < 0) {
				fprintf(stderr, "/bin/sh: 1: cd: can't cd to %s\n", _simpleCommands[0]->_arguments[1]->c_str());
			}
			clear();
			Shell::prompt();
			return;
		}




    // Print contents of Command data structure
    //print();

    // Add execution here
    // For every simple command fork a new process
    // Setup i/o redirection
    // and call exec

		//sets default in, out, and err	
		int defaultin = dup(0);
		int defaultout = dup(1);
		int defaulterr = dup(2);

		//file descriptors
		int fdin;
		int fdout;
		int fderr;
	


		if (_inCounter > 1 || _outCounter > 1) {
			fprintf(stderr, "Ambiguous output redirect.\n");
		}	
		
		//sets in file
		if (_inFile) {
			fdin = open(_inFile->c_str(), O_RDONLY, 0664);
			if (fdin < 0) {
				fprintf(stderr, "/bin/sh: 1: cannot open %s: No such file\n", _inFile->c_str());
				clear();
				Shell::prompt();
				return;
			}
		} else {
			fdin = dup(defaultin);
		}

		
		//gets number of simple commands
		int _numberOfSimpleCommands = (int) _simpleCommands.size();
		//printf("this is the number of simple commands: %d\n", _numberOfSimpleCommands);


		//variable for forking
		int ret;

		//iterates over all the simple commands
		for (int i = 0; i < _numberOfSimpleCommands; i++) {
	
					
			dup2(fdin, 0);
			close(fdin);

			//setup output
			if (i == _numberOfSimpleCommands - 1) {
				if (_outFile && _errFile) {  //if both out and err are to the same file
					if (!_append) {
						remove(_outFile->c_str());
					}
					
					fdout = open(_outFile->c_str(), O_WRONLY | O_CREAT | O_APPEND, 0664);
					fderr = open(_errFile->c_str(), O_WRONLY | O_CREAT | O_APPEND, 0664);	
					
					if (fdout < 0) {
						perror("outFile open error");
						exit(1);
					}

					if (fderr < 0) {
						perror("errFile open error");
						exit(1);
					}
	
				} else if (_outFile) {  //if just out is redirected
					if (_append) {
						fdout = open(_outFile->c_str(), O_WRONLY | O_CREAT | O_APPEND, 0664);
					} else {
						fdout = open(_outFile->c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0664);
					}
			
					
					if (fdout < 0) {
						perror("outFile open error");
						exit(1);
					}
					

				} else if (_errFile) {  //if just err is redirected
					if (_append) {
						fderr = open(_errFile->c_str(), O_WRONLY | O_CREAT | O_APPEND, 0664);
					} else {
						fderr = open(_errFile->c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0664);
					}
			
					
					if (fderr < 0) {
						perror("errFile open error");
						exit(1);
					}	
								
				} else {
					fdout = dup(defaultout);
					fderr = dup(defaulterr);
				}

			} else {
				//if not last simple command, create a pipe
				int fdPipe[2];
				pipe(fdPipe);
				fdout = fdPipe[1];
				fdin = fdPipe[0];

			}

			dup2(fdout, 1);
			close(fdout);
			dup2(fderr, 2);
			close(fderr);

			
			
			//fork
			ret = fork();
			if (ret == 0) {
			
	
				//convert to char** from vector
				char** cargument = new char*[_simpleCommands[i]->_arguments.size()];
				unsigned int j;
				for (j = 0; j < _simpleCommands[i]->_arguments.size(); j++) {
					cargument[j] = const_cast< char* >(_simpleCommands[i]->_arguments[j]->c_str());
				}
				cargument[j] = NULL;
				
				if (!strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "printenv")) {
					char **env = environ;
					while (*env != NULL) {
						printf("%s\n", *env);
						env++;
					}
					exit(0);
				
				}
				

				//execute command
				execvp(_simpleCommands[i]->_arguments[0]->c_str(), cargument);


			} else if (ret < 0) {
				perror("fork");
				return;

			}

			//checks if not background. If true, waits for command to finish
			if (!_background) {
				waitpid(ret, NULL, 0);
			}	


		}

		//resets defaults
		dup2(defaultin, 0);
		dup2(defaultout, 1);
		dup2(defaulterr, 2);
		close(defaultin);
		close(defaultout);
		close(defaulterr);



    // Clear to prepare for next command
    clear();

    // Print new prompt
    Shell::prompt();
}

SimpleCommand * Command::_currentSimpleCommand;
