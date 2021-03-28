#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <SDL.h> 
#include <SDL_ttf.h> 

using namespace std;

void renderPlayer(SDL_Renderer* renderer, SDL_Rect player, int x, int y, int scale, vector<int> tailX, vector<int> tailY, int tailLength)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	player.w = scale;
	player.h = scale;

	// Отримує x та y всіх хвостів блоків та відображає їх
	for (int i = 0; i < tailLength; i++)
	{
		player.x = tailX[i];
		player.y = tailY[i];
		SDL_RenderFillRect(renderer, &player);
	}

	player.x = x;
	player.y = y;

	SDL_RenderFillRect(renderer, &player);
}

