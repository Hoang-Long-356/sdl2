#ifndef CHICKEN_H
#define CHICKEN_H

#include <SDL.h>
#include <SDL_image.h>

class Chicken {
public:
    Chicken(SDL_Renderer* renderer);
    ~Chicken();
    
    void update();
    bool handleInput(const SDL_Event& event); // Chỉ nhận sự kiện từ main.cpp
    void render();

    static const int MIN_JUMP_FORCE = -10;    // Độ cao tối thiểu (lực nhảy tối thiểu)
    static const int GRAVITY = 1;             // Trọng lực
    static const int JUMP_LIMIT = 2;          // Số lần nhảy tối đa
    static const int MAX_JUMP_CHARGE = -14;   // Lực nhảy tối đa (âm lớn hơn)
    static const int CHARGE_RATE = 1;         // Tốc độ tích lũy lực nhảy
    static const int MAX_CHARGE_TIME = 15;    // Thời gian tối đa để charge jump (frame)
    static constexpr float SECOND_JUMP_FACTOR = 0.9f; // Hệ số giảm lực nhảy lần hai
    static const int CHICKEN_SPEED = 5;       // Tốc độ di chuyển
    static const int SCREEN_WIDTH = 900;      // Chiều rộng màn hình
    static const int LEFT_BOUNDARY = 250;     // Ranh giới trái
    static const int RIGHT_BOUNDARY = 651;    // Ranh giới phải
    static const int GROUND_LEVEL = 528;      // Mức mặt đất
    static const int TOP_BOUNDARY = 60;       // Ranh giới trên
    static const int FRAME_DELAY = 5;         // Độ trễ giữa các frame animation

private:
    SDL_Rect idleClips[6];          // 6 frame cho chicken.png (60x65)
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