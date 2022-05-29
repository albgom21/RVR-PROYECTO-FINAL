#include "GameServer.h"
#include "Message.h"
#include <memory>
#include <ctime>
#include <cstdlib>
#include <SDL2/SDL.h>
#include <list>
#include "Constants.h"

GameServer::GameServer(const char *s, const char *p) : socket(s, p)
{}

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
            if(cm.getGOInfo().nJug == 1) offset*=-1;

            obj.pos = Vector2D( cm.getGOInfo().pos.getX() + offset , cm.getGOInfo().pos.getY());

            shields.push_back(obj);

            nShields++;
            
            Message cm;
            cm.setMsgType(MessageType::NEWESCUDO);
            cm.setGOInfo((obj));
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