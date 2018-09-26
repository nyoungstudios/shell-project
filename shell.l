
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

"." {
	return LESS;
}


[^ \t\n][^ \t\n]*  {
  /* Assume that file names have only alpha chars */

	//printf("%s\n", yytext);

	const char *first = (const char *) yytext;
	const char *last = (const char *) yytext + strlen(yytext) - 1;

	if (!strcmp(first,"\"") && !strcmp(last,"\"")) {
		last = "\0";
		yylval.cpp_string = new std::string(first);
		printf("%s\n", first);	
	} else {
	  yylval.cpp_string = new std::string(yytext);
	}
	
  return WORD;
}
