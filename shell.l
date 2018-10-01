
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

static  void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
  unput(c);
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

[\$\(][^\n]*[\)] {
	const char * first = (const char *) yytext + 2;
	char *last = (char *) yytext + strlen(yytext) - 1;
	*last = '\0';
	//printf("%s\n", first);
	//printf("%ld\n", strlen(first));

	int tmpin = dup(0);
	int tmpout = dup(1);
	
	int fdpipein[2];
	int fdpipeout[2];

	pipe(fdpipein);
	pipe(fdpipeout);

	write(fdpipein[1], first, strlen(first));
	write(fdpipein[1], "\n", 1);
	//write(fdpipein[1], "exit", 4);
	//write(fdpipein[1], "\n", 1);

	close(fdpipein[1]);

	dup2(fdpipein[0], 0);
	close(fdpipein[0]);
	dup2(fdpipeout[1], 1);
	close(fdpipeout[1]);

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

	while (read(fdpipeout[0], &ch, 1)) {
		if (ch == '\n') {
			buffer[i++] = ' ';
		} else {
			buffer[i++] = ch;
		}
	}

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

	const char *first = (const char *) yytext + 1;
	char *last = (char *) yytext + strlen(yytext) - 1;
	*last = '\0';
	yylval.cpp_string = new std::string(first);
	return WORD;
}


[^ \t\n\|\>\<\&][^ \t\n\|\>\<\&]*  {
  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);	
  return WORD;
}
