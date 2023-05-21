#include <common.h>

void main_loop();
void hal_init();

int
main(void) {
	for (int i = 0; i < 2; i++) {
		Log("%s", argv[0]);
	}
	Log("game start!");

  hal_init();
	main_loop();

	return 0;
}
