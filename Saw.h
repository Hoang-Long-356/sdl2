#ifndef SAW_H
#define SAW_H

#include <SDL.h>
#include <vector>

class Saw {
public:
    Saw(SDL_Renderer* renderer);
    ~Saw();
    bool update();
    void render() const;
    bool isOffScreen() const;

    static void updateSaws(std::vector<Saw>& saws);
    static void renderSaws(const std::vector<Saw>& saws, SDL_Renderer* renderer);

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect rect;
    float vx, vy;
    int currentFrame;
    Uint32 lastFrameTime;
    static const int FRAME_COUNT = 11;
    SDL_Rect clips[FRAME_COUNT];

    SDL_Texture* loadTexture(const char* path);
    void setupAnimation();
};

#endif