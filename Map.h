#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <SDL_image.h>

class Map {
public:
    Map(SDL_Renderer* renderer);
    ~Map();
    
    void render(); // Chỉ vẽ nền

private:
    SDL_Renderer* renderer;
    SDL_Texture* bgTexture;

    SDL_Texture* loadTexture(const char* path);
};

#endif