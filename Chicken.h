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
    SDL_Rect idleClips[6];          // 6 frame cho chicken.png (60x62)
    SDL_Rect runClips[7];           // 7 frame cho chickenrun.png (60x59)
    void setupAnimation();
    SDL_Renderer* renderer;
    SDL_Texture* idleTexture;       // Texture khi đứng yên
    SDL_Texture* runTexture;        // Texture khi chạy
    SDL_Texture* currentTexture;    // Texture hiện tại
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
    bool isMoving;       // Trạng thái di chuyển

    SDL_Texture* loadTexture(const char* path);
};

#endif