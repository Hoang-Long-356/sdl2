#ifndef SAW_H
#define SAW_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>

const int FRAME_COUNT = 3;
const int SAW_SIZE = 150;
const int DISPLAY_SIZE = 50;
const int FRAME_DELAY = 50;
const float SAW_RADIUS = DISPLAY_SIZE / 2.0f;

class Saw {
public:
    Saw(SDL_Renderer* renderer, int x, int y, float angle);
    ~Saw();

    void update();
    void render();

    SDL_Rect getRect() const { return rect; }

    static std::vector<Saw> createSaws(SDL_Renderer* renderer);

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect clips[FRAME_COUNT];
    SDL_Rect rect;
    float vx, vy;
    int currentFrame;
    Uint32 lastFrameTime;

    SDL_Texture* loadTexture(const char* path);
    void setupAnimation();
};

#endif