#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#define WIN_TITLE "Snake"
#define WIN_WIDTH 600
#define WIN_HEIGHT 400

#define FPS 60
#define FRAMERATE (1000 / FPS)

#define UPDATE_SNAKE_RATE 0.10

typedef enum direction {
	DIR_LEFT,
	DIR_RIGHT,
	DIR_UP,
	DIR_DOWN,
	UNDEFINED,
} direction_t;

typedef struct snake {
	SDL_FRect head;
	SDL_FRect *body;
	int body_capacity;
	int body_count;
} snake_t;

snake_t snake_new(int capacity)
{
	snake_t snake = {0};
	snake.head = (SDL_FRect) {
		.x = 0, .y = 0,
		.w = 20, .h = 20
	};

	snake.body_capacity = capacity;
	snake.body_count = 0;
	SDL_FRect *body = calloc(snake.body_capacity, sizeof(SDL_FRect));
	assert(body != NULL);
	snake.body = body;

	return snake;
}

void snake_free(snake_t *snake)
{
	free(snake->body);
}

void snake_append_body(snake_t *snake)
{
	if (snake->body_count + 1 >= snake->body_capacity) {
		snake->body_capacity *= 2;
		SDL_FRect *body_realloc = realloc(snake->body, snake->body_capacity * sizeof(SDL_FRect));
		assert(body_realloc != NULL);
		snake->body = body_realloc;
	}
	SDL_FRect body = {0};
	body.w = body.h = 20;
	SDL_FRect last_body = {};
	if (snake->body_count == 0) {
		last_body = snake->head;
	} else {
		last_body = snake->body[snake->body_count - 1];
	}
	body.x = last_body.x;
	body.y = last_body.y;
	snake->body[snake->body_count] = body;
	snake->body_count++;
}

void snake_update_body(snake_t *snake)
{
    for (int i = snake->body_count - 1; i > 0; --i) {
        snake->body[i].x = snake->body[i - 1].x;
        snake->body[i].y = snake->body[i - 1].y;
    }

    snake->body[0].x = snake->head.x;
    snake->body[0].y = snake->head.y;
}

void snake_render(SDL_Renderer *renderer, snake_t *snake)
{
	SDL_SetRenderDrawColor(renderer,
						0x00, 0xFF, 0x00,
						SDL_ALPHA_OPAQUE);
	SDL_RenderFillRectF(renderer, &snake->head);
	for (int i = 0; i < snake->body_count; ++i) {
		SDL_RenderFillRectF(renderer, &snake->body[i]);
	}
}

void snake_dump(snake_t *snake)
{
	SDL_Log("Snake body capacity: %d\n", snake->body_capacity);
	SDL_Log("Snake body count: %d\n", snake->body_count);
	SDL_FRect last_body = snake->body[snake->body_count - 1];
	SDL_Log("Snake last body x: %f\n", last_body.x);
	SDL_Log("Snake last body y: %f\n", last_body.y);
}

bool snake_handle_body_intersect(snake_t *snake)
{
	SDL_FRect res = {0};
	for (int i = 2; i < snake->body_count; ++i) {
		if (SDL_IntersectFRect(&snake->body[i], &snake->head, &res)) {
			return true;
		}
	}
	return false;
}

int main(void)
{
	srand(time(NULL));
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	direction_t curr_dir = UNDEFINED;
	direction_t next_dir = UNDEFINED;

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

	snake_t snake = snake_new(20);

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
	int rand_range_x = WIN_WIDTH / box.w - 1; 
	int rand_range_y = WIN_HEIGHT / box.w - 1; 
	food.x = (float)((rand() % rand_range_x) * 20);
	food.y = (float)((rand() % rand_range_y) * 20);

	SDL_FRect result = {};

	while (running) {
		SDL_Event event;

		Uint32 waiting_time = FRAMERATE - (SDL_GetTicks() - last_frame_tick);
		if (waiting_time > 0 && waiting_time <= FRAMERATE) SDL_Delay(waiting_time);
		deltatime = (SDL_GetTicks() - last_frame_tick) / 1000.0f;
		last_frame_tick = SDL_GetTicks();

		snake_dump(&snake);
		snake_update_time_acc += deltatime;
		if (snake_update_time_acc >= UPDATE_SNAKE_RATE) {
			snake_update_body(&snake);
			
			curr_dir = next_dir;

			switch (curr_dir) {
				case DIR_LEFT:
					snake.head.x -= box_speed;
					break;
				case DIR_RIGHT:
					snake.head.x += box_speed;
					break;
				case DIR_UP:
					snake.head.y -= box_speed;
					break;
				case DIR_DOWN:
					snake.head.y += box_speed;
					break;
				default:
					break;
			}
			snake_update_time_acc = 0;
		}


		if (snake.head.x < 0) {
			snake.head.x = WIN_WIDTH - snake.head.w;
		} else if (snake.head.x > WIN_WIDTH - snake.head.w) {
			snake.head.x = 0;
		} else if (snake.head.y < 0) {
			snake.head.y = WIN_HEIGHT - snake.head.h;
		} else if (snake.head.y > WIN_HEIGHT - snake.head.h) {
			snake.head.y = 0;
		}

		if (SDL_IntersectFRect(&snake.head,
						 &food,
						 &result)) {
			food.x = (float)((rand() % rand_range_x) * 20);
			food.y = (float)((rand() % rand_range_y) * 20);
			snake_append_body(&snake);
		}
		if (snake_handle_body_intersect(&snake)) {
			running = false;
			printf("=================\n");
			printf("You Lose! HAH!!\n");
			break;
		}

		// Clear Background
		SDL_SetRenderDrawColor(renderer,
						 0x00, 0x00, 0x00,
						 SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		// Render Snake
		snake_render(renderer, &snake);

		// Render food
		SDL_SetRenderDrawColor(renderer,
						 0xFF, 0x00, 0x00,
						 SDL_ALPHA_OPAQUE);
		SDL_RenderFillRectF(renderer, &food);
		SDL_Log("Food Position x: %f\n", food.x);
		SDL_Log("Food Position y: %f\n", food.y);

		SDL_RenderPresent(renderer);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case SDLK_LEFT:
							if (curr_dir != DIR_RIGHT) next_dir = DIR_LEFT;
							break;
						case SDLK_RIGHT:
							if (curr_dir != DIR_LEFT) next_dir = DIR_RIGHT;
							break;
						case SDLK_UP:
							if (curr_dir != DIR_DOWN) next_dir = DIR_UP;
							break;
						case SDLK_DOWN:
							if (curr_dir != DIR_UP) next_dir = DIR_DOWN;
							break;
						case SDLK_q:
							running = false;
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
	snake_free(&snake);
	return 0;
}

