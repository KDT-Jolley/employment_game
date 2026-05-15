#include "Game.h"

int WINAPI wmain(HINSTANCE hInstance,
	HINSTANCE,
	PWSTR,
	int) {

	Game game(960, 540);
	if (!game.Initialize(hInstance))
	{
		return -1;
	}
	game.Run();
	return 0;
}