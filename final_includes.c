#include "final_includes.h"
#include <stdio.h>
#include <string.h>

void display_prog(struct Program *prog) {
  printf("program\n");
  if (prog->decs != NULL) {
    display_dec(prog->decs);
  }
  printf("begin\n");
  if (prog->state_seq != NULL) {
    display_statement_seq(prog->state_seq);
  }
  printf("end\n");
}

void display_dec(struct Declaration *dec) {
  printf("var %s as ", dec->identifier);
  display_type(dec->type);
  printf(";\n");
  if (dec->next_dec != NULL) {
    display_dec(dec->next_dec);
  }
}

void display_type(struct Type *type) { printf("%s", type->val); }

void display_statement_seq(struct StatementSequence *state_seq) {
  display_statement(state_seq->statement);
  printf(";\n");
  if (state_seq->next_statements != NULL) {

    display_statement_seq(state_seq->next_statements);
  }
}

void display_statement(struct Statement *statement) {
  switch (statement->statement_type) {
  case ASSIGNMENT_STATEMENT:
    display_assignment(statement->assignment);
    break;
  case IF_STATEMENT:
    display_if_statement(statement->if_statement);
    break;
  case WHILE_STATEMENT:
    display_while_statement(statement->while_statement);
    break;
  case WRITEINT_STATEMENT:
    display_writeint(statement->write_int);
    break;
  default:
    break;
  }
}

void display_assignment(struct Assignment *assign) {
  printf("%s := ", assign->identifier);
  if (assign->expression != NULL) {
    display_expression(assign->expression);
  } else {
    printf("readInt");
  }
}

void display_writeint(struct WriteInt *writeint) {
  printf("writeInt ");
  display_expression(writeint->expression);
}

void display_if_statement(struct IfStatement *if_state) {
  printf("if ");
  display_expression(if_state->expression);
  printf(" then\n");
  display_statement_seq(if_state->state_seq);
  if (if_state->else_clause != NULL) {
    display_else_clause(if_state->else_clause);
  }
  printf("end");
}

void display_else_clause(struct ElseClause *else_clause) {
  printf("else\n");
  display_statement_seq(else_clause->statement_sequence);
}

void display_while_statement(struct WhileStatement *while_state) {
  printf("while ");
  display_expression(while_state->expression);
  printf("do\n");
  display_statement_seq(while_state->statement_sequence);
  printf("end");
}

void display_expression(struct Expression *exp) {
  display_simple_expression(exp->simple_expression1);
  if (exp->simple_expression2 != NULL) {
    printf(" %s ", exp->op);
    display_simple_expression(exp->simple_expression2);
  }
}

void display_simple_expression(struct SimpleExpression *simp_exp) {
  display_term(simp_exp->term1);
  if (simp_exp->term2 != NULL) {
    printf(" %s ", simp_exp->op);
    display_term(simp_exp->term2);
  }
}

void display_term(struct Term *term) {
  display_factor(term->factor1);
  if (term->factor2 != NULL) {
    printf(" %s ", term->op);
    display_factor(term->factor2);
  }
}

void display_factor(struct Factor *factor) {
  if (factor->type == FAC_PAREN_EXP) {
    printf("(");
    display_expression(factor->exp);
    printf(")");
  } else {
    printf(" %s ", factor->val);
  }
}

void to_c_file(struct Program *prog) {
  FILE *file = fopen("TL13.c", "w");
  fprintf(file, "#include <stdbool.h>\n #include <stdio.h>\n\n\n");
  fprintf(file, "int main() {\n%s\n\n%s\n\nreturn 0;\n}\n",
          c_style_dec(prog->decs), c_style_statement_seq(prog->state_seq));

  fclose(file);
}

const char *c_style_dec(struct Declaration *dec) {
  const char *type;
  if (dec->type->is_int) {
    type = "int";
  } else {
    type = "bool";
  }
  const char *next_dec_str = "";
  if (dec->next_dec != NULL) {
    next_dec_str = c_style_dec(dec->next_dec);
  }
  int buf_size = strlen(type) + 1 + strlen(next_dec_str) + 1 +
                 strlen(dec->identifier) + 1 + 4;

  char *ret = malloc(buf_size);
  snprintf(ret, buf_size, "%s %s;\n%s", type, dec->identifier, next_dec_str);
  return ret;
}

const char *c_style_statement_seq(struct StatementSequence *state_seq) {
  const char *state = c_style_statement(state_seq->statement);
  if (state_seq->next_statements != NULL) {
    const char *next_state_seq_str =
        c_style_statement_seq(state_seq->next_statements);
    int buf_size = strlen(state) + strlen(next_state_seq_str) + 3;
    char *ret = malloc(buf_size);
    snprintf(ret, buf_size, "%s%s", state, next_state_seq_str);
    return ret;
  } else {
    char *ret = malloc(strlen(state) + 2);
    snprintf(ret, strlen(state) + 2, "%s", state);
    return ret;
  }
}

const char *c_style_statement(struct Statement *statement) {
  switch (statement->statement_type) {
  case ASSIGNMENT_STATEMENT:
    return c_style_assignment(statement->assignment);
  case IF_STATEMENT:
    return c_style_if_statement(statement->if_statement);
  case WHILE_STATEMENT:
    return c_style_while_statement(statement->while_statement);
  case WRITEINT_STATEMENT:
    return c_style_writeint(statement->write_int);
  default:
    printf("REACHED UNREACHABLE STATEMENT\n");
  }
}

const char *c_style_assignment(struct Assignment *assign) {
  if (assign->expression != NULL) {
    const char *exp_str = c_style_expression(assign->expression);
    int buf_size = strlen(assign->identifier) + 1 + strlen(exp_str) + 1 + 4;
    char *ret = malloc(buf_size);
    snprintf(ret, buf_size, "%s = %s;\n", assign->identifier, exp_str);
    return ret;

  } else {

    int buf_size =
        strlen("scanf(\"%d\"&,);") + 1 + strlen(assign->identifier) + 1 + 1;
    char *ret = malloc(buf_size);
    snprintf(ret, buf_size, "scanf(\"%%d\",&%s);\n", assign->identifier);
    return ret;
  }
}

const char *c_style_expression(struct Expression *exp) {
  const char *simp_exp1 = c_style_simple_expression(exp->simple_expression1);
  if (exp->simple_expression2 != NULL) {
    const char *simp_exp2 = c_style_simple_expression(exp->simple_expression2);
    int buf_size = strlen(simp_exp1) + strlen(simp_exp2) + 4;
    char *ret = malloc(buf_size);
    snprintf(ret, buf_size, "%s %s %s", simp_exp1, exp->op, simp_exp2);
    return ret;
  } else {
    return simp_exp1;
  }
}

const char *c_style_simple_expression(struct SimpleExpression *simp_exp) {
  const char *term1 = c_style_term(simp_exp->term1);
  if (simp_exp->term2 != NULL) {
    const char *term2 = c_style_term(simp_exp->term2);
    int buf_size = strlen(term1) + strlen(term2) + 4;
    char *ret = malloc(buf_size);
    snprintf(ret, buf_size, "%s %s %s", term1, simp_exp->op, term2);
    return ret;
  } else {
    return term1;
  }
}

const char *c_style_term(struct Term *term) {

  const char *fac1 = c_style_factor(term->factor1);
  if (term->factor2 != NULL) {
    const char *fac2 = c_style_factor(term->factor2);
    int buf_size = strlen(fac1) + strlen(fac2) + 4;
    char *ret = malloc(buf_size);
    snprintf(ret, buf_size, "%s %s %s", fac1, term->op, fac2);
    return ret;
  } else {
    return fac1;
  }
}

const char *c_style_factor(struct Factor *fac) {
  const char *ret;
  switch (fac->type) {
  case FAC_PAREN_EXP:
    const char *exp = c_style_expression(fac->exp);
    char *ret = malloc(strlen(exp) + 3);
    snprintf(ret, strlen(exp) + 3, "(%s)", exp);
    return ret;
  default:
    return fac->val;
  }
}

const char *c_style_if_statement(struct IfStatement *if_state) {
  const char *else_clause_str =
      if_state->else_clause == NULL
          ? "\n"
          : c_style_else_clause(if_state->else_clause);

  const char *exp_str = c_style_expression(if_state->expression);
  const char *state_str = c_style_statement_seq(if_state->state_seq);
  int buf_size = strlen("if(){\n\n}") + 1 + strlen(else_clause_str) + 1 +
                 strlen(exp_str) + 1 + strlen(state_str) + 1 + 2;
  char *ret = malloc(buf_size);
  snprintf(ret, buf_size, "if(%s){\n%s}%s", exp_str, state_str,
           else_clause_str);
  return ret;
}

const char *c_style_else_clause(struct ElseClause *else_clause) {
  const char *state_seq_str =
      c_style_statement_seq(else_clause->statement_sequence);
  int buf_size = strlen(state_seq_str) + 1 + strlen("else{\n\n}\n") + 1 + 1;
  char *ret = malloc(buf_size);
  snprintf(ret, buf_size, "else{\n%s\n}\n", state_seq_str);
  return ret;
}

const char *c_style_while_statement(struct WhileStatement *while_state) {
  const char *state_seq_str =
      c_style_statement_seq(while_state->statement_sequence);
  const char *exp_str = c_style_expression(while_state->expression);
  int buf_size =
      strlen(state_seq_str) + 1 + strlen(exp_str) + 1 + strlen("while(){\n}\n");
  char *ret = malloc(buf_size);
  snprintf(ret, buf_size, "while(%s){\n%s}\n", exp_str, state_seq_str);
  return ret;
}

const char *c_style_writeint(struct WriteInt *write_int) {
  const char *exp_str = c_style_expression(write_int->expression);
  int buf_size = strlen(exp_str) + 1 + strlen("printf(\"%d\", );") + 1 + 1;
  char *ret = malloc(buf_size);
  snprintf(ret, buf_size, "printf(\"%%d\", %s);", exp_str);
  return ret;
}

// TODO: IMPLEMENT ERROR HANDLING
// TODO: IMPLEMENT ERROR HANDLING
// TODO: IMPLEMENT ERROR HANDLING
// TODO: IMPLEMENT ERROR HANDLING
// TODO: IMPLEMENT ERROR HANDLING
// TODO: IMPLEMENT ERROR HANDLING
// TODO: IMPLEMENT ERROR HANDLING
// TODO: IMPLEMENT ERROR HANDLING
// TODO: IMPLEMENT ERROR HANDLING
