%option noyywrap
%{
/*****************声明和选项设置  begin*****************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexical_analyzer.h"


/*****************声明和选项设置  end*****************/

%}
digit [0-9]
letter [a-zA-Z]
ID {letter}+
INTEGER  {digit}+
FLOAT  ({digit}+\.|{digit}*\.{digit}+)
commentPattern "/*"([^\*]|(\*)*[^\*/])*(\*)*"*/"

%%

 /******************TODO*********************/
 /****请在此补全所有flex的模式与动作  start******/
 //STUDENT TO DO


[+] {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return ADD;}
\- {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return SUB;}
\* {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return MUL;}
\/ {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return DIV;}
\< {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return LT;}
\<= {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return LTE;}
\> {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return GT;}
\>= {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return GTE;}
== {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return EQ;}
!= {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return NEQ;}
= {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return ASSIN;}


; {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return SEMICOLON;}
, {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return COMMA;}
\( {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return LPARENTHESE;}
\) {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return RPARENTHESE;}
\[ {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return LBRACKET;}
\] {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return RBRACKET;}
\{ {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return LBRACE;}
\} {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return RBRACE;}


else {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return ELSE;}
if {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return IF;}
int {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return INT;}
float {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return FLOAT;}
return {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return RETURN;}
void {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return VOID;}
while {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return WHILE;}


{ID} {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return IDENTIFIER;}
{INTEGER} {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return INTEGER;}
{FLOAT} {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return FLOATPOINT;}
\[\] {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return ARRAY;}
[ \t] {pos_start=pos_end;pos_end=pos_start+strlen(yytext); return BLANK;}
[\n]  {lines++; pos_start=1; pos_end=1; return EOL;}
{commentPattern} { return COMMENT;} 





. {return ERROR;} 




 /****请在此补全所有flex的模式与动作  end******/
%%
/****************C代码 start*************/

/// \brief analysize a *.cminus file
///
///	\param input_file, 需要分析的文件路径
/// \param token stream, Token_Node结构体数组，用于存储分析结果，具体定义参考lexical_analyer.h

void analyzer(char* input_file, Token_Node* token_stream){
	if(!(yyin = fopen(input_file,"r"))){
		printf("[ERR] No input file\n");
		exit(1);
	}
	printf("[START]: Read from: %s\n", input_file);

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
	printf("[END]: Analysis completed.\n");
	return;
}



/****************C代码 end*************/
