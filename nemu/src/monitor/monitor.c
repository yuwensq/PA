#include "nemu.h"
#include "monitor/monitor.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void init_log(const char *log_file);
void init_isa();
void init_regex();
void init_wp_pool();
void init_device();
void init_difftest(char *ref_so_file, long img_size);

static char *mainargs = "";
static char *log_file = NULL;
static char *diff_so_file = NULL;
static char *img_file = NULL;
static int is_batch_mode = false;

static inline void welcome() {
#ifdef DEBUG
  Log("Debug: \33[1;32m%s\33[0m", "ON");
  Log("If debug mode is on, A log file will be generated to record every instruction NEMU executes. "
      "This may lead to a large log file. "
      "If it is not necessary, you can turn it off in include/common.h.");
#else
  Log("Debug: \33[1;32m%s\33[0m", "OFF");
#endif

  Log("Build time: %s, %s", __TIME__, __DATE__);
  printf("Welcome to \33[1;41m\33[1;33m%s\33[0m-NEMU!\n", str(__ISA__));
  printf("For help, type \"help\"\n");
}

static inline long load_img() {
  long size;
  if (img_file == NULL) {
    Log("No image is given. Use the default build-in image.");
    extern uint8_t isa_default_img[];
    extern long isa_default_img_size;
    size = isa_default_img_size;
    memcpy(guest_to_host(IMAGE_START), isa_default_img, size);
  }
  else {
    int ret;

    FILE *fp = fopen(img_file, "rb");
    Assert(fp, "Can not open '%s'", img_file);

    Log("The image is %s", img_file);

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    fseek(fp, 0, SEEK_SET);
    ret = fread(guest_to_host(IMAGE_START), size, 1, fp);
    assert(ret == 1);

    fclose(fp);

    // mainargs
    strcpy(guest_to_host(0), mainargs);
  }
  return size;
}

static inline void parse_args(int argc, char *argv[]) {
	//https://www.cnblogs.com/qingergege/p/5914218.html
  int o;
  while ( (o = getopt(argc, argv, "-bl:d:a:")) != -1) {
    switch (o) {
      case 'b': is_batch_mode = true; break;
      case 'a': mainargs = optarg; break;
      case 'l': log_file = optarg; break;
      case 'd': diff_so_file = optarg; break;
      case 1:
                if (img_file != NULL) Log("too much argument '%s', ignored", optarg);
                else img_file = optarg;
                break;
      default:
                panic("Usage: %s [-b] [-l log_file] [img_file]", argv[0]);
    }
  }
}

uint32_t expr(char*, bool*);

static inline void test_expr() {
  FILE *fp = fopen("/home/yuwensq/tmp_workspace/ics2019/nemu/tools/gen-expr/input", "r");
  Assert(fp, "打开表达式测试文件失败");
  bool success = true;
  char line[65536] = {};
  char *l_expr = NULL;
  char *true_result = NULL; 
  uint32_t t_result = 0;
  uint32_t my_result = 0;
  while (fgets(line, 65536, fp)) {
    if (line[strlen(line) - 1] == '\n') 
      line[strlen(line) - 1] = 0;
    true_result = strtok(line, " ");
    t_result = atoi(true_result);
    l_expr = true_result + strlen(true_result) + 1;
    my_result = expr(l_expr, &success);
    if (!success || t_result != my_result) {
      success = false;
      printf("expr:%s, true:%u, my:%u\n", l_expr, t_result, my_result);
      break;
    }
  }
  // Assert(success, "表达式计算有问题，请检查错误");
  fclose(fp);
}

int init_monitor(int argc, char *argv[]) {
  /* Perform some global initialization. */

  /* Parse arguments. */
  parse_args(argc, argv);

  /* Open the log file. */
  init_log(log_file);

  /* Load the image to memory. */
  long img_size = load_img();

  /* Perform ISA dependent initialization. */
  init_isa();

  /* Compile the regular expressions. */
  init_regex();

  test_expr();

  /* Initialize the watchpoint pool. */
  init_wp_pool();

  /* Initialize devices. */
  init_device();

  /* Initialize differential testing. */
  init_difftest(diff_so_file, img_size);

  /* Display welcome message. */
  welcome();

  return is_batch_mode;
}
