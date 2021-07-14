%option noyywrap
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syntax_tree.h"
#include "syntax_analyzer.h"

int files_count;
int lines;
int pos_start;
int pos_end;

void pass_node(char *text){
     yylval.node = new_syntax_tree_node(text);
}
%}

/***************
 TO STUDENTS: Copy your Lab1 here. Make adjustments if necessary.

 Note: don't modify the prologue unless you know what you are doing.
***************/

/* Example for you :-) */
digit [0-9]
letter [a-zA-Z]
ID {letter}+
INTEGER  {digit}+
FLOAT  ({digit}+\.|{digit}*\.{digit}+)
commentPattern "/*"([^\*]|(\*)*[^\*/])*(\*)*"*/"

%%

\+ { pos_start = pos_end; pos_end += 1; pass_node(yytext); return ADD; }
\- {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return SUB;}
\* {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return MUL;}
\/ {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return DIV;}
\< {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return LT;}
\<= {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return LTE;}
\> {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return GT;}
\>= {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return GTE;}
== {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return EQ;}
!= {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return NEQ;}
= {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return ASSIN;}


; {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return SEMICOLON;}
, {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return COMMA;}
\( {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return LPARENTHESE;}
\) {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return RPARENTHESE;}
\[ {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return LBRACKET;}
\] {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return RBRACKET;}
\{ {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return LBRACE;}
\} {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return RBRACE;}


else {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return ELSE;}
if {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return IF;}
int {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return INT;}
float {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return FLOAT;}
return {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return RETURN;}
void {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return VOID;}
while {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return WHILE;}


{ID} {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return IDENTIFIER;}
{INTEGER} {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return INTEGER;}
{FLOAT} {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return FLOATPOINT;}
\[\] {pos_start=pos_end;pos_end=pos_start+strlen(yytext); pass_node(yytext);return ARRAY;}
[ \t] {pos_start=pos_end;pos_end=pos_start+strlen(yytext);}
[\n]  {lines++; pos_start=1; pos_end=1; }
{commentPattern} {} 



%%


/*void analyzer(char* input_file, Token_Node* token_stream){
	if(!(yyin = fopen(input_file,"r"))){
		printf("[ERR] No input file\n");
		exit(1);
	}
	//printf("[START]: Read from: %s\n", input_file);

	int token;
	int index = 0;
	char temp[256];
	int i;

	while(token = yylex()){
		switch(token){
			case COMMENT: 
				//STUDENT TO DO
				strcpy(temp,yytext);
				for(i=0;temp[i]!='\0';i++)
				{
					if(temp[i]=='\n')
					{
						lines++;
						pos_end=1;							
						pos_start=1;
					}
					else
					{
						pos_end++;
					}
						
				}
				break;
			case BLANK:
				//STUDENT TO DO	
				break;
			case EOL:
				//STUDENT TO DO
				break;
			case ERROR:
				printf("[ERR]: unable to analysize %s at %d line, from %d to %d\n", yytext, lines, pos_start, pos_end);
			default :
				if (token == ERROR){
					sprintf(token_stream[index].text, "[ERR]: unable to analysize %s at %d line, from %d to %d", yytext, lines, pos_start, pos_end);
				} else {
					strcpy(token_stream[index].text, yytext);
				}
				token_stream[index].token = token;
				token_stream[index].lines = lines;
				token_stream[index].pos_start = pos_start;
				token_stream[index].pos_end = pos_end;
				index++;
				if (index >= MAX_NUM_TOKEN_NODE){
					printf("%d has too many tokens (> %d)", input_file, MAX_NUM_TOKEN_NODE);
					exit(1);
				}
		}
	}
	//printf("[END]: Analysis completed.\n");
	return;
}*/
