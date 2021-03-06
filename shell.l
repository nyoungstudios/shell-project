
/*
 *
 * CS-252 
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <cstring>
#include "y.tab.hh"
#include "shell.hh"
#include <unistd.h>

//////////start of edit mode////////////

extern "C" char *read_line();

int mygetc(FILE *f) {
	static char *p;
	char ch;

	if (!isatty(0)) {
		//stdin is not a tty. Call real getc
		return getc(f);
	}

	//stdin is a tty. Call our read_line
	if (p == NULL || *p == 0) {
		char *s = read_line();
		p = s;
	}

	ch = *p;
	p++;

	return ch;

}

#undef getc
#define getc(f) mygetc(f)

////////end of edit mode ////////////


static void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
  unput(c);
}

extern char *shellpath;
int backgroundPID;
int lastExitCode;

//function for source
void source(FILE *file, bool firstTry) {
		Shell::_currentCommand._source = true;
		YY_BUFFER_STATE myBufferState = yy_create_buffer(file, 100);
		yypush_buffer_state(myBufferState);
		yyparse();
		fclose(file);
		yypop_buffer_state();
		yy_flush_buffer(myBufferState);
		Shell::_currentCommand._source = false;
		//YY_BUFFER_STATE newBufferState = yy_create_buffer(NULL, 100);

		//first try flag
		if (firstTry) {
			yyrestart(stdin);
		}
}

//function to expand environment variables
std::string expandEnv(char *str) {
	std::string command;
	int i;

	//iterates over all characters
	for (i = 0; i < strlen(str) - 1; i++) {

		if (*(str + i) == '$' && *(str + i + 1) == '{') {
			//variable name for environment variable name
			std::string varName;
			
			//iterates over environment variable name
			for (i = i + 2; i < strlen(str); i++) {

				//checks if end of environment variable name	
				if (*(str + i) == '}') {
					char *var = (char *) malloc (200);	
					if (!strcmp(varName.c_str(), "SHELL")) {
						var = shellpath;
					} else if (!strcmp(varName.c_str(), "$")) {
						sprintf(var, "%d", getpid());
					} else if (!strcmp(varName.c_str(), "?")) {
						sprintf(var, "%d", lastExitCode);
					} else if (!strcmp(varName.c_str(), "!")) {
						sprintf(var, "%d", backgroundPID);
					} else if (!strcmp(varName.c_str(), "_")) {
						var = getenv(varName.c_str());
					} else {
						var = getenv(varName.c_str());
					}
					if (var != 0) {
						//printf("%s\n", var);
						command += var;
					}
					break;
				} else {
					varName += *(str + i);
					//printf("%c", *(str + i));
				}
				
			}
		} else {
			command += *(str + i);
		}
	}

	//checks for last character	
	if (*(str + strlen(str) - 1) != '}') {
		command += *(str + strlen(str) - 1);
	}
	//printf("%s\n", command.c_str());
	//printf("\n");
	return command;
}


%}

%%

\n {
  return NEWLINE;
}

[ \t] {
  /* Discard spaces and tabs */
}

"ls 2> /dev/null" {
	yylval.cpp_string = new std::string("ls");
	return WORD;
}

">>&" {
	return GREATGREATAMP;
}

">>" {
	return GREATGREAT;
}

"2>" {
	return TWOGREAT;
}

">&" {
	return GREATAMP;
}

">" {
  return GREAT;
}

"&" {
	return AMP;
}

"|" {
	return PIPE;
}

"<" {
	return LESS;
}

"$"[^\n\t{}()][^\n\t{}()]* {	
	const char * first = (const char *) yytext + 1;
	char *var = (char *) malloc(200);
	if (!strcmp(first, "SHELL")) {
		var = shellpath;
	} else if (!strcmp(first, "$")) {
		sprintf(var, "%d", getpid());
	} else if (!strcmp(first, "?")) {
		sprintf(var, "%d", lastExitCode);
	} else if (!strcmp(first, "!")) {
		sprintf(var, "%d", backgroundPID);
	} else if (!strcmp(first, "_")) {
		var = getenv(first);
	} else {
		var = getenv(first);
	}
	if (var != 0) {	
		yylval.cpp_string = new std::string(var);
	} else {
		yylval.cpp_string = new std::string("");
	}
	return WORD;
}

"source "[^\n\t][^\n\t]* {
	const char * first = (const char *) yytext + 7;
	std::string text = expandEnv((char *) first);
	FILE *fp = fopen(text.c_str(), "r");
	if (fp == NULL) {
		if (strlen(text.c_str()) == 0) {
			fprintf(stderr, "source: not enough arguments\n");
		} else {
			fprintf(stderr, "cannot open %s: No such file\n", text.c_str());
		}
	} else {
		source(fp, false);
	}
	
}

"$("[^\n)$]*")" {
	//get command
	const char * first = (const char *) yytext + 2;
	char *last = (char *) yytext + strlen(yytext) - 1;
	*last = '\0';

	int tmpin = dup(0);
	int tmpout = dup(1);
	
	int fdpipein[2];
	int fdpipeout[2];

	pipe(fdpipein);
	pipe(fdpipeout);

	write(fdpipein[1], first, strlen(first));
	write(fdpipein[1], "\n", 1);

	close(fdpipein[1]);

	dup2(fdpipein[0], 0);
	close(fdpipein[0]);
	dup2(fdpipeout[1], 1);
	close(fdpipeout[1]);

	//fork and execute process
	int ret = fork();
	if (ret == 0) {
		execvp("/proc/self/exe", NULL);
		_exit(1);
	} else if (ret < 0) {
		perror("fork");
		exit(1);
	}

	dup2(tmpin, 0);
	dup2(tmpout, 1);
	close(tmpin);
	close(tmpout);

	char ch;
	char * buffer = (char *) malloc (4096);
	int i = 0;

	//read the output of the subshell from the pipe
	while (read(fdpipeout[0], &ch, 1)) {
		if (ch == '\n') {
			buffer[i++] = ' ';
		} else {
			buffer[i++] = ch;
		}
	}

	//sets terminating character
	buffer[--i] = '\0';

	for (i = strlen(buffer); i >= 0; i--) {
		unput(buffer[i]);
	}

}

"${"[^\n]*"}" {
	std::string text = expandEnv((char *) yytext);
	yylval.cpp_string = new std::string(text);

	return WORD;

}

["][^\n\"]*["] {
	//remove quotes and return word
	const char *first = (const char *) yytext + 1;
	char *last = (char *) yytext + strlen(yytext) - 1;
	*last = '\0';
	std::string text = expandEnv((char *) first);
	yylval.cpp_string = new std::string(text);
	return WORD;
}

['][^\n\']*['] {
	//remove quotes and return word
	const char *first = (const char *) yytext + 1;
	char *last = (char *) yytext + strlen(yytext) - 1;
	*last = '\0';
	std::string text = expandEnv((char *) first);
	yylval.cpp_string = new std::string(text);
	return WORD;
}

[^ \t\n\|\>\<\&]*[\\][^ \t\n]* {
	int i = 0;
	char * escape = (char *) malloc(strlen(yytext));
	char * temp = yytext;

	//iterates through each character to check for escapes
	while (*temp) {
		if (*temp == '\\') {
			if (*(temp+1) == '\\') {
				temp++;
				escape[i++] = '\\';
			} else if (*(temp+1) == '>') {
				temp++;
				escape[i++] = '>';
			} else if (*(temp+1) == '<') {
				temp++;
				escape[i++] = '<';
			} else if (*(temp+1) == '\"') {
				temp++;
				escape[i++] = '\"';
			} else if (*(temp+1) == '\'') {
				temp++;
				escape[i++] = '\'';
			} else if (*(temp+1) == '&') {
				temp++;
				escape[i++] = '&';
			} else if (*(temp+1) == '|') {
				temp++;
				escape[i++] = '|';
			} else if (*(temp+1) == '~') {
				temp++;
				escape[i++] = '~';
			} else if (*(temp+1) == '*') {
				temp++;
				escape[i++] = '*';
			} else if (*(temp+1) == '(') {
				temp++;
				escape[i++] = '(';
			} else if (*(temp+1) == ')') {
				temp++;
				escape[i++] = ')';
			} else {
				//escape[i++] = *(temp);
				temp++;
			}

		} else {
			escape[i++] = *temp;
		}

		temp++;


	}
	escape[i] = '\0';

	yylval.cpp_string = new std::string(escape);
	return WORD;

}

[^ \t\n\|\>\<\&][^ \t\n\|\>\<\&]*  {
  /* Assume that file names have only alpha chars */

	//test if first character is a tilde (~)
  const char *first = (const char *) yytext;
	if (*first == '~') {
		std::string home = getenv("HOME");
		char *second = (char *) yytext + 1;
		if (*second == '/') {
			home += second;
		} else if (strlen(second) >= 1) {
			home = "/homes/";
			home += second;
		}
		std::string text = expandEnv((char *) home.c_str());
		yylval.cpp_string = new std::string(text); 	
	} else {
		std::string text = expandEnv((char *) yytext);
	  yylval.cpp_string = new std::string(text);	
	}
  return WORD;
}
