CC=g++
CFLAGS=-g -I.
DEPS = Socket.h Game.h TextureManager.h Resources.h Texture.h Vector2D.h InputHandler.h GameClient.h GameServer.h Message.h
OBJ = Socket.o Game.o TextureManager.o Resources.o Texture.o Vector2D.o InputHandler.o GameClient.o GameServer.o Message.o
LIBS=-lpthread -lSDL2 -lSDL2_image

%.o: %.mc $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

all: ms mc

ms: $(OBJ) MainServer.o
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

mc: $(OBJ) MainClient.o
	g++ -o $@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean

clean:
	rm -f *.o ms mc

