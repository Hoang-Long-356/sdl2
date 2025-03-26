#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>

const int FRAME_COUNT = 3; // Số frame trong sprite sheet

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
    SDL_Texture* bgTexture;
    SDL_Texture* sawTexture; // Sprite sheet chứa 3 frame
    SDL_Rect sawClips[FRAME_COUNT]; // Mảng chứa vị trí frame trong sprite sheet

    std::vector<Saw> saws;
    int currentFrame;
    Uint32 lastFrameTime;

    SDL_Texture* loadTexture(const char* path);
    void setupAnimation(); // Cắt frame từ sprite sheet
};

#endif
