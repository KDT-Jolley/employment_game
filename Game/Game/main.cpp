#include "Game.h"

int wmain(wchar_t** argv, wchar_t** evnp) {

	Game game(960, 540);
	game.Run();
	return 0;
}