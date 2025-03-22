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

    static const int MIN_JUMP_FORCE = -10;  // Độ cao tối thiểu (lực nhảy tối thiểu)

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
    int jumpCharge;      // Lực nhảy tích lũy khi giữ phím
    int savedJumpForce;  // Lưu lực nhảy lần đầu để dùng cho lần 2

    SDL_Texture* loadTexture(const char* path);
};

#endif