#include <am.h>
#include <stdio.h>

_Area _heap = {};

void _trm_init() {
}

void _putc(char ch) {
  printf("%c", ch);
}

void _halt(int code) {
  _exit(0);
  // while (1);
}
