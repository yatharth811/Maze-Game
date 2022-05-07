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
		Obstacle(int x, int y, int obstacleType);

		//Shows the dot on the screen
		void render(SDL_Rect& camera );

		SDL_Rect getBox();

		int getType();

    private:
		//Collision box of the dot
		SDL_Rect mBox;

		// type 1 for prof, 2 for nurse, 3 for cycle wala, 4 for chef and 5 for dog
		int type;

};
