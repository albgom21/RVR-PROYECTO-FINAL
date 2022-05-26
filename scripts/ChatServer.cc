#include "Chat.h"
#include "Game.h"
#include "Resources.h"
#include "InputHandler.h"
#include <iostream> 

using namespace std;

int main(int argc, char **argv) {

    Game* app = nullptr;
    app = Game::GetInstance();
   
    //Creacion de las texturas
    for (auto &image : Resources::imageRoutes)
    {
        app->getTextureManager()->loadFromImg(image.id, app->getRenderer(), image.route);
    }
    Texture* background = app->getTextureManager()->getTexture(Resources::ID::Pug);
    SDL_RenderClear(app->getRenderer());
    background->render({0, 0, app->winWidth_, app->winHeight_}, SDL_FLIP_NONE);
    SDL_RenderPresent(app->getRenderer());
    while (true){
        app->run();
    }

    // es.do_messages();

    return 0;
}
