
/*
 * CS-252
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%code requires 
{
#include <string>

#if __cplusplus > 199711L
#define register      // Deprecated in C++11 so remove the keyword
#endif
}

%union
{
  char        *string_val;
  // Example of using a c++ type in yacc
  std::string *cpp_string;
}

%token <cpp_string> WORD
%token NOTOKEN GREAT NEWLINE PIPE GREATGREAT AMP GREATGREATAMP GREATAMP LESS TWOGREAT

%{
//#define yylex yylex
#include <cstdio>
#include "shell.hh"
#include <string.h>

void expandWildcardsIfNecessary(const char * arg);
void yyerror(const char * s);
int yylex();

%}

%%

goal:
  commands
  ;

commands:
  command
  | commands command
  ;

command: simple_command
       ;

simple_command:	
  pipe_list iomodifier_list background_optional NEWLINE {
    //printf("   Yacc: Execute command\n");
    Shell::_currentCommand.execute();
  }
  | NEWLINE {
		Shell::prompt();
		//Shell::_currentCommand.execute();
	} 
  | error NEWLINE { 
		yyerrok;
		Shell::_currentCommand.clear();
		printf("\n");
		Shell::prompt();
	}
  ;

pipe_list:
	pipe_list PIPE command_and_args
	| command_and_args
	;


command_and_args:
  command_word argument_list {
    Shell::_currentCommand.
    insertSimpleCommand( Command::_currentSimpleCommand );
  }
  ;

argument_list:
  argument_list argument
  | /* can be empty */
  ;

argument:
  WORD {
    //printf("   Yacc: insert argument \"%s\"\n", $1->c_str());
    //Command::_currentSimpleCommand->insertArgument( $1 );
		expandWildcardsIfNecessary($1->c_str());
  }
  ;

command_word:
  WORD {
    //printf("   Yacc: insert command \"%s\"\n", $1->c_str());
    Command::_currentSimpleCommand = new SimpleCommand();
    //Command::_currentSimpleCommand->insertArgument( $1 );
		expandWildcardsIfNecessary($1->c_str());
  }
  ;

iomodifier_opt:
  GREAT WORD {
    //printf("   Yacc: insert output \"%s\"\n", $2->c_str());
		Shell::_currentCommand._outFile = $2;
		Shell::_currentCommand._outCounter++;
  }
	| GREATGREAT WORD {
		Shell::_currentCommand._outFile = $2;
		Shell::_currentCommand._append = 1;
		Shell::_currentCommand._outCounter++;
	}
	| GREATGREATAMP WORD {
		Shell::_currentCommand._outFile = $2;
		Shell::_currentCommand._errFile = $2;
		Shell::_currentCommand._append = 1;
		Shell::_currentCommand._outCounter++;
	}
	| GREATAMP WORD {
		Shell::_currentCommand._outFile = $2;
		Shell::_currentCommand._errFile = $2;
		Shell::_currentCommand._outCounter++;
	}	
	| LESS WORD {
		Shell::_currentCommand._inFile = $2;
		Shell::_currentCommand._inCounter++;
	}
	| TWOGREAT WORD {
		Shell::_currentCommand._errFile = $2;
		Shell::_currentCommand._outCounter++;
	}
  ;

iomodifier_list:
	iomodifier_list iomodifier_opt
	| iomodifier_opt
	|
	;

background_optional:
	AMP {
		Shell::_currentCommand._background = 1;
	}
	|
	;


%%

void expandWildcardsIfNecessary(char *arg) {
	if (strcmp(arg, "*") || strcmp(arg, "?")) {

	} else {
		std::string strArg;
		strArg.push_back(*arg);
		std::string argument = const_cast<std::string>(arg);
		Command::_currentSimpleCommand->insertArgument(argument);
	}
	return;

}


void
yyerror(const char * s)
{
  fprintf(stderr,"%s", s);
}

#if 0
main()
{
  yyparse();
}
#endif
