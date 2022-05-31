#include "GameServer.h"
#include "Message.h"
#include <memory>
#include <ctime>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <list>
#include "Constants.h"


GameServer::GameServer(const char *s, const char *p) : socket(s, p)
{initTime = SDL_GetTicks();}

void GameServer::do_messages()
{
    if (socket.bind() == -1)    
        std::cout << "ERROR: en Bind\n";    

    while (true)
    {
        Message cm;
        Socket *s = nullptr;

        if (socket.recv(cm, s) == -1)
            std::cout << "ERROR: no se pudo recibir el mensaje\n";

        switch (cm.getMessageType())
        {
        case MessageType::LOGIN:
        {
            clients.push_back(std::move(std::make_unique<Socket>(*s)));
            GOInfo n;
            n.nJug = nPlayers;
            std::cout << "NPLAYERS: " << n.nJug <<"\n";
            
            if(nPlayers == 0)
                n.pos = Vector2D(X_INI, Y_INI);
            else
                n.pos = Vector2D(X_INI_2, Y_INI);

            players.push_back(n);

            Message newPlayerConnected = Message();
            newPlayerConnected.setMsgType(MessageType::NEWPLAYER);
            newPlayerConnected.setGOInfo(players[nPlayers]);

            //VER SI SE PUEDE REFACTORIZAR LOS FORs YA QUE SOLO HAY A LO SUMO 2 CLIENTES

            for (auto it = clients.begin(); it != clients.end(); it++){
                  if (*((*it).get()) != *s) 
                    socket.send(newPlayerConnected, *(*it));
            }

            for (auto it = players.begin(); it != players.end(); ++it)
            {
                if ((*it).nJug != cm.getGOInfo().nJug)
                {
                    newPlayerConnected.setGOInfo(*it);
                    socket.send(newPlayerConnected, *s);
                }
            }

            nPlayers++;             
            break;
        }

        case MessageType::LOGOUT:
        {

            auto it = clients.begin();

            while (it != clients.end() && (**it != *s))
                ++it;

            if (it == clients.end())
                std::cout << "El usuario no se encontraba conectado\n";
            else
            {
                std::cout << "Usuario desconectado" << "\n";
                clients.erase(it);    
                fin = true;      
            }
            break;
        }

        case MessageType::PLAYERPOS:
        {
            players[cm.getGOInfo().nJug] = cm.getGOInfo();

            for (auto it = clients.begin(); it != clients.end(); it++)
            {
                if (*((*it).get()) != *s) 
                    socket.send(cm, (*((*it).get())));
            }

            break;
        }

        case MessageType::ESCUDO:
        {
            GOInfo obj;
            int offset = 110;
            if(cm.getGOInfo().nJug == 1) offset = -(-TAM_JUG_X+110+TAM_SHIELD_X);

            obj.pos = Vector2D( cm.getGOInfo().pos.getX() + offset , cm.getGOInfo().pos.getY()+ (TAM_JUG_Y/2) - (TAM_SHIELD_Y/2));
            obj.nJug = cm.getGOInfo().nJug;
            obj.id = nShields;
            shields[nShields] = obj;

            nShields++;
            
            Message cm = Message(MessageType::NEWESCUDO, &obj);
            for (auto i = clients.begin(); i != clients.end(); ++i)
                socket.send(cm, (*(*i)));

            break;
        }

         case MessageType::DISPARO:
        {
            GOInfo* obj = new GOInfo();
            int offset = 50;
            if(cm.getGOInfo().nJug == 1)  offset = -(-TAM_JUG_X+50+TAM_BULLET_X);
        
            obj->pos = Vector2D( cm.getGOInfo().pos.getX() + offset , cm.getGOInfo().pos.getY() + (TAM_JUG_Y/2) - (TAM_BULLET_Y/2));
            obj->nJug = cm.getGOInfo().nJug;
            obj->id = nBullets;
            bullets[nBullets] = obj; 
            Message cm = Message(MessageType::NEWBALA, obj);
            nBullets++;
            for (auto i = clients.begin(); i != clients.end(); ++i)
                socket.send(cm, (*(*i)));

            break;
        }
        default:
            break;
        }

        if(cm.getMessageType() != MessageType::LOGIN)
            delete s;

    }
}

void GameServer::move_bullets(){
    
    if (SDL_GetTicks() - initTime > timeUpdate)
    {
        std::list<GOInfo*> bulletsDelete;
        double_t x = 0;
        for (auto b : bullets){
            b.second->nJug == 1 ? x = -VELOCITY : x = VELOCITY;

            if (x<0 && b.second->pos.getX() < -TAM_BULLET_X)
                bulletsDelete.push_back(b.second);
            else if (x>0 && b.second->pos.getX() > W_WIDTH) 
                bulletsDelete.push_back(b.second);
            else {
                b.second->pos.setX(b.second->pos.getX() + x);
                Message msg = Message(MessageType::BALAPOS, b.second);
                for (auto it = clients.begin(); it != clients.end(); ++it)
                    socket.send(msg, *(*it));
            }                
        }
        for (auto o = bulletsDelete.begin(); o != bulletsDelete.end(); ++o){
            Message msg = Message(MessageType::BORRABALA, *o);
            
            for (auto it = clients.begin(); it != clients.end(); ++it)
                    socket.send(msg, *(*it));

            bullets.erase((*o)->id);
        }
        
        initTime = SDL_GetTicks();
    }
}

void GameServer::collisions()
{
    std::list<GOInfo*> bulletsDelete;
    std::list<GOInfo> shieldsDelete;

    for (auto it = bullets.begin(); it != bullets.end(); ++it)
    {
        for (auto it2 = shields.begin(); it2 != shields.end(); ++it2)
        {
            SDL_Rect a, b;
            GOInfo* bul = (*it).second;
            GOInfo shi = (*it2).second;
            a = {(int)bul->pos.getX(), (int)bul->pos.getY(), TAM_BULLET_X, TAM_BULLET_Y};
            b = {(int)shi.pos.getX(), (int)shi.pos.getY(), TAM_SHIELD_X, TAM_SHIELD_Y};

            if (SDL_HasIntersection(&a, &b))
            {
                //Borrado de escudo y bala
                shieldsDelete.push_back((*it2).second);
                bulletsDelete.push_back((*it).second);
            }
        }

        for (auto it3 = players.begin(); it3 != players.end(); ++it3)
        {
            SDL_Rect a, b;
            GOInfo* bul = (*it).second;
            GOInfo pla = (*it3);
            a = {(int)bul->pos.getX(), (int)bul->pos.getY(), TAM_BULLET_X, TAM_BULLET_Y};
            b = {(int)pla.pos.getX(), (int)pla.pos.getY(), TAM_JUG_X, TAM_JUG_Y};

            if (SDL_HasIntersection(&a, &b))
            {
                //Borrado de jugador y bala
                nPlayerDelete = (*it3).nJug;
                bulletsDelete.push_back((*it).second);
            }
        }
    }

    for (auto o = bulletsDelete.begin(); o != bulletsDelete.end(); ++o){
            Message msg = Message(MessageType::BORRABALA, *o);
            
            for (auto it = clients.begin(); it != clients.end(); ++it)
                    socket.send(msg, *(*it));

            bullets.erase((*o)->id);
        }

    for (auto o = shieldsDelete.begin(); o != shieldsDelete.end(); ++o){
            Message msg = Message(MessageType::BORRAESCUDO, &(*o));
            
            for (auto it = clients.begin(); it != clients.end(); ++it)
                    socket.send(msg, *(*it));

            shields.erase((*o).id);
    }

    for (auto o = players.begin(); o != players.end(); ++o){
        
        if(nPlayerDelete == (*o).nJug){
            Message msg = Message(MessageType::PLAYERDEAD, &(*o));
        
        for (auto it = clients.begin(); it != clients.end(); ++it)
                socket.send(msg, *(*it));
            //players.erase(o);
        }
    }
}