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

    _myPlayer = new Player(_app->getTextureManager()->getTexture(Resources::ID::P1),std::strtol(n, NULL, 10));
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

    //Fondo
    Texture *t = _app->getTextureManager()->getTexture(Resources::ID::BACKGROUND);
    t->render({0, 0, W_WIDTH, W_HEIGHT});

    //Jugador
    _myPlayer->getTexture()->render({(int)_myPlayer->getPos().getX(), (int)_myPlayer->getPos().getY(),TAM_JUG, TAM_JUG});

    //Otro jugador cuando se conecte
    if(other){
         t = _app->getTextureManager()->getTexture(Resources::ID::P2);
         t->render({(int)_otherPlayer.pos.getX(), (int)_otherPlayer.pos.getY(), TAM_JUG, TAM_JUG});
    }

    // Escudo
    t = _app->getTextureManager()->getTexture(Resources::ID::SHIELD);
    for (auto it = shields.begin(); it != shields.end(); ++it)
    {
        GOInfo s = (*it);
        t->render({(int)s.pos.getX(), (int)s.pos.getY(), TAM_SHIELD_X, TAM_SHIELD_Y});
    }

    // Balas
   // t = _app->getTextureManager()->getTexture(Resources::ID::BULLET);
   // for (auto it = bullets.begin(); it != bullets.end(); ++it){
   //     GOInfo b = (*it->second);
   //     t->render({(int)b.pos.getX(), (int)b.pos.getY(), TAM_SHIELD_Y, TAM_SHIELD_X});
   // }

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
                //std::cout<< "MENSAJE N: " << p.nJug<< "\n";
                //std::cout<< "MYPLAYER N: " << _myPlayer->getNum()<< "\n";
                if (p.nJug != _myPlayer->getNum()){                   
                    _otherPlayer = p;
                    other = true;
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
                _otherPlayer = p;

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

                break;
            }
            case MessageType::NEWESCUDO:
            {
                GOInfo s = m.getGOInfo();
                shields.push_back(s);
                break;
            }
            /*
            case MessageType::NEWBALA:
            {
                GOInfo* b = new GOInfo();
                b->nJug = m.getGOInfo().nJug;
                b->pos = m.getGOInfo().pos;
                //bullets[b->id] = b;
                break;
            }
            case MessageType::BALAPOS:
            {
                GOInfo bm = m.getGOInfo();
               // bullets[bm.id] = &bm;
            }
            */
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