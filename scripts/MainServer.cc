#include "GameServer.h"
#include <thread>

int main(int argc, char **argv) {

    GameServer gs(argv[1], argv[2]);

    std::thread net_thread([&gs]() { gs.do_messages(); });
        
    while (true)
    {
        if(!gs.fin){
         gs.move_bullets();
         gs.collisions();
        }
    }
    
    return 0;
}