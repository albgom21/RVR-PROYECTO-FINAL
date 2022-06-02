#include "GameServer.h"
#include <thread>

int main(int argc, char** argv) {

	if (argc != 3) { //               0.0.0.0     2222    
		std::cout << "Arguments: [1] Address [2] Port\n";
		return 0;
	}
	// Creación del servidor.
	GameServer gs(argv[1], argv[2]);
	// Thread para crear mensajes.
	std::thread([&gs]() { gs.do_messages(); }).detach();

	// Bucle del servidor para mover las balas y comprobar las colisiones.
	while (!gs.fin)
	{
		gs.move_bullets();
		gs.collisions();
	}

	return 0;
}