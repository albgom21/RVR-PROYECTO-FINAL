#pragma once
#include <SDL2/SDL.h>
//#include "TextureManager.h"


class Game {
private:
	
	SDL_Window* window_ = nullptr;
	SDL_Renderer* renderer_ = nullptr;
	
	//TextureManager* textureManager_ = nullptr;

    void initSDL();
	void initResources();

	Game();

	static Game* instance;

public:
  
  	static Game* GetInstance();

	Game(Game &other) = delete;
    void operator=(const Game &) = delete;
    
    ~Game();

	SDL_Window* getWindow();
	SDL_Renderer* getRenderer();
	//TextureManager* getTextureManager();

	void destroyWindow();
  
	static const int winWidth_ = 800;
	static const int winHeight_ = 800; 
};