#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <SDL_image.h>

class Map {
public:
    Map(SDL_Renderer* renderer);
    ~Map();
    
    void update();
    void render();

private:
    SDL_Renderer* renderer;
    SDL_Texture* sawTexture;
    SDL_Texture* bgTexture;  // Thêm background
    SDL_Rect saws[5];
    SDL_Rect frameBox;  // Thêm khung viền

    SDL_Texture* loadTexture(const char* path);
};

#endif
