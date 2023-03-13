#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
  if (free_ == NULL) {
    return NULL;
  }
  // 从free_里摘一个头头
  WP* result = free_;
  free_ = free_->next;
  // 把监视点加到head中
  result->next = head;
  head = result;
  Log("分配监视点，No为%d\n", result->NO);
  return result;
}

void free_wp(WP* wp) {

}


