#include "Texture.h"

SDL_Texture *Texture::Load(const char *path) {
	SDL_Surface *surface = IMG_Load(path);
	if (!surface) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", IMG_GetError());
		exit(1);
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(_renderer, surface);
	SDL_FreeSurface(surface);
	if (!texture) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "%s", SDL_GetError());
		exit(1);
	}
	return texture;
}
