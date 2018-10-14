
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

static  void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
  unput(c);
}

void source(FILE *file, bool firstTry) {
		Shell::_currentCommand._source = true;
		YY_BUFFER_STATE myBufferState = yy_create_buffer(file, 100);
		yypush_buffer_state(myBufferState);
		yyparse();
		fclose(file);
		yypop_buffer_state();
		yy_flush_buffer(myBufferState);
		Shell::_currentCommand._source = false;
		YY_BUFFER_STATE newBufferState = yy_create_buffer(NULL, 100);
		if (firstTry) {
			yyrestart(stdin);
		}
}


%}

%%

\n {
  return NEWLINE;
}

[ \t] {
  /* Discard spaces and tabs */
}

"^[[A" {

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

"source "[^\n\t][^\n\t]* {
	const char * first = (const char *) yytext + 7;
	FILE *fp = fopen(first, "r");
	if (fp == NULL) {
		fprintf(stderr, "cannot open %s: No such file\n", first);
	} else {
		source(fp, false);
		//Shell::_currentCommand._source = true;
		//YY_BUFFER_STATE myBufferState = yy_create_buffer(fp, 100);
		//yypush_buffer_state(myBufferState);
		//yyparse();
		//fclose(fp);
		//yypop_buffer_state();
		//Shell::_currentCommand._source = false;
	}
	
}


[\$\(][^\n]*[\)] {
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


[\$\{][^\n]*[\}] {
	const char * first = (const char *) yytext + 2;
	char *last = (char *) yytext + strlen(yytext) - 1;
	*last = '\0';
	std::string var = getenv(first);
	yylval.cpp_string = new std::string(var);
	return WORD;

}

["][^\n\"]*["] {
	//remove quotes and return word
	const char *first = (const char *) yytext + 1;
	char *last = (char *) yytext + strlen(yytext) - 1;
	*last = '\0';
	yylval.cpp_string = new std::string(first);
	return WORD;
}


['][^\n\']*['] {
	//remove quotes and return word
	const char *first = (const char *) yytext + 1;
	char *last = (char *) yytext + strlen(yytext) - 1;
	*last = '\0';
	yylval.cpp_string = new std::string(first);
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
		yylval.cpp_string = new std::string(home); 	
	} else {
	  yylval.cpp_string = new std::string(yytext);	
	}
  return WORD;
}
