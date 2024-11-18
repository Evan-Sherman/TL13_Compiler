#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct Program {
  struct Declaration *decs;
  struct StatementSequence *state_seq;
};

struct Declaration {
  // var
  const char *identifier;
  // as
  struct Type *type;
  //;
  struct Declaration *next_dec;
};

struct Type {
  bool is_int;
  const char *val;
};

struct StatementSequence {
  struct Statement *statement;
  //;
  struct StatementSequence *next_statements;
};

enum StatementType {
  ASSIGNMENT_STATEMENT,
  IF_STATEMENT,
  WHILE_STATEMENT,
  WRITEINT_STATEMENT
};

struct Statement {
  struct Assignment *assignment;
  struct IfStatement *if_statement;
  struct WhileStatement *while_statement;
  struct WriteInt *write_int;
  enum StatementType statement_type;
};

struct Assignment {
  const char *identifier;
  struct Expression *expression;
};

struct WriteInt {
  // writeInt
  struct Expression *expression;
};

struct IfStatement {
  // if
  struct Expression *expression;
  // then
  struct StatementSequence *state_seq;
  // else
  struct ElseClause *else_clause;
  // end
};

struct ElseClause {
  // else
  struct StatementSequence *statement_sequence;
};

struct WhileStatement {
  // while
  struct Expression *expression;
  // do
  struct StatementSequence *statement_sequence;
  // end
};

struct Expression {
  struct SimpleExpression *simple_expression1;
  struct SimpleExpression *simple_expression2;
  const char *op;
};

struct SimpleExpression {
  struct Term *term1;
  struct Term *term2;
  const char *op;
};

struct Term {
  struct Factor *factor1;
  struct Factor *factor2;
  const char *op;
};

enum FactorType { FAC_IDENTIFIER, FAC_NUMBER, FAC_BOOL, FAC_PAREN_EXP };
struct Factor {
  const char *val;
  enum FactorType type;
  struct Expression *exp;
};

void display_prog(struct Program *prog);
void display_dec(struct Declaration *dec);
void display_type(struct Type *type);
void display_statement_seq(struct StatementSequence *state_seq);
void display_statement(struct Statement *statement);
void display_assignment(struct Assignment *assign);
void display_writeint(struct WriteInt *writeint);
void display_if_statement(struct IfStatement *if_state);
void display_else_clause(struct ElseClause *else_clause);
void display_while_statement(struct WhileStatement *while_state);
void display_expression(struct Expression *exp);
void display_simple_expression(struct SimpleExpression *simp_exp);
void display_term(struct Term *term);
void display_factor(struct Factor *fac);

void to_c_file(struct Program *);
const char *c_style_dec(struct Declaration *);
const char *c_style_statement_seq(struct StatementSequence *);
const char *c_style_statement(struct Statement *);
const char *c_style_assignment(struct Assignment *);
const char *c_style_expression(struct Expression *);
const char *c_style_simple_expression(struct SimpleExpression *);
const char *c_style_term(struct Term *);
const char *c_style_factor(struct Factor *);
const char *c_style_if_statement(struct IfStatement *);
const char *c_style_else_clause(struct ElseClause *);
const char *c_style_writeint(struct WriteInt *);
const char *c_style_while_statement(struct WhileStatement *);

union YYSTYPE {
  const char *c_string;
  struct Program *program;
  struct Declaration *declaration;
  struct Type *type;
  struct StatementSequence *statement_sequence;
  struct Statement *statement;
  struct Assignment *assignment;
  struct IfStatement *if_statement;
  struct ElseClause *else_clause;
  struct WhileStatement *while_statement;
  struct WriteInt *write_int;
  struct Expression *expression;
  struct SimpleExpression *simple_expression;
  struct Term *term;
  struct Factor *factor;
};
