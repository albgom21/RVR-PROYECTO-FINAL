#include <thread>
#include "GameClient.h"


int main(int argc, char **argv)
{
    GameClient gc(argv[1],argv[2]);
    //std::thread net_thread([&gc](){ gc.net_thread(); });
    std::thread([&gc]() { gc.net_thread(); }).detach();
    gc.initClient();
    gc.run();
    return 0;
}