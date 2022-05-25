#pragma once
#include <SDL2/SDL.h>
#include "TextureManager.h"
#include "Constants.h"

class SDLApp
{
private:
	///<summary>Variables basicas de SDL</summary>
	SDL_Window* window_ = nullptr;
	SDL_Renderer* renderer_ = nullptr;

	///<summary>Manager que gestiona las texturas </summary>
	TextureManager* textureManager_ = nullptr;

    void initSDL();
	///<summary>Metodo que inicializa los recursos de la app</summary>
	void initResources();


	SDLApp();

	static SDLApp* instance;

public:
  
  	static SDLApp* GetInstance();

	SDLApp(SDLApp &other) = delete;
    void operator=(const SDLApp &) = delete;
    
    ~SDLApp();

	SDL_Window* getWindow();
	SDL_Renderer* getRenderer();
	TextureManager* getTextureManager();

	void destroyWindow();

    ///<summary>Constantes con las dimensiones de la ventana </summary>
	static const int winWidth_ = WINDOW_WIDTH;
	static const int winHeight_ = WINDOW_HEIGHT; 
};