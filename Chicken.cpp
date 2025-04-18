#include "Chicken.h"
#include <iostream>

const int CHICKEN_SPEED = 5;
const int GRAVITY = 1;
const int FRAME_DELAY = 5;
const int JUMP_LIMIT = 2;
const int MAX_JUMP_CHARGE = -14;
const int CHARGE_RATE = 1;
const int MAX_CHARGE_TIME = 15;
const float SECOND_JUMP_FACTOR = 0.9f;
const int SCREEN_WIDTH = 900;

const int LEFT_BOUNDARY = 250;
const int RIGHT_BOUNDARY = 651;
const int GROUND_LEVEL = 528;
const int TOP_BOUNDARY = 60;

Chicken::Chicken(SDL_Renderer* renderer)
    : renderer(renderer), velocityY(0), isJumping(false), facingLeft(false),
      frame(0), frameCounter(0), jumpTime(0), jumpCount(0), jumpCharge(0), savedJumpForce(0),
      isMoving(false) {
    idleTexture = loadTexture("images/chicken.png");
    runTexture = loadTexture("images/chickenrun.png");
    currentTexture = idleTexture;
    chickenRect = {SCREEN_WIDTH / 2 - 500, GROUND_LEVEL, 60, 63}; // Kích thước ban đầu cho idle (60x63)
    setupAnimation();
}

Chicken::~Chicken() {
    SDL_DestroyTexture(idleTexture);
    SDL_DestroyTexture(runTexture);
}

SDL_Texture* Chicken::loadTexture(const char* path) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path);
    if (!newTexture) std::cout << "Không thể tải " << path << "\n";
    return newTexture;                          
}

void Chicken::setupAnimation() {
    idleClips[0] = {0, 0, 60, 65};
    idleClips[1] = {60, 0, 60, 65};
    idleClips[2] = {120, 0, 60, 65};
    idleClips[3] = {0, 65, 60, 65};
    idleClips[4] = {60, 65, 60, 65};
    idleClips[5] = {120, 65, 60, 65};

    runClips[0] = {0, 0, 60, 57};
    runClips[1] = {60, 0, 60, 57};
    runClips[2] = {120, 0, 60, 57};
    runClips[3] = {0, 57, 60, 57};
    runClips[4] = {60, 57, 60, 57};
    runClips[5] = {120, 57, 60, 57};
    runClips[6] = {0, 114, 60, 57};
}

bool Chicken::handleInput(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
            if (jumpCount < JUMP_LIMIT) {
                if (jumpCount == 0) {
                    jumpCharge = MIN_JUMP_FORCE;
                    velocityY = jumpCharge;
                    savedJumpForce = jumpCharge;
                } else if (jumpCount == 1) {
                    jumpCharge = static_cast<int>(savedJumpForce * SECOND_JUMP_FACTOR);
                    velocityY = jumpCharge;
                }
                isJumping = true;
                jumpTime = 0;
                jumpCount++;
            }
        }
    }

    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_UP] && isJumping && jumpCount == 1) {
        if (jumpTime < MAX_CHARGE_TIME) {
            jumpCharge -= CHARGE_RATE;
            if (jumpCharge < MAX_JUMP_CHARGE) jumpCharge = MAX_JUMP_CHARGE;
            velocityY = jumpCharge;
            savedJumpForce = jumpCharge;
            jumpTime++;
        }
    }

    bool wasMoving = isMoving;
    isMoving = keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_RIGHT];

    if (keystate[SDL_SCANCODE_LEFT] && chickenRect.x > LEFT_BOUNDARY) {
        facingLeft = true;
    } else if (keystate[SDL_SCANCODE_RIGHT] && chickenRect.x + chickenRect.w < RIGHT_BOUNDARY) {
        facingLeft = false;
    }

    if (isMoving && currentTexture != runTexture) {
        currentTexture = runTexture;
        chickenRect.w = 60;
        chickenRect.h = 57;
        frame = 0;
    } else if (!isMoving && currentTexture != idleTexture) {
        currentTexture = idleTexture;
        chickenRect.w = 60;
        chickenRect.h = 65;
        frame = 0;
    }

    frameCounter++;
    if (frameCounter >= FRAME_DELAY) {
        frame = (frame + 1) % (isMoving ? 7 : 6);
        frameCounter = 0;
    }

    return true;
}

void Chicken::update() {
    if (isJumping) {
        velocityY += GRAVITY;
        chickenRect.y += velocityY;

        if (chickenRect.y >= GROUND_LEVEL) {
            chickenRect.y = GROUND_LEVEL;
            isJumping = false;
            velocityY = 0;
            jumpCount = 0;
            jumpTime = 0;
        }

        if (chickenRect.y <= TOP_BOUNDARY) {
            chickenRect.y = TOP_BOUNDARY;
            velocityY = 0;
        }
    }

    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_LEFT] && chickenRect.x > LEFT_BOUNDARY) {
        chickenRect.x -= CHICKEN_SPEED;
    } else if (keystate[SDL_SCANCODE_RIGHT] && chickenRect.x + chickenRect.w < RIGHT_BOUNDARY) {
        chickenRect.x += CHICKEN_SPEED;
    }
}

void Chicken::render() {
    SDL_Rect* currentClip = (currentTexture == idleTexture) ? &idleClips[frame] : &runClips[frame];
    SDL_RenderCopyEx(renderer, currentTexture, currentClip, &chickenRect, 0, nullptr, facingLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}

void Chicken::resetPosition() {
    chickenRect.x = SCREEN_WIDTH / 2; // Đặt lại vị trí giữa màn hình
    chickenRect.y = GROUND_LEVEL;     // Đặt lại trên mặt đất
    velocityY = 0;                    // Đặt lại vận tốc
    isJumping = false;                // Không nhảy
    jumpCount = 0;                    // Reset số lần nhảy
    jumpTime = 0;                     // Reset thời gian nhảy
    facingLeft = false;               // Hướng mặt mặc định
}