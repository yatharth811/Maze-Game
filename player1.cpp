#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <bits/stdc++.h>
#include <fstream>
#include "timer.h"
#include "LTexture.h"
#include "Tile.h"
#include "obstacle.h"
#include "constants.h"
using namespace std;


SDL_Rect charOneClips[WALKING_ANIMATION_FRAMES];
SDL_Rect charTwoClips[WALKING_ANIMATION_FRAMES];
SDL_Rect healthClips[6];

int frame = 0;

//Texture wrapper class
//The tile

//The dot that will move around on the screen


//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia( Tile* tiles[], Tile* tiles2[]);

//Frees media and shuts down SDL
void close( Tile* tiles[] );

//Box collision detector
bool checkCollision( SDL_Rect a, SDL_Rect b );

//Checks collision box against set of tiles
bool touchesRoad( SDL_Rect box, Tile* tiles[] );

//Sets tiles from tile map
bool setTiles( Tile *tiles[], string file);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;


//Scene textures
LTexture gDotTexture;
LTexture charOneTexture;
LTexture charTwoTexture;
LTexture firstImageTexture;
LTexture secondImageTexture;
LTexture thirdImageTexture;
LTexture fourthImageTexture;
LTexture fifthImageTexture;
LTexture sixthImageTexture;
LTexture seventhImageTexture;
LTexture healthImageTexture;
LTexture nurseTexture;
LTexture professorTexture;
LTexture cycleWalaTexture;
LTexture cycleOneTexture;
LTexture cycleTwoTexture;
LTexture chefTexture;
LTexture timerTexture;
LTexture startScreenTexture;
LTexture ruleScreenTexture;
LTexture cashTexture;
LTexture cashTextTexture;
LTexture winnerScreen;
LTexture loserScreen;
LTexture promptTexture1;
LTexture healthTextTexture1;
LTexture healthTextTexture2;

TTF_Font* gFont = NULL;
Mix_Music *gMusic = NULL;

SDL_Rect gTileClips1[ TOTAL_TILE_SPRITES1 ];
SDL_Rect gTileClips2[ TOTAL_TILE_SPRITES2 ];
SDL_Rect gTileClips3[ TOTAL_TILE_SPRITES3 ];
SDL_Rect gTileClips4[ TOTAL_TILE_SPRITES4 ];
SDL_Rect gTileClips5[ TOTAL_TILE_SPRITES5 ];
SDL_Rect gTileClips6[ TOTAL_TILE_SPRITES6 ];
SDL_Rect gTileClips7[ TOTAL_TILE_SPRITES7 ];

LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		if(path.compare("assests/character.bmp"))
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0xFE, 0xFE, 0xFE ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

Tile::Tile( int x, int y, int tileType )
{
    //Get the offsets
    mBox.x = x;
    mBox.y = y;

    //Set the collision box
    mBox.w = TILE_WIDTH;
    mBox.h = TILE_HEIGHT;

    //Get the tile type
    mType = tileType;
}

void Tile::render( SDL_Rect& camera )
{
    //If the tile is on screen
    if( checkCollision( camera, mBox ) )
    {
        //Show the tile
		if (mType == 0)
			// firstImageTexture.render( mBox.x - camera.x, mBox.y - camera.y, &gTileClips1[232] );
			return;

		if (mType > 0 && mType <= 324)
        	firstImageTexture.render( mBox.x - camera.x, mBox.y - camera.y, &gTileClips1[ mType - 1] );

		else if (mType > 324 && mType <= 520)
			secondImageTexture.render( mBox.x - camera.x, mBox.y - camera.y, &gTileClips2[ mType - 325] );
		
		else if (mType > 520 && mType <= 16904)
			thirdImageTexture.render( mBox.x - camera.x, mBox.y - camera.y, &gTileClips3[ mType - 521 ] );
		
		else if (mType > 16904 && mType <= 17003)
			fourthImageTexture.render(mBox.x - camera.x, mBox.y - camera.y, &gTileClips4[ mType - 16905 ] );

		else if (mType > 17003 && mType <= 17009)
			fifthImageTexture.render(mBox.x - camera.x, mBox.y - camera.y, &gTileClips5[ mType - 17004 ] );

		else if (mType > 17009 && mType <= 17393)
			sixthImageTexture.render(mBox.x - camera.x, mBox.y - camera.y, &gTileClips6[ mType - 17010 ] );

		else
			seventhImageTexture.render(mBox.x - camera.x, mBox.y - camera.y, &gTileClips7[ mType - 17394 ] );
		
		// else
		// 	fourthImageTexture.render(mBox.x - camera.x, mBox.y - camera.y, &gTileClips4[ mType ])
	}
}

int Tile::getType()
{
    return mType;
}

SDL_Rect Tile::getBox()
{
    return mBox;
}


class Character{
	public:
	SDL_Rect charBox;
	int health, state, cash; // State 0 for walking, 1 for cycling
	int id, direction, mVelx, mVely;
	static const int DOT_VEL = 10;
	Character(int playerId){
		health = 60;
		charBox.x = 9290;
		charBox.y = 3520;
		charBox.w = 60;
		charBox.h = 60;
		direction = 0;
		mVelx = 0;
		mVely = 0;
		id = playerId;
		state = 0;
		cash = 0;
	}

	void render(SDL_Renderer* gRenderer,SDL_Rect* camera, int frame, bool flag){
		int healthspriteno;
		SDL_Rect newrect = {charBox.x-camera->x,charBox.y-camera->y,charBox.w,charBox.h};

		SDL_Rect healthRect = {0, 0, 140, 60};

		if(health>=90 && health<=100) healthspriteno=0;
		else if(health>=70 && health<90) healthspriteno = 1;
		else if(health>=50 && health<70) healthspriteno = 2;
		else if(health>=30 && health<50) healthspriteno = 3;
		else if(health>=10 && health<30) healthspriteno = 4;
		else if(health>=0 && health<10) healthspriteno = 5;
		

		if (id == 1){
			healthRect.y -= (healthspriteno != 0 || healthspriteno != 5 ? 10 : 0);
			if(flag && !state) SDL_RenderCopy(gRenderer,charOneTexture.getTexture(),&charOneClips[direction*4+frame],&newrect);
			if(flag && state) SDL_RenderCopy(gRenderer,cycleOneTexture.getTexture(),&charOneClips[direction*4+frame],&newrect);
			SDL_RenderCopy(gRenderer,healthImageTexture.getTexture(),&healthClips[healthspriteno],&healthRect);
		}
		else if (id == 2){
			healthRect.y = healthRect.y + 60;
			if(flag && !state) SDL_RenderCopy(gRenderer,charTwoTexture.getTexture(),&charTwoClips[direction*4+frame],&newrect);
			if(flag && state) SDL_RenderCopy(gRenderer,cycleTwoTexture.getTexture(),&charTwoClips[direction*4+frame],&newrect);
			SDL_RenderCopy(gRenderer,healthImageTexture.getTexture(),&healthClips[healthspriteno],&healthRect);
		}	
	}

	void handleEvent(SDL_Event& e){
		if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
		{
			//Adjust the velocity
			switch( e.key.keysym.sym )
			{
				case SDLK_UP: mVely -= DOT_VEL; direction = 3; break;
				case SDLK_DOWN: mVely += DOT_VEL; direction = 0; break;
				case SDLK_LEFT: mVelx -= DOT_VEL; direction = 1; break;
				case SDLK_RIGHT: mVelx += DOT_VEL; direction = 2; break;
			}
		}
		//If a key was released
		else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
		{
			//Adjust the velocity
			switch( e.key.keysym.sym )
			{
				case SDLK_UP: mVely += DOT_VEL; break;
				case SDLK_DOWN: mVely -= DOT_VEL; break;
				case SDLK_LEFT: mVelx += DOT_VEL; break;
				case SDLK_RIGHT: mVelx -= DOT_VEL; break;
			}
		}
	}

	void move(Tile *tiles[]){
		 charBox.x += mVelx;

		//If the dot went too far to the left or right or touched a wall
		// || touchesRoad( mBox, tiles ) 
		if( ( charBox.x < 0 ) || ( charBox.x + charBox.w > LEVEL_WIDTH ) || !touchesRoad(charBox, tiles))
		{
			//move back
			charBox.x -= mVelx;
		}

		// cout << mBox.x << endl;
		//Move the dot up or down
		charBox.y += mVely;

		//If the dot went too far up or down or touched a wall
		// || touchesRoad( mBox, tiles )
		if( ( charBox.y < 0 ) || ( charBox.y + charBox.h > LEVEL_HEIGHT ) || !touchesRoad(charBox, tiles))
		{
			//move back
			charBox.y -= mVely;
		}

	}

	void setCamera(SDL_Rect& camera){
		//Center the camera over the dot
		camera.x = ( charBox.x + charBox.w / 2 ) - SCREEN_WIDTH / 2;
		camera.y = ( charBox.y + charBox.h / 2 ) - SCREEN_HEIGHT / 2;

		if( camera.x < 0 )
		{ 
			camera.x = 0;
		}
		if( camera.y < 0 )
		{
			camera.y = 0;
		}
		if( camera.x > LEVEL_WIDTH - camera.w )
		{
			camera.x = LEVEL_WIDTH - camera.w;
		}
		if( camera.y > LEVEL_HEIGHT - camera.h )
		{
			camera.y = LEVEL_HEIGHT - camera.h;
		}

	}

	SDL_Rect getCharRect(){
        return charBox;
    }

    bool checkcolwithchar(SDL_Rect camera, SDL_Rect fromserver){
        charBox = fromserver;
        return checkCollision(charBox,camera);
    }

	int getDirection(){
		return direction;
	}

	void changedirection(int dir){
		direction = dir;
	}

	int getHealth(){
		return health;
	}

	void changehealth(int heal){
		health = heal;
	}

	void checkStats(){
		health = max(0, health);
		health = min(health, 100);
		cash = max(0, cash);
	}

};


Obstacle :: Obstacle(int x, int y, int obstacleType){
	mBox.x = x;
	mBox.y = y;
	mBox.w = OBS_WIDTH;
	mBox.h = OBS_HEIGHT;
	type = obstacleType;
}

void Obstacle :: render(SDL_Rect& camera){
	if (checkCollision(mBox, camera)){
		if (type == 1){
			SDL_Rect newBox = {mBox.x - camera.x, mBox.y - camera.y, mBox.w, mBox.h};
			SDL_RenderCopy(gRenderer, professorTexture.getTexture(), &gRect, &newBox);
		}
		else if (type == 2){
			SDL_Rect newBox = {mBox.x - camera.x, mBox.y - camera.y, mBox.w, mBox.h};
			SDL_RenderCopy(gRenderer, nurseTexture.getTexture(), &gRect, &newBox);
		}
		else if (type == 3){
			SDL_Rect newBox = {mBox.x - camera.x, mBox.y - camera.y, mBox.w, mBox.h};
			SDL_RenderCopy(gRenderer, cycleWalaTexture.getTexture(), &gRect, &newBox);
		}
		else if (type == 4){
			SDL_Rect newBox = {mBox.x - camera.x, mBox.y - camera.y, mBox.w, mBox.h};
			SDL_RenderCopy(gRenderer, chefTexture.getTexture(), &gRect, &newBox);
		}
		else{
			return;
		}
	}
}

SDL_Rect Obstacle :: getBox(){
	return mBox;
}

int Obstacle :: getType(){
	return type;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "Player 1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				 //Initialize SDL_mixer
                if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                }
			}
		}
	}

	if( TTF_Init() == -1 )
		{
			printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
			success = false;
		}

	return success;
}

bool loadMedia( Tile* tiles[], Tile* tiles2[])
{
	//Loading success flag
	bool success = true;

	 //Load music
    gMusic = Mix_LoadMUS( "assets/music.wav" );
    if( gMusic == NULL )
    {
        printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }

	if (!winnerScreen.loadFromFile("assets/won.png")){
		printf( "Failed to load winner texture!\n" );
		success = false;
	}

	if (!loserScreen.loadFromFile("assets/loss.png")){
		printf( "Failed to load loser texture!\n" );
		success = false;
	}

	if(!cashTexture.loadFromFile("assets/cash.png")){
		printf( "Failed to load professor texture!\n" );
		success = false;
	}

	gFont = TTF_OpenFont( "assets/SigmarOne-Regular.ttf", 32 );
    if( gFont == NULL )
    {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    } 

	SDL_Color clr = {255,255,255,255};
	string msg1 = "You need at least 80 health to get a yulu!", msg2 = "You need at least 20 cash to buy food!" ;

	if( !promptTexture1.loadFromRenderedText( msg1.c_str(), clr) )
	{
		printf( "Unable to render prompt1 texture!\n" );
	}

	if(!nurseTexture.loadFromFile("assets/nurse.png")){
		printf( "Failed to load nurse texture!\n" );
		success = false;
	}

	if(!professorTexture.loadFromFile("assets/prof1.png")){
		printf( "Failed to load professor texture!\n" );
		success = false;
	}

	if(!cycleWalaTexture.loadFromFile("assets/prof2.png")){
		printf( "Failed to load cycle wala texture!\n" );
		success = false;
	}

	if(!cycleOneTexture.loadFromFile("assets/lucasbike.png")){
		printf( "Failed to load lucas' cycle texture!\n" );
		success = false;
	}

	if(!cycleTwoTexture.loadFromFile("assets/dawnbike.png")){
		printf( "Failed to load dawn's cycle texture!\n" );
		success = false;
	}


	if (!chefTexture.loadFromFile("assets/chef.png")){
		printf( "Failed to load chef texture!\n" );
		success = false;
	}


	if(!healthImageTexture.loadFromFile("assets/health.png")){
		printf( "Failed to load health texture!\n" );
		success = false;
	}
	else{
		for(int i=0;i<=5;i++){
			healthClips[i] = {0, 82*i, 496, 82};
		}
	}

	//Load dot texture
	if( !gDotTexture.loadFromFile( "assets/dot.bmp" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

	if( !charOneTexture.loadFromFile("assets/lucas.png")){
		printf( "Failed to load walking animation texture!\n" );
        success = false;
	}

	else{
		//direction down
		int x = 0, y = 0, cnt = 0;

		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				charOneClips[cnt] = {x, y, 64, 64};
				x += 64;
				cnt++;
			}
			x = 0;
			y += 64;
		}
	}

	if (!charTwoTexture.loadFromFile("assets/dawn.png")){
		printf( "Failed to load walking animation texture!\n" );
        success = false;
	}
	else{
		int x = 0, y = 0, cnt = 0;

		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				charTwoClips[cnt] = {x, y, 64, 64};
				x += 64;
				cnt++;
			}
			x = 0;
			y += 64;
		}	
	}

	//Load tile texture
	if( !firstImageTexture.loadFromFile( "assets/grass.jpeg" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}

	if( !secondImageTexture.loadFromFile( "assets/outdoors.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}

	if( !thirdImageTexture.loadFromFile( "assets/main.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}

	if( !fourthImageTexture.loadFromFile( "assets/vehicles.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}


	if( !fifthImageTexture.loadFromFile( "assets/font1.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}


	if( !sixthImageTexture.loadFromFile( "assets/buttons.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}


	if( !seventhImageTexture.loadFromFile( "assets/blue-spritesheet.png" ) )
	{
		printf( "Failed to load tile set texture!\n" );
		success = false;
	}


	//Load tile map
	if( !setTiles( tiles, "maps/layer1.txt") )
	{
		printf( "Failed to load tile set for layer 1!\n" );
		success = false;
	}

	if( !setTiles( tiles2, "maps/layer2.txt") )
	{
		printf( "Failed to load tile set for layer 2!\n" );
		success = false;
	}

	if (!startScreenTexture.loadFromFile("assets/start-screen.png")){
		printf( "Failed to start screen\n" );
		success = false;	
	}

	if (!ruleScreenTexture.loadFromFile("assets/rules.png")){
		printf( "Failed to load rule screen\n" );
		success = false;	
	}


	return success;
}

void close( Tile* tiles[] )
{
	//Deallocate tiles
	for( int i = 0; i < TOTAL_TILES; ++i )
	{
		 if( tiles[ i ] != NULL )
		 {
			delete tiles[ i ];
			tiles[ i ] = NULL;
		 }
	}

	//Free loaded images
	gDotTexture.free();
	firstImageTexture.free();
	secondImageTexture.free();
	thirdImageTexture.free();
	fourthImageTexture.free();
	fifthImageTexture.free();
	sixthImageTexture.free();
	seventhImageTexture.free();
	charOneTexture.free();
	charTwoTexture.free();
	healthImageTexture.free();
	nurseTexture.free();
	professorTexture.free();
	cycleWalaTexture.free();
	cycleOneTexture.free();
	cycleTwoTexture.free();
	chefTexture.free();
	startScreenTexture.free();
	ruleScreenTexture.free();
	cashTextTexture.free();
	winnerScreen.free();
	loserScreen.free();
	promptTexture1.free();
	healthTextTexture1.free();
	healthTextTexture2.free();

	 //Free the music
    Mix_FreeMusic( gMusic );
    gMusic = NULL;

	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	timerTexture.free();

	TTF_CloseFont( gFont );
    gFont = NULL;    



	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool checkCollision( SDL_Rect a, SDL_Rect b )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}

bool setTiles( Tile* tiles[], string file)
{
	//Success flag
	bool tilesLoaded = true;

    //The tile offsets
    int x = 0, y = 0;

    //Open the map
    std::ifstream map(file);

    //If the map couldn't be loaded
    if( map.fail() )
    {
		printf( "Unable to load map file!\n" );
		tilesLoaded = false;
    }
	else
	{
		//Initialize the tiles
		for( int i = 0; i < TOTAL_TILES; ++i )
		{
			//Determines what kind of tile will be made
			int tileType = -1;

			//Read tile from map file
			map >> tileType;

			//If the was a problem in reading the map
			if( map.fail() )
			{
				//Stop loading map
				printf( "Error loading map: Unexpected end of file!\n" );
				tilesLoaded = false;
				break;
			}

			//If the number is a valid tile number
			if( ( tileType >= 0 ) && ( tileType < TOTAL_TILE_SPRITES ) )
			{
				tiles[ i ] = new Tile( x, y, tileType );
				// cout << x << " " << y << endl;
			}
			//If we don't recognize the tile type
			else
			{
				//Stop loading map
				printf( "Error loading map: Invalid tile type at %d!\n", i );
				cout << "Tile Type: " << tileType << endl;
				tilesLoaded = false;
				break;
			}

			//Move to next tile spot
			x += TILE_WIDTH;

			//If we've gone too far
			if( x >= LEVEL_WIDTH )
			{
				//Move back
				x = 0;

				//Move to the next row
				y += TILE_HEIGHT;
			}
		}
		
		//Clip the sprite sheet
		if( tilesLoaded )
		{
			int cnt1 = 0, x1 = 0, y1 = 0;
			for (int i = 0; i < 18; i++){
				for (int j=0; j < 18; j++){
					gTileClips1[cnt1] = {x1, y1, TILE_WIDTH, TILE_HEIGHT};
					x1 += 32;
					cnt1++; 
				}
				y1 += 32;
				x1 = 0;
			}

			int cnt2 = 0, x2 = 0, y2 = 0;
			for (int i = 0; i < 14; i++){
				for (int j=0; j < 14; j++){
					gTileClips2[cnt2] = {x2, y2, TILE_WIDTH, TILE_HEIGHT};
					x2 += 32;
					cnt2++; 
				}
				y2 += 32;
				x2 = 0;
			}

			int cnt3 = 0, x3 = 0, y3 = 0;
			for (int i = 0; i < 128; i++){
				for (int j=0; j < 128; j++){
					gTileClips3[cnt3] = {x3, y3, TILE_WIDTH, TILE_HEIGHT};
					x3 += 32;
					cnt3++; 
				}
				y3 += 32;
				x3 = 0;
			}

			int cnt4 = 0, x4 = 0, y4 = 0;
			for (int i = 0; i < 9; i++){
				for (int j=0; j < 11; j++){
					gTileClips4[cnt4] = {x4, y4, TILE_WIDTH, TILE_HEIGHT};
					x4 += 32;
					cnt4++; 
				}
				y4 += 32;
				x4 = 0;
			}

			int cnt5 = 0, x5 = 0, y5 = 0;
			for (int i = 0; i < 2; i++){
				for (int j=0; j < 3; j++){
					gTileClips5[cnt5] = {x5, y5, TILE_WIDTH, TILE_HEIGHT};
					x5 += 32;
					cnt5++; 
				}
				y5 += 32;
				x5 = 0;
			}


			int cnt6 = 0, x6 = 0, y6 = 0;
			for (int i = 0; i < 48; i++){
				for (int j=0; j < 8; j++){
					gTileClips6[cnt6] = {x6, y6, TILE_WIDTH, TILE_HEIGHT};
					x6 += 32;
					cnt6++; 
				}
				y6 += 32;
				x6 = 0;
			}

			int cnt7 = 0, x7 = 0, y7 = 0;
			for (int i = 0; i < 64; i++){
				for (int j=0; j < 64; j++){
					gTileClips7[cnt7] = {x7, y7, TILE_WIDTH, TILE_HEIGHT};
					x7 += 32;
					cnt7++; 
				}
				y7 += 32;
				x7 = 0;
			}



		}
	}

    //Close the file
    map.close();

    //If the map was loaded fine
    return tilesLoaded;
}

bool touchesRoad( SDL_Rect box, Tile* tiles[] )
{
    //Go through the tiles
    for( int i = 0; i < TOTAL_TILES; ++i )
    {
        //If the tile is a wall type tile
        int type = tiles[i]->getType() - 325;
		// cout << type << endl;
		SDL_Rect tet = tiles[i]->getBox();

		// if( checkCollision( box, tiles[ i ]->getBox() ) )
        //     {
		// 		cout << "YES " << type << endl;
        //         // return true;
        //     }

		if(find(roadTiles.begin(), roadTiles.end(), type) != roadTiles.end())
        {
            if( checkCollision( box, tiles[ i ]->getBox() ) )
            {
				// cout << "YES " << type << endl;
                return true;
            }
        }
    }

    //If no wall tiles were touched
    return false;
}

int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//The level tiles
		Tile* tileSet[ TOTAL_TILES ];
		Tile* tileSet2[TOTAL_TILES];

		//Networking
		int datain[] = {0,0,0,0,0,0,0,0,0}, dataout[] = {0,0,0,0,0,0,0,0,0};

		IPaddress ip;
        SDLNet_ResolveHost(&ip, "127.0.0.1", 1234);
        TCPsocket client = SDLNet_TCP_Open(&ip);

		//Load media
		if (client!=NULL){

			if( !loadMedia( tileSet, tileSet2) )
			{
				printf( "Failed to load media!\n" );
			}
			else
			{	
				//Main loop flag
				bool quit = false;

				//Event handler
				SDL_Event e;

				//The dot that will be moving around on the screen
				//Dot dot;

				LTimer timer;

				SDL_Color textColor = { 255, 255, 255, 255 };

				stringstream timeText, cashText, healthText1, healthText2;
				Character boy(1);
				Character boy2(2);
				Obstacle prof1(7970, 3680, 1);
				Obstacle prof2(6910, 3500, 1);
				Obstacle prof3(8220, 1280, 1);
				Obstacle prof4(8550, 2670, 1);
				Obstacle prof5(7425, 2420, 1);
				Obstacle nurse(5025, 3420, 2);
				Obstacle cycleWala(410, 0, 3);
				Obstacle chefShiru(7390, 3440, 4);
				Obstacle chefAmul(7380, 2940, 4);
				Obstacle chefRajdhani(3780, 3370, 4);
				Obstacle chefChayos(4110, 2750, 4);
				Obstacle chefMasalaMix(4110, 2660, 4);

				//Level camera
				SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
				SDL_Rect fromserver;
				SDL_Rect destinationEnd = {9290,3520,60,60};

				// SDL_Rect nurseSource = {0, 0, 64, 64};
				// SDL_Rect nurseDestination = {5025,3420,60,60};
				// SDL_Rect profDestinations[5];
				// profDestinations[0] = {7970, 3680, 60, 60};
				// profDestinations[1] = {6910, 3500, 60, 60};
				// profDestinations[2] = {8220, 1280, 60, 60};
				// profDestinations[3] = {8550, 2670, 60, 60};
				// profDestinations[4] = {7425, 2420, 60, 60};

				int previousCollision1 = -1e6;
				int previousCollision2 = -1e6;
				int previousCollision3 = -1e6;
				int previousCollision4 = -1e6;
				int previousCollision5 = -1e6;
				int previousCollisionNurse = -1e6;
				int previousCollisionShiru = -1e6;
				int previousCollisionAmul = -1e6;
				int previousCollisionRajdhani = -1e6;
				int previousCollisionChayos = -1e6;
				int previousCollisionMasalaMix = -1e6;
				int winner = 0;

				bool startScreen = true, gameStarted = false, gameEnded = false, ruleScreen = false;



				//While application is running
				while( !quit )
				{
					if (!timer.isStarted()) timer.start();
					//Handle events on queue
					while( SDL_PollEvent( &e ) != 0 )
					{
						//User requests quit
						if( e.type == SDL_QUIT )
						{
							quit = true;
							dataout[0] = -1;
                            SDLNet_TCP_Send(client, dataout, 36);
						}

						//Handle input for the dot
						// dot.handleEvent( e );
						if (startScreen || ruleScreen){
							if (e.type == SDL_KEYDOWN){
								if (e.key.keysym.sym == SDLK_1){
									gameStarted = true;
									startScreen = false;
									gameEnded = false;
									ruleScreen = false;
								}
								else if (e.key.keysym.sym == SDLK_2){
									gameStarted = false;
									startScreen = false;
									gameEnded = false;
									ruleScreen = true;
								}
							}
						}
						
						if (gameStarted){
							boy.handleEvent(e);
							if(e.type == SDL_KEYDOWN){
								frame++;
								if(frame >= 16){
									frame = 0;
								}
							}
						}
						
					}

					//Clear screen
					SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
					SDL_RenderClear( gRenderer );

					//Move the dot
					// dot.move( tileSet2 );
					// dot.setCamera( camera );
					if (startScreen){
						startScreenTexture.render(0,0);
					}
					else if (ruleScreen){
						ruleScreenTexture.render(0,0);
					}
					else if (gameStarted){

						if (checkCollision(boy.getCharRect(), destinationEnd) && boy.state){
							gameEnded = true;
							// gameStarted = false;
							winner = 1;
						}
						else if (boy.health == 0){
							gameEnded = true;
							// gameStarted = false;
							winner = 2;
						}
						else if (datain[7]){
							gameEnded = true;
							// gameStarted = false;
							winner = datain[7];
						}

						
						boy.move(tileSet2);
						SDL_Rect curpos = boy.getCharRect();
						dataout[0] = curpos.x;
						dataout[1] = curpos.y;
						dataout[2] = curpos.w;
						dataout[3] = curpos.h;
						dataout[4] = boy.getDirection();
						dataout[5] = frame/4;
						dataout[6] = boy.getHealth();
						dataout[7] = winner;
						dataout[8] = boy.state;
						SDLNet_TCP_Send(client, dataout, 36);
						boy.setCamera(camera);
						//Render level
						for( int i = 0; i < TOTAL_TILES; ++i )
						{
							tileSet[ i ]->render( camera );
						}

						//Render level
						for( int i = 0; i < TOTAL_TILES; ++i )
						{
							tileSet2[ i ]->render( camera );
						}

						boy.render(gRenderer, &camera, frame/4, true);

						SDLNet_TCP_Recv(client,datain,36);
						fromserver = {datain[0], datain[1], datain[2], datain[3]};

						boy2.changehealth(datain[6]);
						boy2.state = datain[8];

						healthText1.str("");
						healthText1 << boy.health;

						if( !healthTextTexture1.loadFromRenderedText( healthText1.str().c_str(), textColor ) )
						{
							printf( "Unable to render time texture!\n" );
						}

						healthTextTexture1.render(145 , 0);


						healthText2.str("");
						healthText2 << boy2.health;

						if( !healthTextTexture2.loadFromRenderedText( healthText2.str().c_str(), textColor ) )
						{
							printf( "Unable to render time texture!\n" );
						}

						healthTextTexture2.render(145 , 80);




						bool f = boy2.checkcolwithchar(camera,fromserver);
						if(f) boy2.changedirection(datain[4]);
						boy2.render(gRenderer, &camera, datain[5], f);

						if(Mix_PlayingMusic() == 0)
						Mix_PlayMusic( gMusic, -1 );

						prof1.render(camera);
						prof2.render(camera);
						prof3.render(camera);
						prof4.render(camera);
						prof5.render(camera);
						nurse.render(camera);
						cycleWala.render(camera);
						chefShiru.render(camera);
						chefAmul.render(camera);
						chefRajdhani.render(camera);
						chefChayos.render(camera);
						chefMasalaMix.render(camera);

						int gameTime = timer.getTicks() / 1000;
						int hours = gameTime/3600, minutes = (gameTime%3600)/60;
						// cout << "Time: " << hours << "hrs " << minutes  << " mins " << gameTime << " secs" << endl;

						timeText.str( "" );
						timeText << "TIME: " << hours << ":" << minutes  << ":" << gameTime%60;

						if( !timerTexture.loadFromRenderedText( timeText.str().c_str(), textColor ) )
						{
							printf( "Unable to render time texture!\n" );
						}

						timerTexture.render(SCREEN_WIDTH - timerTexture.getWidth() , 0 );

						SDL_Rect cashRect = {SCREEN_WIDTH/2, 0, 60, 60};

						SDL_RenderCopy(gRenderer,cashTexture.getTexture(), NULL, &cashRect);

						cashText.str("");
						cashText << boy.cash;

						if( !cashTextTexture.loadFromRenderedText( cashText.str().c_str(), textColor ) )
						{
							printf( "Unable to render cash texture!\n" );
						}
						cashTextTexture.render(SCREEN_WIDTH/2 + 70, 0);

						if (gameTime >= 86400){
							timer.stop();
						}

						if (checkCollision(boy.getCharRect(), prof1.getBox()) && (gameTime - previousCollision1 >= 20) && (boy.health >= 5) && (boy.state == 0)){
							boy.health -= 5;
							previousCollision1 = gameTime;
							boy.cash += 10;
						}

						if (checkCollision(boy.getCharRect(), prof2.getBox()) && (gameTime - previousCollision2 >= 20) && (boy.health >= 5) && (boy.state == 0)){
							boy.health -= 5;
							previousCollision2 = gameTime;
							boy.cash += 10;
						}

						if (checkCollision(boy.getCharRect(), prof3.getBox()) && (gameTime - previousCollision3 >= 20) && (boy.health >= 5) && (boy.state == 0)){
							boy.health -= 5;
							previousCollision3 = gameTime;
							boy.cash += 10;
						}

						if (checkCollision(boy.getCharRect(), prof4.getBox()) && (gameTime - previousCollision4 >= 20) && (boy.health >= 5) && (boy.state == 0)){
							boy.health -= 5;
							previousCollision4 = gameTime;
							boy.cash += 10;
						}

						if (checkCollision(boy.getCharRect(), prof5.getBox()) && (gameTime - previousCollision5 >= 20) && (boy.health >= 5) && (boy.state == 0) ){
							boy.health -= 5;
							previousCollision5 = gameTime;
							boy.cash += 10;
						}

						if (checkCollision(boy.getCharRect(), nurse.getBox()) && (gameTime - previousCollisionNurse >= 120) && (boy.health <=  90) && (boy.state == 0)){
							boy.health += 10;
							previousCollisionNurse = gameTime;
						}

						if (checkCollision(boy.getCharRect(), cycleWala.getBox()) && boy.health >= 80){
							boy.state = 1;
						}
						else if (checkCollision(boy.getCharRect(), cycleWala.getBox()) && boy.health < 80){
							promptTexture1.render(SCREEN_WIDTH/2 - promptTexture1.getWidth()/2, SCREEN_HEIGHT/2 - promptTexture1.getHeight()/2);
						}

						if (checkCollision(boy.getCharRect(), chefShiru.getBox()) && (gameTime - previousCollisionShiru >= 20) && (boy.cash >= 20 && boy.health <= 90 && boy.state == 0)){
							boy.health += 20;
							boy.cash -= 20;
						}

						if (checkCollision(boy.getCharRect(), chefAmul.getBox()) && (gameTime - previousCollisionAmul >= 20)  && (boy.cash >= 20 && boy.health <= 90 && boy.state == 0)){
							boy.health += 20;
							boy.cash -= 20;
						}

						if (checkCollision(boy.getCharRect(), chefRajdhani.getBox()) && (gameTime - previousCollisionRajdhani >= 20) && (boy.cash >= 20 && boy.health <= 90 && boy.state == 0)){
							boy.health += 20;
							boy.cash -= 20;
						}

						if (checkCollision(boy.getCharRect(), chefChayos.getBox()) && (gameTime - previousCollisionChayos >= 20) && (boy.cash >= 20 && boy.health <= 90 && boy.state == 0)){
							boy.health += 20;
							boy.cash -= 20;
						}

						if (checkCollision(boy.getCharRect(), chefMasalaMix.getBox()) && (gameTime - previousCollisionMasalaMix >= 20) && (boy.cash >= 20 && boy.health <= 90 && boy.state == 0)){
							boy.health += 20;
							boy.cash -= 20;
						}

						boy.checkStats();

						if (winner) {
							if (boy.id == winner){
								winnerScreen.render(0,0);
							}
							else{
								loserScreen.render(0,0);
							}
						}						

					}
					else if (gameEnded){
						cout << winner << endl;
						if (winner == 1){
							// you won wali screen
							winnerScreen.render(0,0);
						}
						else{
							// you lost
							loserScreen.render(0,0);
						}

					}



					//Update screen
					SDL_RenderPresent( gRenderer );


				}
			}

			SDLNet_TCP_Close(client);

		}
		SDLNet_Quit();
		//Free resources and close SDL
		close( tileSet );
	}

	return 0;
}