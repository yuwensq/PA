#include <common.h>

void main_loop();
void hal_init();

int
main(int argc, char *argv[], char *envp[]) {
	Log("%d", argc);
	Log("game start!");

  hal_init();
	main_loop();

	return 0;
}
