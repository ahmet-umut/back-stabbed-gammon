#include <SDL2/SDL.h>
#include <cmath>

SDL_Renderer *renderer;

dci (int x, int y, int r) {	//draw circle
	static SDL_Rect r1,r2;	static float f;
	for (float a=.3; a<1; a+=.3) {
		f = cos(a)+cos(a-.1);
		r1.x = x-r*f/2, r1.y = y-r*sin(a), r1.w = 2*r*f/2, r1.h = 2*r*sin(a);
		r2.x = x-r*sin(a), r2.y = y-r*f/2, r2.w = 2*r*sin(a), r2.h = 2*r*f/2;
		SDL_RenderFillRect(renderer,&r1);
		SDL_RenderFillRect(renderer,&r2);
		}
	}

sco (SDL_Color&c) {	SDL_SetRenderDrawColor(renderer,c.r,c.g,c.b,255);	}	//set color
