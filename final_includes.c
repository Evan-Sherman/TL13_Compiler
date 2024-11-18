#include "final_includes.h"
#include <stdio.h>
#include <string.h>

// these are parallel arrays
// should always be same length and coordinated
const char **dec_arr;
bool *is_int_arr;
int dec_len;
int is_int_len;
const char *error_msg = NULL;

void add_dec(const char *str) {
  const char **new_arr = malloc(sizeof(char *) * (dec_len + 1));
  for (int i = 0; i < dec_len; i++) {
    new_arr[i] = dec_arr[i];
  }
  new_arr[dec_len] = str;
  dec_len++;
  dec_arr = new_arr;
}

bool is_in_dec(const char *str) {
  for (int i = 0; i < dec_len; i++) {
    if (strcmp(str, dec_arr[i]) == 0) {
      return true;
    }
  }
  return false;
}

bool is_dec_int(const char *str) {
  for (int i = 0; i < dec_len; i++) {
    if (strcmp(str, dec_arr[i]) == 0) {
      return is_int_arr[i];
    }
  }
  // should not reach, caller should validate
  return false;
}

void validate_ident(const char *ident) {
  if (!is_in_dec(ident)) {
    int buf_size = strlen("Undeclared identifier '' encountered\n") + 1 +
                   strlen(ident) + 1 + 7;
    error_msg = malloc(buf_size);
    snprintf((char *)error_msg, buf_size,
             RED "Undeclared identifier '%s' encountered" RESET, ident);
    terminate();
  } else {
    return;
  }
}

void validate_type_of_ident(const char *ident, bool is_int) {
  if (is_dec_int(ident) != is_int) {
    const char *type_str = !is_int ? "int" : "boolean";
    int buf_size = strlen("Mismatched types. '' was declared as a \n") + 1 +
                   strlen(ident) + 1 + strlen(type_str) + 1 + 6;
    error_msg = malloc(buf_size);
    snprintf((char *)error_msg, buf_size,
             RED "Mismatched types. '%s' was declared as a %s\n" RESET, ident,
             type_str);
    terminate();
  } else {
    return;
  }
}

void check_duplicate_dec(const char *str) {
  if (is_in_dec(str)) {
    int buf_size = strlen("Duplicate Declaration. '' is already declared\n") +
                   1 + strlen(str) + 1 + 6;
    error_msg = malloc(buf_size);
    snprintf((char *)error_msg, buf_size,
             RED "Duplicate declaration. '%s' is already declared\n", str);
    terminate();
  }
}

void sigterm_handler(int sig) {
  if (error_msg == NULL) {
    printf("\nKill called. exiting...\n");
    exit(0);
  } else {
    printf("\n%s\n", error_msg);
    exit(0);
  }
}

void terminate() {
  pid_t pid = getpid();
  kill(pid, SIGTERM);
}

void add_is_int(bool is_int) {
  bool *new_arr = malloc(sizeof(bool) * (is_int_len + 1));
  for (int i = 0; i < is_int_len; i++) {
    new_arr[i] = is_int_arr[i];
  }
  new_arr[is_int_len] = is_int;
  is_int_len++;
  is_int_arr = new_arr;
}

void to_c_file(struct Program *prog) {
  FILE *file = fopen("TL13.c", "w");
  fprintf(file, "#include <stdbool.h>\n #include <stdio.h>\n\n\n");
  const char *decs = c_style_dec(prog->decs);
  const char *states = c_style_statement_seq(prog->state_seq);
  fprintf(file, "int main() {\n%s\n\n%s\n\nreturn 0;\n}\n", decs, states);
  fclose(file);
  printf(GREEN "\nSuccessfully compiled into TL13.c\n" RESET);
}

const char *c_style_dec(struct Declaration *dec) {
  const char *type;
  check_duplicate_dec(dec->identifier);
  add_dec(dec->identifier);
  add_is_int(dec->type->is_int);
  const char *default_val;
  if (dec->type->is_int) {
    type = "int";
    default_val = "0";
  } else {
    type = "bool";
    default_val = "false";
  }
  const char *next_dec_str = "";
  if (dec->next_dec != NULL) {
    next_dec_str = c_style_dec(dec->next_dec);
  }
  int buf_size = strlen(type) + 1 + strlen(next_dec_str) + 1 +
                 strlen(dec->identifier) + 1 + strlen(default_val) + 1 + 7;
  char *ret = malloc(buf_size);
  snprintf(ret, buf_size, "%s %s = %s;\n%s", type, dec->identifier, default_val,
           next_dec_str);
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
  validate_ident(assign->identifier);
  if (assign->expression != NULL) {
    validate_type_of_ident(
        assign->identifier,
        assign->expression->simple_expression2 ==
            NULL); // expressions always use a boolean operator if there's 2
    const char *exp_str = c_style_expression(assign->expression);
    int buf_size = strlen(assign->identifier) + 1 + strlen(exp_str) + 1 + 4;
    char *ret = malloc(buf_size);
    snprintf(ret, buf_size, "%s = %s;\n", assign->identifier, exp_str);
    return ret;

  } else {
    validate_type_of_ident(assign->identifier, true);
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
    int buf_size = strlen(simp_exp1) + strlen(simp_exp2) + strlen(exp->op) + 4;
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
  case FAC_IDENTIFIER:
    validate_ident(fac->val);
    return fac->val;
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
  snprintf(ret, buf_size, "printf(\"%%d\\n\", %s);", exp_str);
  return ret;
}
