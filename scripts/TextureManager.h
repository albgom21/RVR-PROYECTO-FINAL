#pragma once
#include "Texture.h"
#include <map>


class TextureManager
{
public:
	TextureManager();
	virtual ~TextureManager();

	bool initObject();

	Texture* getTexture(std::size_t tag) { return textures_[tag]; }

	bool loadFromImg(std::size_t, SDL_Renderer* renderer, const string& fileName);

private:
	void storeTexture(std::size_t tag, Texture* texture);

	bool initialized_ = false;
	map<std::size_t, Texture*> textures_;
};
