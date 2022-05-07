#include <bits/stdc++.h>
#include<SDL2/SDL.h>

//Screen dimension constants


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

//The dimensions of the level
const int LEVEL_WIDTH = 9600;
const int LEVEL_HEIGHT = 6400;

//Tile constants
const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 32;
const int TOTAL_TILES = 60000;
const int TOTAL_TILE_SPRITES1 = 324;
const int TOTAL_TILE_SPRITES2 = 196;
const int TOTAL_TILE_SPRITES3 = 16384;
const int TOTAL_TILE_SPRITES4 = 99;
const int TOTAL_TILE_SPRITES5 = 6;
const int TOTAL_TILE_SPRITES6 = 384;
const int TOTAL_TILE_SPRITES7 = 4096;
const int TOTAL_TILE_SPRITES = TOTAL_TILE_SPRITES1 + TOTAL_TILE_SPRITES2 + TOTAL_TILE_SPRITES3 + TOTAL_TILE_SPRITES4 + TOTAL_TILE_SPRITES5 + TOTAL_TILE_SPRITES6 + TOTAL_TILE_SPRITES7;

std::vector<int> roadTiles{7, 8, 20, 21, 22, 23, 34, 35, 36, 37, 49, 50, 62, 63, 64, 65, 76, 77, 78, 79, 90, 91, 92, 93, 104, 105, 106, 107, 14742};

const int WALKING_ANIMATION_FRAMES = 16;
SDL_Rect gRect = {0, 0, 64, 64};