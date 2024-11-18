%{
  #include "final.tab.h"
  #include "final_includes.h"
  int tokenNum;
  char* token;
%}

%%
[1-9][0-9]*|0 {
yylval.c_string = strdup(yytext);
return(NUMBER);
}

false|true {
yylval.c_string = strdup(yytext);
return(BOOL);
}

[A-Z][A-Z0-9]* {

yylval.c_string = strdup(yytext);
return(IDENTIFIER);
}

"(" {
return(LP);
}

")" {
return(RP);
}

":=" {
return(ASSIGN);
}

";" {
return(SC);
}

"*"|"div"|"mod" {
yylval.c_string = strdup(yytext);
return(OP2);
}

"+"|"-" {
yylval.c_string = strdup(yytext);
return(OP3);
}

"="|"!="|"<"|">"|"<="|">=" {
yylval.c_string = strdup(yytext);
return(OP4);
}

"if" {
return(IF_KEY);
}

"then" {
return(THEN_KEY);
}

"else" {
return(ELSE_KEY);
}

"begin" {
return(BEGIN_KEY);
}

"end" {
return(END_KEY);
}

"while" {
return(WHILE_KEY);
}

"do" {
return(DO_KEY);
}

"program" {
return(PROGRAM_KEY);
}

"var" {
return(VAR_KEY);
}

"as" {
return(AS_KEY);
}

"int" {
return(INT_KEY);
}

"bool" {
return(BOOL_KEY);
}


"writeInt" {
return(WRITEINT);
}

"readInt" {
return(READINT);
}

"  "|" " {;}

\n {;}

