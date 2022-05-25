#include "Chat.h"
#include "Game.h"

int main(int argc, char **argv)
{

    Game* app = nullptr;
    app = Game::GetInstance();
    SDL_Delay(3000);
    // ChatServer es(argv[1], argv[2]);

    // es.do_messages();

    return 0;
}
