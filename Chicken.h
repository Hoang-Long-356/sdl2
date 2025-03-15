#ifndef CHICKEN_H
#define CHICKEN_H

#include <SDL.h>
#include <SDL_image.h>

class Chicken {
public:
    Chicken(SDL_Renderer* renderer);
    ~Chicken();
    
    void update();
    bool handleInput();
    void render();

private:
    SDL_Rect chickenClips[3]; 
    void setupAnimation();
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect chickenRect;
    int velocityY;
    bool isJumping;
    bool facingLeft;
    int frame;
    int frameCounter;
    int jumpTime;
    int jumpCount;
    int firstJumpForce;

    SDL_Texture* loadTexture(const char* path);
};

#endif
