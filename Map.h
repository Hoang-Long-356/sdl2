#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>

struct Saw {
    SDL_Rect rect;
    float vx, vy;
};

class Map {
public:
    Map(SDL_Renderer* renderer);
    ~Map();
    
    void update();
    void render();

private:
    SDL_Renderer* renderer;
    SDL_Texture* sawTexture;
    SDL_Texture* bgTexture;
    std::vector<Saw> saws;  // Dùng vector để lưu lưỡi cưa
    SDL_Rect frameBox;

    SDL_Texture* loadTexture(const char* path);
};

#endif
