#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <string.h>

enum {
  TK_NOTYPE = 256, 
  TK_ADD,
  TK_SUB,
  TK_MUL,
  TK_DIV,
  TK_EQ,
  TK_NUM,
  TK_LPARENT,
  TK_RPARENT

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +",  TK_NOTYPE},           // spaces
  {"\\+", TK_ADD},              // add
  {"-",   TK_SUB},              // sub
  {"\\*", TK_MUL},              // mul
  {"/",   TK_DIV},              // div
  {"==",  TK_EQ},               // equal
  {"[1-9][0-9]*|0",  TK_NUM},   // number
  {"\\(",   TK_LPARENT},          // left parenthese
  {"\\)",   TK_RPARENT},          // right parenthese

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  Assert(e, "要解析的字符串为空");

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        int tk_type = rules[i].token_type;

        if (substr_len > 32 || (tk_type != TK_NOTYPE && nr_token >= 32)) {
          printf("运算符过长或过多\n");
          return false;
        }

        switch (tk_type) {
          case TK_NOTYPE: break;
          case TK_ADD: case TK_SUB: 
          case TK_MUL: case TK_DIV: tokens[nr_token++].type = tk_type; break;
          case TK_NUM: {
            tokens[nr_token].type = tk_type;
            memcpy(tokens[nr_token].str, substr_start, substr_len);
            nr_token++;
            } break;
          default: printf("unknown token type\n"); return false;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
