#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <bits/stdc++.h>
#include <fstream>
using namespace std;
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

vector<int> roadTiles{7, 8, 20, 21, 22, 23, 34, 35, 36, 37, 49, 50, 62, 63, 64, 65, 76, 77, 78, 79, 90, 91, 92, 93, 104, 105, 106, 107, 14742};


const int WALKING_ANIMATION_FRAMES = 16;
SDL_Rect gSpriteClips[ WALKING_ANIMATION_FRAMES ];


int frame = 0;

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );
		
		#if defined(SDL_TTF_MAJOR_VERSION)
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Gets image dimensions
		int getWidth();
		int getHeight();

		SDL_Texture* getTexture(){
			return mTexture;
		}

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;

		//Image dimensions
		int mWidth;
		int mHeight;
};

//The tile
class Tile
{
    public:
		//Initializes position and type
		Tile( int x, int y, int tileType );

		//Shows the tile
		void render( SDL_Rect& camera );

		//Get the tile type
		int getType();

		//Get the collision box
		SDL_Rect getBox();

    private:
		//The attributes of the tile
		SDL_Rect mBox;

		//The tile type
		int mType;
};

//The dot that will move around on the screen
class Dot
{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 10;

		//Initializes the variables
		Dot();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot and check collision against tiles
		void move( Tile *tiles[] );

		//Centers the camera over the dot
		void setCamera( SDL_Rect& camera );

		//Shows the dot on the screen
		void render( SDL_Rect& camera );

		SDL_Rect getBox();

    private:
		//Collision box of the dot
		SDL_Rect mBox;

		//The velocity of the dot
		int mVelX, mVelY;
};

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
LTexture charTexture;
LTexture firstImageTexture;
LTexture secondImageTexture;
LTexture thirdImageTexture;
LTexture fourthImageTexture;
LTexture fifthImageTexture;
LTexture sixthImageTexture;
LTexture seventhImageTexture;
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

Dot::Dot()
{
    //Initialize the collision box
    mBox.x = 9290;
    mBox.y = 3520;
	// mBox.x = 0;
	// mBox.y = 0;
	mBox.w = DOT_WIDTH;
	mBox.h = DOT_HEIGHT;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

void Dot::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
    }
}

void Dot::move( Tile *tiles[] )
{
    //Move the dot left or right
    mBox.x += mVelX;

    //If the dot went too far to the left or right or touched a wall
	// || touchesRoad( mBox, tiles ) 
    if( ( mBox.x < 0 ) || ( mBox.x + DOT_WIDTH > LEVEL_WIDTH ) || !touchesRoad(mBox, tiles))
    {
        //move back
        mBox.x -= mVelX;
    }

	// cout << mBox.x << endl;
    //Move the dot up or down
    mBox.y += mVelY;

    //If the dot went too far up or down or touched a wall
	// || touchesRoad( mBox, tiles )
    if( ( mBox.y < 0 ) || ( mBox.y + DOT_HEIGHT > LEVEL_HEIGHT ) || !touchesRoad(mBox, tiles))
    {
        //move back
        mBox.y -= mVelY;
    }
}

void Dot::setCamera( SDL_Rect& camera )
{
	//Center the camera over the dot
	camera.x = ( mBox.x + DOT_WIDTH / 2 ) - SCREEN_WIDTH / 2;
	camera.y = ( mBox.y + DOT_HEIGHT / 2 ) - SCREEN_HEIGHT / 2;

	//Keep the camera in bounds
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

SDL_Rect Dot :: getBox(){

	return mBox;

}


void Dot::render( SDL_Rect& camera )
{
    //Show the dot
	gDotTexture.render( mBox.x - camera.x, mBox.y - camera.y );
}

class Character{
	public:
	SDL_Rect charBox;
	int direction,mVelx,mVely;
	static const int DOT_VEL = 10;
	Character(){
		charBox.x = 9290;
		charBox.y = 3520;
		charBox.w = 60;
		charBox.h = 60;
		direction = 0;
		mVelx = 0;
		mVely = 0;

	}

	void render(SDL_Renderer* gRenderer,SDL_Rect* camera, int frame){
		SDL_Rect newrect = {charBox.x-camera->x,charBox.y-camera->y,charBox.w,charBox.h};


		SDL_RenderCopy(gRenderer,charTexture.getTexture(),&gSpriteClips[direction*4+frame],&newrect);
	}

	void handleEvent(SDL_Event& e){
		if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
		{
			// frame++;

			// if(frame/4 >= WALKING_ANIMATION_FRAMES/4){
			// 	frame = 0;
			// }

			//Adjust the velocity
			switch( e.key.keysym.sym )
			{
				case SDLK_UP: mVely -= DOT_VEL; direction = 1; break;
				case SDLK_DOWN: mVely += DOT_VEL; direction = 0; break;
				case SDLK_LEFT: mVelx -= DOT_VEL; direction = 2; break;
				case SDLK_RIGHT: mVelx += DOT_VEL; direction = 3; break;
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

};

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
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
		gWindow = SDL_CreateWindow( "Player1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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
			}
		}
	}

	return success;
}

bool loadMedia( Tile* tiles[], Tile* tiles2[])
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	if( !gDotTexture.loadFromFile( "assets/dot.bmp" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

	if( !charTexture.loadFromFile("assets/character.bmp")){
		printf( "Failed to load walking animation texture!\n" );
        success = false;
	}

	else{
		//direction down
        gSpriteClips[ 0 ].x =   0;
        gSpriteClips[ 0 ].y =   0;
        gSpriteClips[ 0 ].w =  200;
        gSpriteClips[ 0 ].h =  300;

        gSpriteClips[ 1 ].x =  200;
        gSpriteClips[ 1 ].y =  0;
        gSpriteClips[ 1 ].w =  200;
        gSpriteClips[ 1 ].h = 300;
        
        gSpriteClips[ 2 ].x = 400;
        gSpriteClips[ 2 ].y =   0;
        gSpriteClips[ 2 ].w =  200;
        gSpriteClips[ 2 ].h = 300;

        gSpriteClips[ 3 ].x = 600;
        gSpriteClips[ 3 ].y =   0;
        gSpriteClips[ 3 ].w =  200;
        gSpriteClips[ 3 ].h = 300;

		//dir up
        gSpriteClips[ 4 ].x =   0;
        gSpriteClips[ 4 ].y =   300;
        gSpriteClips[ 4 ].w =  200;
        gSpriteClips[ 4 ].h = 300;

        gSpriteClips[ 5 ].x =  200;
        gSpriteClips[ 5 ].y =   300;
        gSpriteClips[ 5 ].w =  200;
        gSpriteClips[ 5 ].h = 300;
        
        gSpriteClips[ 6 ].x = 400;
        gSpriteClips[ 6 ].y =   300;
        gSpriteClips[ 6 ].w =  200;
        gSpriteClips[ 6 ].h = 300;

        gSpriteClips[ 7 ].x = 600;
        gSpriteClips[ 7 ].y =  300;
        gSpriteClips[ 7 ].w =  200;
        gSpriteClips[ 7 ].h = 300;

		//dir left
        gSpriteClips[ 8 ].x =   0;
        gSpriteClips[ 8 ].y =   600;
        gSpriteClips[ 8 ].w =  200;
        gSpriteClips[ 8 ].h = 300;

        gSpriteClips[ 9 ].x =  200;
        gSpriteClips[ 9 ].y =   600;
        gSpriteClips[ 9 ].w =  200;
        gSpriteClips[ 9 ].h = 300;
        
        gSpriteClips[ 10 ].x = 400;
        gSpriteClips[ 10].y =  600;
        gSpriteClips[ 10 ].w =  200;
        gSpriteClips[ 10 ].h = 300;

        gSpriteClips[ 11 ].x = 600;
        gSpriteClips[ 11 ].y =   600;
        gSpriteClips[ 11 ].w =  200;
        gSpriteClips[ 11 ].h = 300;

		//dir right
        gSpriteClips[ 12 ].x =   0;
        gSpriteClips[ 12 ].y =   900;
        gSpriteClips[ 12 ].w =  200;
        gSpriteClips[ 12 ].h = 300;

        gSpriteClips[ 13 ].x =  200;
        gSpriteClips[ 13 ].y =   900;
        gSpriteClips[ 13 ].w =  200;
        gSpriteClips[ 13 ].h = 300;
        
        gSpriteClips[ 14 ].x = 400;
        gSpriteClips[ 14 ].y =   900;
        gSpriteClips[ 14 ].w =  200;
        gSpriteClips[ 14 ].h = 300;

        gSpriteClips[ 15 ].x = 600;
        gSpriteClips[ 15 ].y =   900;
        gSpriteClips[ 15 ].w =  200;
        gSpriteClips[ 15 ].h = 300;
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
	charTexture.free();


	//Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
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
		int datain[] = {0,0,0,0,0,0}, dataout[] = {0,0,0,0,0,0};

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

				
				
				Character boy;
				Character boy2;

				//Level camera
				SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
				SDL_Rect fromserver;



				//While application is running
				while( !quit )
				{
					//Handle events on queue
					while( SDL_PollEvent( &e ) != 0 )
					{
						//User requests quit
						if( e.type == SDL_QUIT )
						{
							quit = true;
							dataout[0] = -1;
                            SDLNet_TCP_Send(client, dataout, 16);
						}

						//Handle input for the dot
						// dot.handleEvent( e );
						boy.handleEvent(e);

						if(e.type == SDL_KEYDOWN){
							frame++;
							if(frame >= 16){
								frame = 0;
							}
						}
					}

					//Move the dot
					// dot.move( tileSet2 );
					// dot.setCamera( camera );

					boy.move(tileSet2);
					SDL_Rect curpos = boy.getCharRect();
                    dataout[0] = curpos.x;
                    dataout[1] = curpos.y;
                    dataout[2] = curpos.w;
                    dataout[3] = curpos.h;
					dataout[4] = boy.getDirection();
					dataout[5] = frame/4;
                    SDLNet_TCP_Send(client, dataout, 24);
					boy.setCamera(camera);

					//Clear screen
					SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
					SDL_RenderClear( gRenderer );

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

					// cout << "-----" << dot.getBox().x << " " << dot.getBox().y ;
					//Render dot
					//dot.render( camera );
					boy.render(gRenderer, &camera, frame/4);

					SDLNet_TCP_Recv(client,datain,24);
                    fromserver = {datain[0], datain[1], datain[2], datain[3]};


                    if(boy2.checkcolwithchar(camera,fromserver)){
						boy2.changedirection(datain[4]);
                        boy2.render(gRenderer, &camera, datain[5]);
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