#include <stdbool.h>
#include <SDL2/SDL.h>

#define WIN_TITLE "Snake"
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define FPS 60
#define FRAMERATE (1000 / FPS)

int main(void)
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	bool is_up_pressed = false;
	bool is_down_pressed = false;
	bool is_left_pressed = false;
	bool is_right_pressed = false;

	bool running = true;

	Uint32 last_frame_tick = 0;
	float deltatime = 0.0f;

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

	SDL_FRect box = {
		.x = 10, .y = 10,
		.w = 20, .h = 20
	};

	while (running) {
		SDL_Event event;

		Uint32 waiting_time = FRAMERATE - (SDL_GetTicks() - last_frame_tick);
		if (waiting_time > 0 && waiting_time <= FRAMERATE) SDL_Delay(waiting_time);
		deltatime = (SDL_GetTicks() - last_frame_tick) / 1000.0f;
		last_frame_tick = SDL_GetTicks();

		if (is_left_pressed) box.x -= 1;
		if (is_right_pressed) box.x += 1;

		SDL_SetRenderDrawColor(renderer,
                   0x00, 0x00, 0x00,
                   SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer,
                   0xFF, 0x00, 0x00,
                   SDL_ALPHA_OPAQUE);
		SDL_RenderFillRectF(renderer, &box);

		SDL_RenderPresent(renderer);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_LEFT:
							is_left_pressed = true;
							break;
						case SDLK_RIGHT:
							is_right_pressed = true;
							break;
						case SDLK_UP:
							is_up_pressed = true;
							break;
						case SDLK_DOWN:
							is_down_pressed = true;
							break;
						case SDLK_q:
							running = false;
							break;
						default:
							break;
					}
					break;
				case SDL_KEYUP:
					switch (event.key.keysym.sym) {
						case SDLK_LEFT:
							is_left_pressed = false;
							break;
						case SDLK_RIGHT:
							is_right_pressed = false;
							break;
						case SDLK_UP:
							is_up_pressed = false;
							break;
						case SDLK_DOWN:
							is_down_pressed = false;
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	return 0;
}
