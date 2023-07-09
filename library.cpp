#include <SDL2/SDL.h>
#include <cmath>

SDL_Renderer *renderer;
SDL_Rect _1,_2;
dc (int x, int y, int r) {	//pi=4
	for (float a=0; a<=1; a+=.3) {
		_1.x = x-r*(cos(a)+.015), _1.y = y-r*(sin(a)+.015), _1.w = 2*r*(cos(a)+.015), _1.h = 2*r*(sin(a)+.015);
		_2.x = x-r*(sin(a)+.015), _2.y = y-r*(cos(a)+.015), _2.w = 2*r*(sin(a)+.015), _2.h = 2*r*(cos(a)+.015);
		SDL_RenderFillRect(renderer,&_1);
		SDL_RenderFillRect(renderer,&_2);
		}
	}	

