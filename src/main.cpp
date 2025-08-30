#include <iostream>
#include <SDL2/SDL.h>

#define WIN_TITLE "Snake"
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

int main(void)
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	bool running = true;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		SDL_Log("Failed to init SDL: %s\n", SDL_GetError());
		return 1;
	}

	if (SDL_CreateWindowAndRenderer(
        WIN_WIDTH, WIN_HEIGHT, 0,
        &window, &renderer) < 0) {
		SDL_Log("Failed to create window and renderer SDL: %s\n", SDL_GetError());
		return 1;
	}
	SDL_SetWindowTitle(window, WIN_TITLE);

	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = false;
					break;
			}
		}
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	return 0;
}
