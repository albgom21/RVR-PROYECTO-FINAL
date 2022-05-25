#include "Game.h"
#include "iostream"
#include <SDL2/SDL.h>
//#include "Resources.h"

Game* Game::instance = nullptr;

Game::Game(){
    initSDL();
	//initResources();
}

Game::~Game(){
	destroyWindow();
}

void Game::initSDL(){
    int winX, winY; // Pos ventana
	winX = winY = SDL_WINDOWPOS_CENTERED;
	// InicializaciOn del sistema, ventana y renderer
	SDL_Init(SDL_INIT_EVERYTHING);
	window_ = SDL_CreateWindow("PING-INVADERS", winX, winY, winWidth_,
		winHeight_, SDL_WINDOW_SHOWN);
	  

	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
	if (window_ == NULL || renderer_ == NULL) {
		std::cout<< "Game window or renderer was null \n";
	}

}

Game* Game::GetInstance()
{
	if(instance==nullptr){
    	instance = new Game();
    }
    return instance;
}

SDL_Window* Game::getWindow(){
	return window_;
}

SDL_Renderer* Game::getRenderer(){
	return renderer_;
}

// TextureManager* Game::getTextureManager(){
// 	return textureManager_;
// }

// void Game::initResources(){

// 	//Crear e inicializar textureManager
// 	textureManager_ = new TextureManager();
// 	textureManager_->initObject();

// 	//Creacion de las texturas
// 	for (auto& image : Resources::imageRoutes) {
// 		textureManager_->loadFromImg(image.textureId, renderer_, image.filename);
// 	}
// }

void Game::destroyWindow(){
	//Destruimos textureManager
	//delete textureManager_;

    //Destruimos render y window
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);

	renderer_ = nullptr;
	window_ = nullptr;

	//Cerramos SDL
	SDL_Quit();
}