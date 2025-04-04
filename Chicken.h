#ifndef CHICKEN_H
#define CHICKEN_H

#include <SDL.h>
#include <SDL_image.h>

class Chicken {
public:
    Chicken(SDL_Renderer* renderer);
    ~Chicken();
    
    void update();
    bool handleInput(const SDL_Event& event);
    void render();
    void resetPosition(); // Thêm phương thức để đặt lại vị trí gà

    static const int MIN_JUMP_FORCE = -10;
    static const int GRAVITY = 1;
    static const int JUMP_LIMIT = 2;
    static const int MAX_JUMP_CHARGE = -14;
    static const int CHARGE_RATE = 1;
    static const int MAX_CHARGE_TIME = 15;
    static constexpr float SECOND_JUMP_FACTOR = 0.9f;
    static const int CHICKEN_SPEED = 5;
    static const int SCREEN_WIDTH = 900;
    static const int LEFT_BOUNDARY = 250;
    static const int RIGHT_BOUNDARY = 651;
    static const int GROUND_LEVEL = 528;
    static const int TOP_BOUNDARY = 60;
    static const int FRAME_DELAY = 5;

    const SDL_Rect& getRect() const { return chickenRect; }
    bool isChickenJumping() const { return isJumping; }
    int getGroundLevel() const { return GROUND_LEVEL; }

private:
    SDL_Rect idleClips[6];
    SDL_Rect runClips[7];
    void setupAnimation();
    SDL_Renderer* renderer;
    SDL_Texture* idleTexture;
    SDL_Texture* runTexture;
    SDL_Texture* currentTexture;
    SDL_Rect chickenRect;
    int velocityY;
    bool isJumping;
    bool facingLeft;
    int frame;
    int frameCounter;
    int jumpTime;
    int jumpCount;
    int jumpCharge;
    int savedJumpForce;
    bool isMoving;

    SDL_Texture* loadTexture(const char* path);
};

#endif