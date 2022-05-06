#include<bits/stdc++.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#pragma once

class Obstacle{
    public:
		//The dimensions of the dot
		static const int OBS_WIDTH = 60;
		static const int OBS_HEIGHT = 60;

		//Initializes the variables
		Obstacle();

		//Shows the dot on the screen
		void render(SDL_Texture* texture, SDL_Rect& camera );

		SDL_Rect getBox();

    private:
		//Collision box of the dot
		SDL_Rect mBox;

};