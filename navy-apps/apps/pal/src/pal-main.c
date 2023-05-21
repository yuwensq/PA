#include <common.h>

void main_loop();
void hal_init();

int
main(int argc) {
	Log("game start!");
	Log("%d", argc);

  hal_init();
	main_loop();

	return 0;
}
