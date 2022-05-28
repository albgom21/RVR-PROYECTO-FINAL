#include "GameClient.h"
#include "Game.h"
#include "Player.h"
#include "InputHandler.h"
#include "Resources.h" 
#include "Constants.h"
#include "Message.h"
#include "GOInfo.h"

GameClient::GameClient(const char *s, const char *p, const char *n) : socket(s, p){
    _app = Game::GetInstance();

    for (auto &image : Resources::imageRoutes) 
        _app->getTextureManager()->loadFromImg(image.id, _app->getRenderer(), image.route);

    _myPlayer = new Player(_app->getTextureManager()->getTexture(Resources::ID::Pug), 
                    _app->getTextureManager()->getTexture(Resources::ID::Pug),std::strtol(n, NULL, 10));
}

GameClient::~GameClient() {
    delete _myPlayer;
    delete _app;
}

void GameClient::initClient(){
    Message m = Message(MessageType::LOGIN, _myPlayer);
    if (socket.send(m, socket) == -1)
        std::cout << "ERROR: no se pudo enviar el mensaje de LOGIN\n";
}

void GameClient::render(){
    SDL_RenderClear(_app->getRenderer());
    //Jugador
    _myPlayer->getTexture()->render({(int)_myPlayer->getPos().getX(), (int)_myPlayer->getPos().getY(),TAM_JUG, TAM_JUG});

    // //El otro jugador
    // if(players.size() != 0){
    //     Texture *t = _app->getTextureManager()->getTexture(Resources::ID::Pug);
    //     t->render({(int)players[0].pos.getX(), (int)players[0].pos.getY(), TAM_JUG, TAM_JUG});
    // }

    Texture *t = _app->getTextureManager()->getTexture(Resources::ID::Pug);
    for (auto it = players.begin(); it != players.end(); ++it)
    {
        GOInfo p = (*it);
        std::cout << "POS OTRO JUG Y : "<< (int)p.pos.getY()<<"\n";
        t->render({(int)p.pos.getX(), (int)p.pos.getY(), TAM_JUG, TAM_JUG});
    }

        

    SDL_RenderPresent(_app->getRenderer());
}

void GameClient::net_thread()
{
    while (playing)
    {
        Message m;
        socket.recv(m);

        switch (m.getMessageType())
        {
            case MessageType::NEWPLAYER:
            {
                GOInfo p = m.getGOInfo();
                std::cout<< "MENSAJE N: " << p.nJug<< "\n";
                std::cout<< "MYPLAYER N: " << _myPlayer->getNum()<< "\n";
                if (p.nJug != _myPlayer->getNum()){                   
                    players.push_back(p);
                    std::cout<< "LLENADO PLAYER \n";
                }
                else
                {
                    _myPlayer->setPos(p.pos);
                    _myPlayer->setNum(p.nJug);
                }

                break;
            }
            case MessageType::PLAYERPOS:
            {
                GOInfo p = m.getGOInfo();
               // std::cout<<"N PLAYER POS QUE ME LLEGA: "<<m.getGOInfo().nJug<<std::endl;
                players[m.getGOInfo().nJug] = p;
                break;
            }
            case MessageType::PLAYERDEAD:
            {
                if (m.getGOInfo().nJug == _myPlayer->getNum())
                {
                    m.setMsgType(MessageType::LOGOUT);
                    socket.send(m, socket);
                    std::cout << "GAME OVER\n";

                }
                else
                    std::cout << "YOU WIN!\n";


                playing = false;
                for (auto it = players.begin(); it != players.end(); it++)
                    players.erase(it);
                break;
            }
        }
    }
}





void GameClient::input(){
    bool posM = false;
    bool escudoM = false;
    bool balaM = false;
	HandleEvents::instance()->update();
	if(HandleEvents::instance()->isKeyDown(SDL_SCANCODE_W) && (_myPlayer->getPos().getY()-VELOCITY) >= 0){
        _myPlayer->setPos({_myPlayer->getPos().getX(), _myPlayer->getPos().getY() - VELOCITY});
        posM = true;
    }
    else if (HandleEvents::instance()->isKeyDown(SDL_SCANCODE_S) && (_myPlayer->getPos().getY()+VELOCITY+TAM_JUG) <= W_HEIGHT){
        _myPlayer->setPos({_myPlayer->getPos().getX(), _myPlayer->getPos().getY() + VELOCITY});
        posM = true;
    }
    else if (HandleEvents::instance()->isKeyDown(SDL_SCANCODE_X) && _myPlayer->canShield() )
    {   
        _myPlayer->addShield();
        escudoM = true;
    }
    else if (HandleEvents::instance()->isKeyDown(SDL_SCANCODE_SPACE))
    {
        balaM = true;
    }

    //Mandar el mensaje al servidor
    if (posM && playing)
    {
        Message m(MessageType::PLAYERPOS, _myPlayer);
        socket.send(m, socket);
    }
    else if (escudoM && playing)
    {
        Message m(MessageType::ESCUDO, _myPlayer);
        socket.send(m, socket);
    }
    else if (balaM && playing)
    {
        Message m(MessageType::DISPARO, _myPlayer);
        socket.send(m, socket);
    }

}

void GameClient::run(){
    while(playing){
        input();
        render();
    }    
        
}