#include <common.h>

void main_loop();
void hal_init();

int
main(int argc, char *argv[]) {
	Log("game start!");
	printf("%x", argv);

  hal_init();
	main_loop();

	return 0;
}
