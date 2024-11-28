%{
  #include <stdio.h>
  #include <stdlib.h>
  #include "final_includes.h"
  #include <string.h>
  int yylex(void);
  void yyerror(const char*);
  extern int yylineno;
  int num_decs = 0;
%}

%locations
%define api.value.type {union YYSTYPE}
%define parse.error detailed
%start prog
%token LP RP ASSIGN SC READINT WRITEINT NEWLINE
%token <c_string> IDENTIFIER OP2 OP3 OP4 NUMBER BOOL
%token IF_KEY THEN_KEY ELSE_KEY BEGIN_KEY END_KEY PROGRAM_KEY WHILE_KEY DO_KEY VAR_KEY AS_KEY INT_KEY BOOL_KEY 
%type <declaration> dec
%type <statement_sequence> statementSeq
%type <program> prog
%type <type> type
%type <statement> statement
%type <assignment> assignment
%type <if_statement> ifStatement
%type <else_clause> elseClause
%type <while_statement> whileStatement
%type <write_int> writeInt
%type <expression> expression
%type <simple_expression> simpleExpression
%type <term> term
%type <factor> factor

%%
prog:{
    $$ = NULL;
    } 
    | 
    PROGRAM_KEY dec BEGIN_KEY statementSeq END_KEY{
    struct Program* prog = malloc(sizeof(struct Program));
    prog->decs = $2;
    prog->state_seq = $4;
    to_c_file(prog);
    $$ = prog;
    }
    |
    error {
      YYABORT;
    }
    ;

dec:{
   $$ = NULL;
   } 
   |
   VAR_KEY IDENTIFIER AS_KEY type SC dec{
   num_decs++;
   struct Declaration* dec = malloc(sizeof(struct Declaration));
   dec->identifier = $2;
   dec->type = $4;
   dec->next_dec = $6;
   dec->line_num = yylineno - num_decs;
   $$ = dec;
   }
   ;

type: INT_KEY{
    struct Type* type = malloc(sizeof(struct Type));
    type->is_int = true;
    type->val = "int";
    $$ = type;
    }
    | BOOL_KEY{
    struct Type* type = malloc(sizeof(struct Type));
    type->is_int = false;
    type->val = "bool";
    $$ = type;
    }
    ;

statementSeq: {
            $$ = NULL;
            }
            |
            statement SC statementSeq{
            struct StatementSequence* state_seq = malloc(sizeof(struct StatementSequence));
            state_seq->statement = $1;
            state_seq->next_statements = $3;
            $$ = state_seq;
            }
            ;

statement: assignment{
         struct Statement* statement = malloc(sizeof(struct Statement));
         statement->assignment = $1;
         statement->statement_type = ASSIGNMENT_STATEMENT;
         $$ = statement;
         } 
         | ifStatement{
         struct Statement* statement = malloc(sizeof(struct Statement));
         statement->if_statement = $1;
         statement->statement_type = IF_STATEMENT;
         $$ = statement;
         }
         | whileStatement{
         struct Statement* statement = malloc(sizeof(struct Statement));
         statement->while_statement = $1;
         statement->statement_type = WHILE_STATEMENT;
         $$ = statement;
         }
         | writeInt{
         struct Statement* statement = malloc(sizeof(struct Statement));
         statement->write_int = $1;
         statement->statement_type = WRITEINT_STATEMENT;
         $$ = statement;
         }
         ;

assignment: IDENTIFIER ASSIGN expression{
          struct Assignment* assign = malloc(sizeof(struct Assignment));
          assign->identifier = $1;
          assign->expression = $3;
          $$ = assign;
          }
          | IDENTIFIER ASSIGN READINT{
          struct Assignment* assign = malloc(sizeof(struct Assignment));
          assign->identifier = $1;
          $$ = assign;
          }
          ;

writeInt: WRITEINT expression{
        struct WriteInt* write_int = malloc(sizeof(struct WriteInt));
        write_int->expression = $2;
        $$ = write_int;
        }
        ;

ifStatement: IF_KEY expression THEN_KEY statementSeq elseClause END_KEY{
           struct IfStatement* if_state = malloc(sizeof(struct IfStatement));
           if_state->expression = $2;
           if_state->state_seq = $4;
           if_state->else_clause = $5;
           $$ = if_state;
           }
          ;

elseClause:{
          $$ = NULL;
          } 
          |
          ELSE_KEY statementSeq{
          struct ElseClause* else_clause = malloc(sizeof(struct ElseClause));
          else_clause->statement_sequence = $2;
          $$ = else_clause;
          }
          ;

whileStatement: WHILE_KEY expression DO_KEY statementSeq END_KEY{
              struct WhileStatement* while_state = malloc(sizeof(struct WhileStatement));
              while_state->expression = $2;
              while_state->statement_sequence = $4;
              $$ = while_state;
              }
              ;

expression: simpleExpression{
          struct Expression* exp = malloc(sizeof(struct Expression));
          exp->simple_expression1 = $1;
          $$ = exp;
          }
          | simpleExpression OP4 simpleExpression{
          struct Expression* exp = malloc(sizeof(struct Expression));
          exp->simple_expression1 = $1;
          exp->op = $2;
          if(strcmp(exp->op, "=") == 0){
            exp->op = "==";
          }
          exp->simple_expression2 = $3;
          $$ = exp;
          }
          ;

simpleExpression: term OP3 term{
                struct SimpleExpression* simp_exp = malloc(sizeof(struct SimpleExpression));
                simp_exp->term1 = $1;
                simp_exp->op = $2;
                simp_exp->term2 = $3;
                $$ = simp_exp;
                }
                | term{
                struct SimpleExpression* simp_exp = malloc(sizeof(struct SimpleExpression));
                simp_exp->term1 = $1;
                $$ = simp_exp;
                }
                ;

term: factor OP2 factor{
    struct Term* term = malloc(sizeof(struct Term));
    term->factor1 = $1;
    term->op = $2;
    if(strcmp(term->op, "div") == 0){
      term->op = "/";
    }
    if(strcmp(term->op, "mod") == 0){
      term->op = "%";
    }
    term->factor2 = $3;
    $$ = term;
    }
    | factor{
    struct Term* term = malloc(sizeof(struct Term));
    term->factor1 = $1;
    $$ = term;
    }
    ;

factor: IDENTIFIER{
      struct Factor* fac = malloc(sizeof(struct Factor));
      fac->val = $1;
      fac->type = FAC_IDENTIFIER;
      $$ = fac;
      }
      | NUMBER{
      struct Factor* fac = malloc(sizeof(struct Factor));
      fac->val = $1;
      fac->type = FAC_NUMBER;
      $$ = fac;
      }
      | BOOL{
      struct Factor* fac = malloc(sizeof(struct Factor));
      fac->val = $1;
      fac->type = FAC_BOOL;
      $$ = fac;
      }
      | LP expression RP{
      struct Factor* fac = malloc(sizeof(struct Factor));
      fac->type = FAC_PAREN_EXP;
      fac->exp = $2;
      $$ = fac;
      }
%%

int main(){
int res = yyparse();

return(res);
}

void yyerror(const char* s){
  fprintf(stderr, RED "%s\n" RESET, s);
}

int yywrap() {
return 1;
}
