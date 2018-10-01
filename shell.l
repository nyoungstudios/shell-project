
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

[\$\{][^\n]*[\}] {
	const char * first = (const char *) yytext + 2;
	char *last = (char *) yytext + strlen(yytext) - 1;
	*last = '\0';
	printf("%s\n", *first);
	//printf("%c\n", *last);

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
