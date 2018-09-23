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

Command::Command() {
    // Initialize a new vector of Simple Commands
    _simpleCommands = std::vector<SimpleCommand *>();

    _outFile = NULL;
    _inFile = NULL;
    _errFile = NULL;
    _background = false;
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

    if ( _outFile ) {
        delete _outFile;
    }
    _outFile = NULL;

    if ( _inFile ) {
        delete _inFile;
    }
    _inFile = NULL;

    if ( _errFile ) {
        delete _errFile;
    }
    _errFile = NULL;

    _background = false;
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
			exit(1);
		}

		//clear
		if (!strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "clear")) {
			std::system("clear");
			clear();
			Shell::prompt();
			return;
		}


    // Print contents of Command data structure
    print();

    // Add execution here
    // For every simple command fork a new process
    // Setup i/o redirection
    // and call exec

		int defaultin = dup(0);
		int defaultout = dup(1);
		int defaulterr = dup(2);

		int fdin;
		int fdout;
		int fderr;


		int _numberOfSimpleCommands = (int) _currentSimpleCommand->_arguments.size();

		int ret;


		for (int i = 0; i < _numberOfSimpleCommands; i++) {
			ret = fork();
			if (ret == 0) {
				
				//convert to char** from vector
				char** cargument = new char*[_simpleCommands[i]->_arguments.size()];
				unsigned int j;
				for (j = 0; j < _simpleCommands[i]->_arguments.size(); j++) {
					cargument[j] = const_cast< char* >(_simpleCommands[i]->_arguments[j]->c_str());
					printf("%s\n", cargument[j]);
				}
				cargument[j] = NULL;

				execvp(_simpleCommands[i]->_arguments[0]->c_str(), cargument);

			} else if (ret < 0) {
				perror("fork");
				return;

			}

		}


		dup2(defaultin, 0);
		dup2(defaultout, 1);
		dup2(defaulterr, 2);
		close(defaultin);
		close(defaultout);
		close(defaulterr);

		if (!_background) {
			waitpid(ret, NULL, 0);
		}


    // Clear to prepare for next command
    clear();

    // Print new prompt
    Shell::prompt();
}

SimpleCommand * Command::_currentSimpleCommand;
