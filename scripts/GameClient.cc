#include "GameClient.h"
#include "Game.h"
#include "Player.h"
#include "InputHandler.h"
#include "Resources.h" 

GameClient::GameClient(){
    _app = nullptr;
    _app = Game::GetInstance();

    for (auto &image : Resources::imageRoutes) 
        _app->getTextureManager()->loadFromImg(image.id, _app->getRenderer(), image.route);

    _myPlayer = new Player(_app->getTextureManager()->getTexture(Resources::ID::Pug), _app->getTextureManager()->getTexture(Resources::ID::Pug), Vector2D(100, 400), 0);
    _y = _myPlayer->getPos().getY();

}

void GameClient::initClient(){

}

void GameClient::render(){
    SDL_RenderClear(_app->getRenderer());
    _myPlayer->getTexture()->render({(int)_myPlayer->getPos().getX(), (int)_myPlayer->getPos().getY(), 150, 150});
    SDL_RenderPresent(_app->getRenderer());
}

void GameClient::input(){
	HandleEvents::instance()->update();
	if(HandleEvents::instance()->isKeyDown(SDL_SCANCODE_W) && (_y-5) < 750) 
        _y-=5;
    else if (HandleEvents::instance()->isKeyDown(SDL_SCANCODE_S) && (_y+5) > 50) 
        _y+=5;

    _myPlayer->setPos({_myPlayer->getPos().getX(), _y});
}

void GameClient::run(){
    while(true){
    input();
    render();
    }
        
}