#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#define WIN_TITLE "Snake"
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#define FPS 60
#define FRAMERATE (1000 / FPS)

#define UPDATE_SNAKE_RATE 0.25

int main(void)
{
	srand(time(NULL));
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
		.x = 0, .y = 0,
		.w = 20, .h = 20
	};
	float box_speed = box.w;
	float snake_update_time_acc = 0;

	SDL_FRect food = {
		.x = 0, .y = 0,
		.w = 20, .h = 20
	};
	int rand_range = WIN_WIDTH / box.w - 1; 
	food.x = (float)((rand() % rand_range) * 20);
	food.y = (float)((rand() % rand_range) * 20);

	SDL_FRect result = {};

	while (running) {
		SDL_Event event;

		Uint32 waiting_time = FRAMERATE - (SDL_GetTicks() - last_frame_tick);
		if (waiting_time > 0 && waiting_time <= FRAMERATE) SDL_Delay(waiting_time);
		deltatime = (SDL_GetTicks() - last_frame_tick) / 1000.0f;
		last_frame_tick = SDL_GetTicks();

		snake_update_time_acc += deltatime;
		if (snake_update_time_acc >= UPDATE_SNAKE_RATE) {
			if (is_left_pressed) box.x -= box_speed;
			if (is_right_pressed) box.x += box_speed;
			if (is_up_pressed) box.y -= box_speed;
			if (is_down_pressed) box.y += box_speed;
			snake_update_time_acc = 0;
		}

		if (box.x < 0) {
			box.x = WIN_WIDTH - box.w;
		} else if (box.x > WIN_WIDTH) {
			box.x = 0;
		} else if (box.y < 0) {
			box.y = WIN_HEIGHT - box.h;
		} else if (box.y > WIN_HEIGHT) {
			box.y = 0;
		}

		if (SDL_IntersectFRect(&box,
                            &food,
                            &result)) {
			food.x = (float)((rand() % rand_range) * 20);
			food.y = (float)((rand() % rand_range) * 20);
		}

		// Clear Background
		SDL_SetRenderDrawColor(renderer,
                   0x00, 0x00, 0x00,
                   SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		// Render Snake
		SDL_SetRenderDrawColor(renderer,
                   0x00, 0xFF, 0x00,
                   SDL_ALPHA_OPAQUE);
		SDL_RenderFillRectF(renderer, &box);

		// Render food
		SDL_SetRenderDrawColor(renderer,
                   0xFF, 0x00, 0x00,
                   SDL_ALPHA_OPAQUE);
		SDL_RenderFillRectF(renderer, &food);

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
							is_up_pressed = false;
							is_down_pressed = false;
							is_right_pressed = false;
							break;
						case SDLK_RIGHT:
							is_left_pressed = false;
							is_up_pressed = false;
							is_down_pressed = false;
							is_right_pressed = true;
							break;
						case SDLK_UP:
							is_left_pressed = false;
							is_up_pressed = true;
							is_down_pressed = false;
							is_right_pressed = false;
							break;
						case SDLK_DOWN:
							is_left_pressed = false;
							is_up_pressed = false;
							is_down_pressed = true;
							is_right_pressed = false;
							break;
						case SDLK_q:
							running = false;
							break;
						default:
							break;
					}
					break;
				case SDL_KEYUP:
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
