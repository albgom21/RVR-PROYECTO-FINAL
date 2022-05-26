#include "Chat.h"
#include "Game.h"
#include "GameClient.h"
#include "Resources.h"
#include "InputHandler.h"
#include <iostream> 

using namespace std;

int main(int argc, char **argv) {

    GameClient* _gc = new GameClient();
    _gc->run();
    // es.do_messages();

    return 0;
}
