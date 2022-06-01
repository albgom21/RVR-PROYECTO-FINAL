#include "GameServer.h"
#include <thread>

int main(int argc, char **argv) {


    if(argc != 3){                 // 0.0.0.0    2222    
        std::cout << "Arguments: [1] Address [2] Port\n";
        return 0;
    }
    GameServer gs(argv[1], argv[2]);

    std::thread([&gs]() { gs.do_messages(); }).detach();
        
    while (!gs.fin)
    {
        gs.move_bullets();
        gs.collisions();
    }

    return 0;
}