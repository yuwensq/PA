#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
static int nr_use_wp = 0;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = 0;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(char* expre) {
  if (free_ == NULL) {
    Log("无可用断点\n");
    return NULL;
  }
  bool success = false;
  uint32_t res = expr(expre, &success);
  if (!success) {
    Log("断点表达式不合法\n");
    return NULL;
  }
  // 从free_里摘一个头头
  WP* result = free_;
  free_ = free_->next;
  // 把监视点加到head中
  result->next = head;
  head = result;
  // 初始化断点
  result->NO = ++nr_use_wp;
  result->last_val = res;
  memcpy(result->expr, expre, strlen(expre));
  result->expr[strlen(expre)] = 0;
  Log("分配监视点，No为%d\n", result->NO);
  return result;
}

void free_wp(WP* wp) {
  WP* tmp = head;
  if (wp == NULL) {
    Log("要释放的wp为空\n");
    goto END;
  }
  else if (head == NULL) {
    Log("监视点head链表为空\n");
    goto END;
  }
  if (head == wp) {
    head = head->next; 
    tmp->next = free_;
    free_ = tmp;
    goto SUCCESS;
  }
  while (tmp->next && tmp->next != wp)
    tmp = tmp->next;
  if (tmp->next == wp) {
    tmp->next = tmp->next->next;
    wp->next = free_;
    free_ = wp;
  }
  else {
    Log("所释放的wp不存在head中\n");
    goto END;
  }
SUCCESS:
  
END:
  return;
}

static void print_wp(WP* wp) {
  if (wp == NULL)
    return;
  print_wp(wp->next);
  printf("%d\t\t%s\n", wp->NO, wp->expr);
}

void print_wp_info() {
  printf("wp_NO\t\twp_expr\n");
  print_wp(head);
}

static bool judge_wp(WP* wp) {
  bool trigger = false;
  uint32_t res = 0;
  if (wp == NULL) 
    return false;
  trigger = judge_wp(wp->next);
  res = expr(wp->expr, NULL);
  if (res != wp->last_val) {
    trigger = true;
    printf("Trigger WP NO:%d\tlast_val:%d\tnew_val:%d\texpr:%s\n", wp->NO, wp->last_val, res, wp->expr);
    wp->last_val = res;
  }
  return trigger;
}

bool wp_trigger() {
  return judge_wp(head);
}

void remove_wp(int no) {
  WP* tmp = head;
  while (tmp) {
    if (tmp->NO == no) {
      free_wp(tmp);
    }
    tmp = tmp->next;
  }
}