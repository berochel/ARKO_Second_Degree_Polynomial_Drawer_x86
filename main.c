/*
 *    Second Degree Polynomial Drawer by Jaroslaw Zabuski
 *
 *	 **LICENSE**
 *
 *	 This file is a part of Second Degree Polynomial .
 *
 *	 Second Degree Polynomial Drawer is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Second Degree Polynomial Drawer is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Second Degree Polynomial Drawer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <SDL.h>
#include <SDL_ttf.h>
#include "quadratic.h"
#include <stdio.h>

//#define DEBUG

#ifdef DEBUG
	#define debug_print_to_console(x) printf(x)
#else
	#define debug_print_to_console(x) do {} while (0)
#endif

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define STP_STEP 0.005
#define PARAMETER_STEP 0.5

enum {
	CURSOR_POSITION_B = 0,
	CURSOR_POSITION_C,
	CURSOR_POSITION_D,
	CURSOR_POSITION_S,
	CURSOR_POSITION_TOTAL
};

double func_parameters[] = {1, 0, 0, STP_STEP };
int cursorPosition = CURSOR_POSITION_B;

SDL_Rect linerect = {
		.x = 80,
		.y = 560,
		.w = 40,
		.h = 1
	};
SDL_Window *window = NULL;
SDL_Surface *screenSurface = NULL;
TTF_Font* Sans = NULL;

void initialize_SDL(void);
void terminate_SDL(void);

void drawText(void);
void quadratic_draw(SDL_Surface *surface, double *coeffs);

int main(void)
{
	int is_quit = 0, is_draw = 0;
	SDL_Event key_ev;

	initialize_SDL();

	while(!is_quit){

		while(SDL_PollEvent(&key_ev) != 0){

			is_draw = 0;

			if(key_ev.type == SDL_QUIT){
				is_quit = 1;
				continue;
			}
			else if(key_ev.type == SDL_KEYDOWN){
				switch(key_ev.key.keysym.sym){
				case SDLK_UP:
				case SDLK_w:
					debug_print_to_console("Key up\n");
					is_draw = 1;
					if(cursorPosition == CURSOR_POSITION_S){
						func_parameters[cursorPosition] += STP_STEP;
					} else {
						func_parameters[cursorPosition] += PARAMETER_STEP;
					}
					break;

				case SDLK_DOWN:
				case SDLK_s:
					debug_print_to_console("Key down\n");
					is_draw = 1;
					if(cursorPosition == CURSOR_POSITION_S){
						if(func_parameters[cursorPosition] > STP_STEP){
							func_parameters[cursorPosition] -= STP_STEP;
						} else {
							is_draw = 0;
						}
					} else {
						func_parameters[cursorPosition] -= PARAMETER_STEP;
					}
					break;

				case SDLK_LEFT:
				case SDLK_a:
					debug_print_to_console("Key left\n");
					is_draw = 1;
					cursorPosition -=  1;
					cursorPosition = (cursorPosition < 0) ? CURSOR_POSITION_TOTAL - 1 : cursorPosition;
					linerect.x = 80 + cursorPosition*60;
					break;

				case SDLK_RIGHT:
				case SDLK_d:
					debug_print_to_console("Key right\n");
					is_draw = 1;
					cursorPosition = (cursorPosition + 1)%CURSOR_POSITION_TOTAL;
					linerect.x = 80 + cursorPosition*60;
					break;

				case SDLK_q:
					debug_print_to_console("Exiting\n");
					exit(EXIT_SUCCESS);
					break;

				default:
					debug_print_to_console("Other key\n");
					break;
				}
			}

			if(is_draw){
				SDL_FillRect(screenSurface, NULL, 0);
				drawText();
				quadratic_draw(screenSurface, func_parameters);
				SDL_UpdateWindowSurface(window);
			}

		}

	}

	terminate_SDL();
	return 0;
}

void quadratic_draw(SDL_Surface *surface, double *coeffs){
	debug_print_to_console("Printing graph\n");

	quadratic_draw_body((unsigned char *)surface->pixels, surface->w, surface->h,
		-1*coeffs[0], -1*coeffs[1], -1*coeffs[2], coeffs[3]);
}

static void putString(char *text, SDL_Color color, float x_pos, float y_pos, int y_offset){

	// having to create the surface first as TTF_RenderText_Solid could only be used on SDL_Surface
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, text, color);

	SDL_Rect srcrect = {
		.x = 0,
		.y = 0,
		.w = 64,
		.h = 32
	};

	SDL_Rect dstrect = {
		.x = (int)(screenSurface->w*x_pos),
		.y = (int)(screenSurface->h*y_pos) + y_offset,
		.w = 64,
		.h = 32
	};

	SDL_FillRect(screenSurface, &dstrect, SDL_MapRGB(screenSurface->format, 255, 255, 255));
	SDL_BlitSurface(surfaceMessage, &srcrect, screenSurface, &dstrect);
}

void drawText(void){

	char string[20];

	//color in rgb format, incompatibile with SDL_FILLRECT tho
	SDL_Color Black = {.r=0, .g=0, .b=0};

	SDL_Rect srcrect = {
		.x = 0,
		.y = 0,
		.w = SCREEN_WIDTH,
		.h = SCREEN_HEIGHT
	};
	SDL_FillRect(screenSurface, &srcrect, SDL_MapRGB(screenSurface->format, 255, 255, 255));
	
	SDL_FillRect(screenSurface, &linerect, SDL_MapRGB(screenSurface->format, 0, 0, 0));

	snprintf(string, 19, "A = %.1f", func_parameters[CURSOR_POSITION_B]);
	putString(string, Black, 0.10, 0.95, 0);
	snprintf(string, 19, "B = %.1f", func_parameters[CURSOR_POSITION_C]);
	putString(string, Black, 0.175, 0.95, 0);
	snprintf(string, 19, "C = %.1f", func_parameters[CURSOR_POSITION_D]);
	putString(string, Black, 0.25, 0.95, 0);
	snprintf(string, 19, "Stp =%.3f", func_parameters[CURSOR_POSITION_S]);
	putString(string, Black, 0.325, 0.95, 0);

}

void initialize_SDL(void){

	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("SDL Init fail: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	if(TTF_Init() < 0){
		printf("TTF Init fail: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	window = SDL_CreateWindow("Projekt ARKO", SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN );

	if(window == NULL){
		printf("Failed to init window: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	

	SDL_Rect srcrect = {
		.x = 0,
		.y = 0,
		.w = SCREEN_WIDTH,
		.h = SCREEN_HEIGHT
	};
	screenSurface = SDL_GetWindowSurface(window);
	SDL_FillRect(screenSurface, &srcrect, SDL_MapRGB(screenSurface->format, 255, 255, 255));
	
	Sans = TTF_OpenFont("/usr/share/fonts/truetype/ubuntu/UbuntuMono-B.ttf", 12);
	if(Sans == NULL){
		printf("Couldn't get font: %s\n", SDL_GetError());
	}

	drawText();
	quadratic_draw(screenSurface, func_parameters);
	SDL_UpdateWindowSurface(window);
}

void terminate_SDL(void){

	SDL_DestroyWindow(window);
	window = NULL;

	SDL_Quit();
}
