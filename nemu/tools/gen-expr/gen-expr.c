#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static void itoa(int num, char *str) {
  int cnt = 0;
  while (num) {
    str[cnt++] = '0' + num % 10;
    num /= 10;
  }
  int i = 0;
  for (; i < cnt / 2; i++) {
    char tmp = str[i];
    str[i] = str[cnt - i - 1];
    str[cnt - i - 1] = tmp;
  }
  return;
}

static char buf[65536];
static int pos = 0;
static int nr_token = 0;

static void insert_space() {
  int space_num = rand() % 7;
  int i = 0;
  for (; i < space_num; i++, pos++) {
    buf[pos] = ' ';
  }
}

enum {
  ORIGIN = 0,
  ADD, 
  SUB,
  MUL,
  DIV,
};

static int32_t gen_rand_expr(int father_type, int left) {
  // printf("%d\n", pos);
  char string[32] = {0};
  int type = nr_token > 5 ? 0 : rand() % 3; // 限制总的token数目
  switch (type)
  {
  case 0: {
    insert_space();
    int32_t rand_num = rand() % 0xFFFFFFFF;
    itoa(rand_num, string);
    memcpy(buf + pos, string, strlen(string));
    pos += strlen(string);
    nr_token++;
    insert_space();
    return rand_num;
  }
  case 1: {
    insert_space();
    buf[pos++] = '(';
    nr_token++;
    int32_t res = gen_rand_expr(ORIGIN, 1);
    buf[pos++] = ')';
    nr_token++;
    insert_space();
    return res;
  }
  default: {
    insert_space();
    int op = rand() % 4 + 1;
    if ((op == ADD || op == SUB) && father_type >= MUL) {
      buf[pos++] = '(';
      nr_token++;
    }
    else if (op == DIV && father_type == DIV && left == 0) {
      buf[pos++] = '(';
      nr_token++;
    }
    int32_t res1 = gen_rand_expr(op, 1);
    int32_t res2 = 0;
    int old_pos = pos;
    int old_nr_token = nr_token;
    pos++; // 加加是为了留一个运算符位置
    nr_token++;
    res2 = gen_rand_expr(op, 0);
    if (res2 == 0 && op == DIV) { // 如果除零要恢复
      memset(buf + old_pos, 0, pos - old_pos);
      pos = old_pos;
      nr_token = old_nr_token;
      insert_space();
      return res1;
    }
    if ((op == ADD || op == SUB) && father_type >= MUL) {
      buf[pos++] = ')';
    }
    else if (op == DIV && father_type == DIV && left == 0) {
      buf[pos++] = ')';
      nr_token++;
    }
    insert_space();
    switch (op)
    {
    case ADD: buf[old_pos] = '+'; return res1 + res2;
    case SUB: buf[old_pos] = '-'; return res1 - res2;
    case MUL: buf[old_pos] = '*'; return res1 * res2; 
    default: buf[old_pos] = '/'; return res1 / res2;
    }
  } break;
  }
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    // printf("%d----------------\n", i);
    nr_token = 0;
    pos = 0;
    memset(buf, 0, sizeof(buf));
    gen_rand_expr(ORIGIN, 1);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r"); // 用创建管道的方式启动一个线程
    assert(fp != NULL);

    int result;
    __attribute__((unused))
    int error = fscanf(fp, "%d", &result); 
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
