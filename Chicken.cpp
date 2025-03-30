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
const int CHICKEN_WIDTH = 200;

const int LEFT_BOUNDARY = 255;
const int RIGHT_BOUNDARY = 670;
const int GROUND_LEVEL = 550;
const int TOP_BOUNDARY = 61;

Chicken::Chicken(SDL_Renderer* renderer)
    : renderer(renderer), velocityY(0), isJumping(false), facingLeft(false),
      frame(0), frameCounter(0), jumpTime(0), jumpCount(0), jumpCharge(0), savedJumpForce(0),
      isMoving(false) {
    idleTexture = loadTexture("images/chicken.png");
    runTexture = loadTexture("images/chickenrun.png");
    currentTexture = idleTexture;
    chickenRect = {SCREEN_WIDTH / 2, GROUND_LEVEL, 60, 63}; // Kích thước ban đầu cho idle (60x63)
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
    // Idle: 6 frame cho chicken.png (3 cột x 2 hàng)
    // Lấy 6 frame: Cột 0, 1, 2 ở hàng 0 và Cột 0, 1, 2 ở hàng 1
    idleClips[0] = {0, 0, 60, 65};      // Cột 0, hàng 0
    idleClips[1] = {60, 0, 60, 65};     // Cột 1, hàng 0
    idleClips[2] = {120, 0, 60, 65};    // Cột 2, hàng 0
    idleClips[3] = {0, 65, 60, 65};     // Cột 0, hàng 1
    idleClips[4] = {60, 65, 60, 65};    // Cột 1, hàng 1
    idleClips[5] = {120, 65, 60, 65};   // Cột 2, hàng 1

    // Run: 7 frame cho chickenrun.png (3 cột x 3 hàng, lấy theo hàng, kích thước 60x57)
    runClips[0] = {0, 0, 60, 57};       // Hàng 0, cột 0
    runClips[1] = {60, 0, 60, 57};      // Hàng 0, cột 1
    runClips[2] = {120, 0, 60, 57};     // Hàng 0, cột 2
    runClips[3] = {0, 57, 60, 57};      // Hàng 1, cột 0
    runClips[4] = {60, 57, 60, 57};     // Hàng 1, cột 1
    runClips[5] = {120, 57, 60, 57};    // Hàng 1, cột 2
    runClips[6] = {0, 114, 60, 57};     // Hàng 2, cột 0
}

bool Chicken::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return false;

        if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_UP) {
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

    if (keystate[SDL_SCANCODE_LEFT]) {
        if (chickenRect.x > LEFT_BOUNDARY) {
            chickenRect.x -= CHICKEN_SPEED;
        }
        facingLeft = true;
        isMoving = true;
    } else if (keystate[SDL_SCANCODE_RIGHT]) {
        if (chickenRect.x + chickenRect.w < RIGHT_BOUNDARY) {
            chickenRect.x += CHICKEN_SPEED;
        }
        facingLeft = false;
        isMoving = true;
    } else {
        isMoving = false;
    }

    // Xử lý chuyển đổi texture trực tiếp giữa idle và run
    if (isMoving && currentTexture != runTexture) {
        currentTexture = runTexture;
        chickenRect.w = 60;
        chickenRect.h = 57; // Chiều cao cho chickenrun.png
        frame = 0;
    } else if (!isMoving && currentTexture != idleTexture) {
        currentTexture = idleTexture;
        chickenRect.w = 60;
        chickenRect.h = 64; // Chiều cao cho chicken.png
        frame = 0;
    }

    // Cập nhật frame animation
    if (isMoving) {
        frameCounter++;
        if (frameCounter >= FRAME_DELAY) {
            frame = (frame + 1) % 7; // 7 frame cho run
            frameCounter = 0;
        }
    } else {
        frameCounter++;
        if (frameCounter >= FRAME_DELAY) {
            frame = (frame + 1) % 6; // 6 frame cho idle
            frameCounter = 0;
        }
    }

    if (keystate[SDL_SCANCODE_UP] && isJumping && jumpCount == 1) {
        if (jumpTime < MAX_CHARGE_TIME) {
            jumpCharge -= CHARGE_RATE;
            if (jumpCharge < MAX_JUMP_CHARGE) jumpCharge = MAX_JUMP_CHARGE;
            if (jumpCharge > MIN_JUMP_FORCE) jumpCharge = MIN_JUMP_FORCE;
            velocityY = jumpCharge;
            savedJumpForce = jumpCharge;
            jumpTime++;
        }
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
        }

        if (chickenRect.y <= TOP_BOUNDARY) {
            chickenRect.y = TOP_BOUNDARY;
            velocityY = 0;
        }
    }
}

void Chicken::render() {
    SDL_Rect* currentClip = (currentTexture == idleTexture) ? &idleClips[frame] : &runClips[frame];
    SDL_RenderCopyEx(renderer, currentTexture, currentClip, &chickenRect, 0, nullptr, facingLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}