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

            while (it != clients.end() && (*((*it).get()) != *s))
                ++it;

            if (it == clients.end())
                std::cout << "ERROR: el jugador ya estaba desconectado\n";
            else
            {
                clients.erase((it));                      
                Socket *delSock = (*it).release(); 
                delete delSock;                           
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
            int offset = 100;
            if(cm.getGOInfo().nJug == 1) offset = -(-TAM_JUG+100+TAM_SHIELD_X);

            obj.pos = Vector2D( cm.getGOInfo().pos.getX() + offset , cm.getGOInfo().pos.getY());

            shields.push_back(obj);

            nShields++;
            
            Message cm = Message(MessageType::NEWESCUDO, &obj);
            for (auto i = clients.begin(); i != clients.end(); ++i)
                socket.send(cm, (*(*i)));

            break;
        }

         case MessageType::DISPARO:
        {
            GOInfo* obj = new GOInfo();
            int offset = 100;
            if(cm.getGOInfo().nJug == 1) offset*=-1;

        
            obj->pos = Vector2D( cm.getGOInfo().pos.getX() + offset , cm.getGOInfo().pos.getY() + 25);
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
    
    if (SDL_GetTicks() - initTime > TimeTocreate)
    {
        
        double_t x = 0;
        for (auto b : bullets){
            b.second->nJug == 1 ? x = -2 : x = 2;

            if (x<0 && b.second->pos.getX() < 0)
                bulletsDelete.push_back(b.second);
            else if (x>0 && b.second->pos.getX() > W_WIDTH) //SUMARLE EL TAM DE LA BALA
                bulletsDelete.push_back(b.second);
            else {
                b.second->pos.setX(b.second->pos.getX() + x);
                Message msg = Message(MessageType::BALAPOS, b.second);
                for (auto it = clients.begin(); it != clients.end(); ++it)
                    socket.send(msg, *(*it));
            }                
        }
        for (auto o = bulletsDelete.begin(); o != bulletsDelete.end(); ++o){
            std::cout<<"SIZE: "<<bulletsDelete.size();
            
            Message cm = Message(MessageType::BORRABALA, *o);
            
            for (auto it = clients.begin(); it != clients.end(); ++it)
                    socket.send(cm, *(*it));

            bullets.erase((*o)->id);
            bulletsDelete.remove(*o);
            delete *o;
        }
        bulletsDelete.clear();

      
        
        initTime = SDL_GetTicks();
    }
}