#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define RED "\033[31m"
#define RESET "\033[0m"
#define GREEN "\033[32m"

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
  int line_num;
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

int get_dec_index(const char *);
bool is_dec_int(const char *);
void add_dec(struct Declaration *dec);
void validate_ident(const char *);
void validate_type_of_ident(const char *, bool);
void check_duplicate_dec(const char *);
void terminate();

void to_c_file(struct Program *);
bool is_exp_int(struct Expression *);
bool is_simp_exp_int(struct SimpleExpression *);
bool is_term_int(struct Term *);
bool is_fac_int(struct Factor *);

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
