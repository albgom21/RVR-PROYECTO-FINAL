#include <SDL2/SDL_image.h>
#include <string>
using namespace std;

class Texture {
private:
	SDL_Texture* texture_;
	SDL_Renderer* renderer_;
	int width_;
	int height_;

public:
	Texture();
	Texture(SDL_Renderer* renderer, const string& fileName);
	Texture(SDL_Renderer* renderer, SDL_Surface* src, SDL_Rect* srcRect, SDL_Surface* dest, SDL_Rect* destRect);

	virtual ~Texture();

	inline int getWidth() {
		return width_;
	}

	inline int getHeight() {
		return height_;
	}

	inline bool isReady() {
		return texture_ != nullptr;
	}

	bool loadFromImg(SDL_Renderer* renderer, const string& fileName);

	bool loadFromSurface(SDL_Renderer* renderer, SDL_Surface* src, SDL_Rect* srcRect, SDL_Surface* dest, SDL_Rect* destRect);

	void render(int x, int y) const;

	void render(const SDL_Rect& dest) const;
	void render(const SDL_Rect& dest, const SDL_Rect& frame) const;

	void render(const SDL_Rect& dest, double angle, const SDL_Rect& frame) const;
	void render(const SDL_Rect& dest, double angle) const;

	SDL_Texture* getSDLTex() { return texture_; }

	void close();
};