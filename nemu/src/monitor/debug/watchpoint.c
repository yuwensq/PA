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
    return NULL;
  }
  // 从free_里摘一个头头
  WP* result = free_;
  free_ = free_->next;
  // 把监视点加到head中
  result->next = head;
  head = result;
  result->NO = ++nr_use_wp;
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
  nr_use_wp--;
  
END:
  return;
}
