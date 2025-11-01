#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "SDL.h"

// SDL Container object
typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
} sdl_t;

// Emulator Configuration object
typedef struct {
	uint32_t window_width;	// SDL Window width
	uint32_t window_height;	// SDL Window height
	uint32_t fg_color;		// Foreground color RGBA8888
	uint32_t bg_color;		// Background color RGBA8888
	uint32_t scale_factor;	// Amount to scale a CHIP8 pixel by e.g. 20x will be a 20x larger window
} config_t;

// Emulator State
typedef enum {
	QUIT,
	RUNNING,
	PAUSED,
} emulator_state_t;

// CHIP8 Machine Object
typedef struct {
	emulator_state_t state;
} chip8_t;

// Initialize SDL
bool init_sdl(sdl_t *sdl, const config_t config) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
		SDL_Log("Could not initialize SDL subsystems! %s\n", SDL_GetError());
		return false;
	}

	sdl -> window = SDL_CreateWindow(
		"CHIP8 Emulator",
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED,
		config.window_width * config.scale_factor,
		config.window_height * config.scale_factor,
		0
	);

	if (!sdl->window) {
		SDL_Log("Could not create SDL window %s\n", SDL_GetError());
		return false;
	}

	sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
	if (!sdl->renderer) {
		SDL_Log("Could not create SDL renderer %s\n", SDL_GetError());
		return false;
	}

	return true;	// Success
}

// Final cleanup
void final_cleanup(const sdl_t sdl) {
	SDL_DestroyRenderer(sdl.renderer);
	SDL_DestroyWindow(sdl.window);
	SDL_Quit();	// Shut down SDL subsystem
}

// Set up initial emulator configuration from passed in arguments
bool set_config_from_args(config_t *config, const int argc, char **argv) {
	// Set defaults
	*config = (config_t) {
		.window_width = 64,		// CHIP8 original X resolution
		.window_height = 32,	// CHIP8 original y resolution
		.fg_color = 0xFFFFFFFF,	// YELLOW
		.bg_color = 0xFFFF00FF,	// BLACK
		.scale_factor = 20,		// Default resolution will be 1280x640
	};

	// Override defaults from passed in arguments
	for (int i = 1; i < argc; i++) {
		(void)argv[i];	// Prevent compiler error from unused ariables argc/argv
		// ...
	}

	return true;	// Success
}

// Initialize CHIP8 machine
bool init_chip8(chip8_t *chip8) {
	chip8->state = RUNNING;	// Default machine state to on/running
	return true;	// Success
}

// Clear screen / SDL window to background color
void clear_screen(const sdl_t sdl, const config_t config) {
	const uint8_t r = (config.bg_color >> 24) & 0xFF;
	const uint8_t g = (config.bg_color >> 16) & 0xFF;
	const uint8_t b = (config.bg_color >> 8) & 0xFF;
	const uint8_t a = (config.bg_color >> 0) & 0xFF;
	
	SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
	SDL_RenderClear(sdl.renderer);
}

// Update window with any changes
void update_screen(const sdl_t sdl) {
	SDL_RenderPresent(sdl.renderer);
}

// Handle user input
void handle_input(chip8_t *chip8) {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				// Exit window; End Program
				chip8->state = QUIT; // Will quit main emulator loop
				return;

			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:
						// Escape key; Exit window & End program
						chip8->state = QUIT;
						return;
					
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

// Da main squeeze
int main(int argc, char **argv) {
	// Initialize emulator configuration
	config_t config = {0};
	if (!set_config_from_args(&config, argc, argv)) exit(EXIT_FAILURE);

	// Initialize CHIP8 machine
	chip8_t chip8 = {0};
	if (!init_chip8(&chip8)) exit(EXIT_FAILURE);

	// Initialize SDL
	sdl_t sdl = {0};
	if (!init_sdl(&sdl, config)) exit(EXIT_FAILURE);

	// Initial screen clear to background color
	clear_screen(sdl, config);

	//Main emulator loop
	while (chip8.state != QUIT) {
		// Hande user inut
		handle_input(&chip8);

		// if (chip8.state == PAUSED) continue;


		// get_time();
		// Emulate CHIP8 Instructions
		// get_time() elapsed since last get_time()

		// Delay for approximately 60hz/60fps (16.67ms)
		// SDL_Delay(16 - actual time elapsed);
		SDL_Delay(16);

		// Update window with changes
		update_screen(sdl);
	}

	// Final cleanup
	final_cleanup(sdl);

	exit(EXIT_SUCCESS);
}