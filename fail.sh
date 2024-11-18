flex final.lex.l;
bison -d final.y;
gcc final.tab.c lex.yy.c final_includes.c -o parser;
./parser < fail_input.txt;
