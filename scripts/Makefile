CC=g++
CFLAGS=-g -I.
DEPS = Socket.h Chat.h Game.h TextureManager.h Resources.h Texture.h Vector2D.h InputHandler.h Player.h GameClient.h
OBJ = Socket.o Chat.o Game.o TextureManager.o Resources.o Texture.o Vector2D.o InputHandler.o Player.o GameClient.o
LIBS=-lpthread -lSDL2 -lSDL2_image

%.o: %.cc $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

all: cs cc

cs: $(OBJ) ChatServer.o
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

cc: $(OBJ) ChatClient.o
	g++ -o $@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean

clean:
	rm -f *.o cs cc

