#include "Texture.h"
#include <SDL2/SDL_image.h>
#include <iostream>
using namespace std;

Texture::Texture() : texture_(nullptr), renderer_(nullptr), width_(0), height_(0) {
}

Texture::Texture(SDL_Renderer* renderer, const string& fileName) : texture_(nullptr), width_(0), height_(0) {
	loadFromImg(renderer, fileName);
}


Texture::Texture(SDL_Renderer* renderer, SDL_Surface* src, SDL_Rect* srcRect, SDL_Surface* dest, SDL_Rect* destRect) :
	texture_(nullptr), width_(0), height_(0) {
	loadFromSurface(renderer, src, srcRect, dest, destRect);
}

Texture::~Texture() {
	close();
}

void Texture::close() {
	if (texture_ != nullptr) {
		SDL_DestroyTexture(texture_);
		texture_ = nullptr;
		width_ = 0;
		height_ = 0;
	}
}

bool Texture::loadFromImg(SDL_Renderer* renderer, const string& fileName) {
	SDL_Surface* surface = IMG_Load(fileName.c_str());
	if (surface != nullptr) {
		close();
		texture_ = SDL_CreateTextureFromSurface(renderer, surface);
		if (texture_ != nullptr) {
			width_ = surface->w;
			height_ = surface->h;
		}
		SDL_FreeSurface(surface);
	}
	else {
		throw "No se puede cargar la imagen: " + fileName;
	}
	renderer_ = renderer;
	return texture_ != nullptr;
}

bool Texture::loadFromSurface(SDL_Renderer* renderer, SDL_Surface* src, SDL_Rect* srcRect, SDL_Surface* dest, SDL_Rect* destRect)
{
	if (SDL_BlitSurface(src, srcRect, dest, destRect) == 0) {
		if (dest != nullptr) {
			close();
			texture_ = SDL_CreateTextureFromSurface(renderer, dest);
			if (texture_ != nullptr) {
				width_ = dest->w;
				height_ = dest->h;
			}
			SDL_FreeSurface(dest);
		}
		else {
			throw "No se ha podido hacer BlitSurface";
		}
	}
	renderer_ = renderer;
	return texture_ != nullptr;
}

void Texture::render(int x, int y) const {
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = width_;
	dest.h = height_;
	render(dest);
}

void Texture::render(const SDL_Rect& dest, const SDL_Rect& frame) const {
	if (texture_) {
		SDL_RenderCopy(renderer_, texture_, &frame, &dest);
	}
	else std::cout << "NO hay textura\n";
}

void Texture::render(const SDL_Rect& dest) const {
	SDL_Rect frame = { 0, 0, width_, height_ };
	render(dest, frame);
}


void Texture::render(const SDL_Rect& dest, double angle, const SDL_Rect& frame) const {
	if (texture_) {
		SDL_RenderCopyEx(renderer_, texture_, &frame, &dest, angle, nullptr, SDL_FLIP_NONE);
	}
}

void Texture::render(const SDL_Rect& dest, double angle) const {
	SDL_Rect frame = { 0, 0, width_, height_ };
	render(dest, angle, frame);
}