#include <thread>
#include "GameClient.h"

int main(int argc, char** argv) {

	if (argc != 4) { //               0.0.0.0    2222        0-1
		std::cout << "Arguments: [1] Address [2] Port [3] NJugador\n";
		return 0;
	}
	// Creación de un cliente.
	GameClient gc(argv[1], argv[2], argv[3]);
	// Thread para recibir mensajes.
	std::thread([&gc]() { gc.net_thread(); }).detach();
	// Login del cliente.
	gc.initClient();
	// Bucle del cliente: input y render.
	gc.run();
	return 0;
}