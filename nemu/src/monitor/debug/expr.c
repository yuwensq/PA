#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>

#define use_bit(x) (1 << (x))

enum
{
  // 独热码，方便用，但是最多支持32种Token
  TK_NOTYPE = use_bit(0),
  TK_ADD = use_bit(1),
  TK_SUB = use_bit(2),
  TK_MUL = use_bit(3),
  TK_DIV = use_bit(4),
  TK_EQ = use_bit(5),
  TK_NEQ = use_bit(6),
  TK_AND = use_bit(7),
  TK_NUM = use_bit(8),
  TK_HEX_NUM = use_bit(9),
  TK_LPARENT = use_bit(10),
  TK_RPARENT = use_bit(11),
  TK_REG = use_bit(12),
  TK_DEREF = use_bit(13),
  TK_MINUS = use_bit(14),
  TK_NOT = use_bit(15),
  TK_OR = use_bit(16),

  /* TODO: Add more token types */

};

static struct rule
{
  char *regex;
  int token_type;
} rules[] = {

    /* TODO: Add more rules.
     * Pay attention to the precedence level of different rules.
     */

    {" +", TK_NOTYPE},         // spaces
    {"\\+", TK_ADD},           // add
    {"-", TK_SUB},             // sub
    {"\\*", TK_MUL},           // mul
    {"/", TK_DIV},             // div
    {"==", TK_EQ},             // equal
    {"!=", TK_NEQ},            // not equal
    {"!", TK_NOT},            // not
    {"&&", TK_AND},            // and
    {"\\|\\|", TK_OR},            // or
    {"0[xX][0-9A-Fa-f]+", TK_HEX_NUM}, // hexi number
    {"[1-9][0-9]*|0", TK_NUM}, // number
    {"\\(", TK_LPARENT},       // left parenthese
    {"\\)", TK_RPARENT},       // right parenthese
    {"\\$eax|\\$ebx|\\$ecx|\\$edx|\\$esi|\\$edi|\\$esp|\\$ebp|\\$pc", TK_REG},  // register

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++)
  {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0)
    {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token
{
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e)
{
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  memset(tokens, 0, sizeof(tokens));

  Assert(e, "要解析的字符串为空");

  while (e[position] != '\0')
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
      {
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

        if (substr_len > 32 || (tk_type != TK_NOTYPE && nr_token >= 32))
        {
          printf("运算符过长或过多\n");
          return false;
        }

        switch (tk_type)
        {
        case TK_NOTYPE:
          break;
        case TK_ADD:
        case TK_DIV:
        case TK_EQ:
        case TK_NEQ:
        case TK_AND:
        case TK_NOT:
        case TK_OR:
        case TK_LPARENT:
        case TK_RPARENT:
          tokens[nr_token++].type = tk_type;
          break;
        case TK_SUB:
        case TK_MUL:
        {
          if (nr_token == 0 || (tokens[nr_token - 1].type & (TK_HEX_NUM | TK_NUM | TK_REG | TK_RPARENT)) == 0) 
            tk_type = (tk_type == TK_MUL ? TK_DEREF : TK_MINUS);
          tokens[nr_token++].type = tk_type;
        }
          break;
        case TK_REG:
        case TK_NUM:
        case TK_HEX_NUM:
        {
          tokens[nr_token].type = tk_type;
          memcpy(tokens[nr_token].str, substr_start, substr_len);
          nr_token++;
        }
        break;
        default:
          printf("unknown token type\n");
          return false;
        }
        break;
      }
    }

    if (i == NR_REGEX)
    {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  // i = 0;
  // for (; i < nr_token; i++) {
  //   printf("%d ", tokens[i].type);
  // }
  // printf("\n");
  return true;
}

static bool bracket_check(int l, int r) {
  if (l > r) {
    Log("左边界大于右边界，请检查输入\n");
  }
  int nr_lparen = 0;
  int i = l;
  for (; i <= r; i++) {
    if (tokens[i].type == TK_LPARENT)
      nr_lparen++;
    else if (tokens[i].type == TK_RPARENT) {
      nr_lparen--;
      if (nr_lparen < 0) 
        return false;
    }
  }
  if (nr_lparen)
    return false;
  return true;
}

static bool expr_surrounded_by_paren(int l, int r) {
  if (tokens[l].type != TK_LPARENT || tokens[r].type != TK_RPARENT)
    return false;
  return bracket_check(l + 1, r - 1);
}

uint32_t isa_reg_str2val(const char *s, bool *success);

bool op_is_lower_than(int op_a, int op_b) {
  // 判断运算符a运算顺序是否低于b
  // b可以看作已经记录的最后算的运算符，a是当前遍历到的运算符，返回true表示更新最后运算符
  if (op_b & (TK_DEREF | TK_MINUS | TK_NOT)) {
    if (op_a & (TK_DEREF | TK_MINUS | TK_NOT))
      return false;
    else 
      return true;
  }
  else if (op_b & (TK_MUL | TK_DIV)) {
    if (op_a & (TK_OR | TK_AND | TK_NEQ | TK_EQ | TK_ADD | TK_SUB | TK_MUL | TK_DIV))
      return true;
    else
      return false;
  }
  else if (op_b & (TK_ADD | TK_SUB)) {
    if (op_a & (TK_OR | TK_AND | TK_NEQ | TK_EQ | TK_ADD | TK_SUB))
      return true;
    else 
      return false;
  }
  else if (op_b & (TK_EQ | TK_NEQ)) {
    if (op_a & (TK_OR | TK_AND | TK_NEQ | TK_EQ))
      return true;
    else 
      return false;
  }
  else if (op_b & TK_AND) {
    if (op_a & (TK_AND | TK_OR))
      return true;
    else 
      return false;
  }
  else if (op_b & TK_OR) {
    if (op_a & TK_OR)
      return true;
    else 
      return false;
  }
  else {
    Assert(false, "未知运算符%d", op_b);
  }
  return true;
}

static int32_t eval(int l, int r, bool *success)
{
  if (*success == false)
    return 0;
  if (l > r)
  {
    Log("左边界大于右边界，错误\n");
    *success = false;
    return 0;
  }
  else if (l == r)
  {
    switch (tokens[l].type)
    {
    case TK_NUM:
      return strtol(tokens[l].str, NULL, 10);
      break;
    case TK_HEX_NUM:
      return strtol(tokens[l].str, NULL, 16);
      break;
    case TK_REG:
      return isa_reg_str2val(tokens[l].str, success);
    default: 
    {
      Log("token应该为数字或寄存器\n");
      *success = false;
    }
      break;
    }
    return 0; 
  }
  else if (expr_surrounded_by_paren(l, r))
  {
    return eval(l + 1, r - 1, success);
  }
  else
  {
    int i = 0;
    int now_tk_type = 0;
    int nr_lparen = 0;
    int min_op_type = 0;
    int min_op_pos = -1;
    for (i = l; i <= r; i++) {
      now_tk_type = tokens[i].type;
      if (now_tk_type == TK_LPARENT)
        nr_lparen++;
      else if (now_tk_type == TK_RPARENT)
        nr_lparen--;
      else if (nr_lparen == 0) { // 只有在括号外才判断
        if ((now_tk_type & (TK_NUM | TK_HEX_NUM | TK_REG)) != 0) 
          continue;
        if (min_op_pos == -1 || op_is_lower_than(now_tk_type, min_op_type)) {
          min_op_pos = i;
          min_op_type = now_tk_type;
        }
      }
    }
    int32_t sub_res1 = 0;
    int32_t sub_res2 = eval(min_op_pos + 1, r, success);
    if (!(min_op_type & (TK_DEREF | TK_MINUS | TK_NOT)))
      sub_res1 = eval(l, min_op_pos - 1, success);
    switch (min_op_type)
    {
    case TK_DEREF:
      return vaddr_read(sub_res2, 4);
    case TK_MINUS:
      return -sub_res2;
    case TK_NOT:
      return !sub_res2;
    case TK_ADD: //printf("%u %c %u\n", sub_res1, '+', sub_res2); 
      return sub_res1 + sub_res2;
    case TK_SUB: //printf("%u %c %u\n", sub_res1, '-', sub_res2);
      return sub_res1 - sub_res2;
    case TK_MUL: //printf("%u %c %u\n", sub_res1, '*', sub_res2);
      return sub_res1 * sub_res2;
    case TK_EQ:
      return (sub_res1 == sub_res2);
    case TK_NEQ:
      return (sub_res1 != sub_res2);
    case TK_AND:
      return (sub_res1 && sub_res2);
    case TK_OR:
      return (sub_res1 || sub_res2);
    case TK_DIV: //printf("%u %c %u\n", sub_res1, '/', sub_res2);
      if (sub_res2) return sub_res1 / sub_res2;
    default:
      *success = false;
      break;
    }
  }
  return 0;
}

static uint32_t evalute(int nm_token, bool *success)
{
  *success = true;
  // 括号匹配提前，很显然，只用遍历一次就行，没必要添加到函数中
  *success = bracket_check(0, nm_token - 1);
  if (*success == false)
  {
    Log("表达式括号不匹配\n");
    return 0;
  }
  return (unsigned)eval(0, nm_token - 1, success);
}

uint32_t expr(char *e, bool *success)
{
  if (!make_token(e))
  {
    Log("make token失败\n");
    if (success)
      *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  uint32_t res = 0;
  if (success)
    res = evalute(nr_token, success);
  else {
    bool succ = false;
    res = evalute(nr_token, &succ);
  }
  return res;
}
