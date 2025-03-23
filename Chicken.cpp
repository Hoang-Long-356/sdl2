#include "Chicken.h"
#include <iostream>

const int CHICKEN_SPEED = 5;
const int GRAVITY = 1;
const int FRAME_DELAY = 5;
const int JUMP_LIMIT = 2;
const int MAX_JUMP_CHARGE = -17;
const int CHARGE_RATE = 1;
const int MAX_CHARGE_TIME = 15;
const float SECOND_JUMP_FACTOR = 0.9f;
const int SCREEN_WIDTH = 900;
const int CHICKEN_WIDTH = 200;
const int MIN_JUMP_FORCE = -7;

const int LEFT_BOUNDARY = 193;
const int RIGHT_BOUNDARY = 728;
const int GROUND_LEVEL = 505;
const int TOP_BOUNDARY = 61;

Chicken::Chicken(SDL_Renderer* renderer)
    : renderer(renderer), velocityY(0), isJumping(false), facingLeft(false),
      frame(0), frameCounter(0), jumpTime(0), jumpCount(0), jumpCharge(0), savedJumpForce(0) {
    texture = loadTexture("images/chicken.png");
    chickenRect = {SCREEN_WIDTH/2, GROUND_LEVEL, 200, 200};
    setupAnimation();
}

Chicken::~Chicken() {
    SDL_DestroyTexture(texture);
}

SDL_Texture* Chicken::loadTexture(const char* path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) std::cout << "Failed to load " << path << "\n";
    return texture;
}

void Chicken::setupAnimation() {
    int frameWidth = 200;
    int frameHeight = 200;
    for (int i = 0; i < 3; i++) {
        chickenClips[i] = {i * frameWidth, 0, frameWidth, frameHeight};
    }
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
    } else if (keystate[SDL_SCANCODE_RIGHT]) {
        if (chickenRect.x + CHICKEN_WIDTH < RIGHT_BOUNDARY) {
            chickenRect.x += CHICKEN_SPEED;
        }
        facingLeft = false;
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

    if (keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_RIGHT]) {
        frameCounter++;
        if (frameCounter >= FRAME_DELAY) {
            frame = (frame + 1) % 3;
            frameCounter = 0;
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
    SDL_Rect* currentClip = &chickenClips[frame];
    SDL_RenderCopyEx(renderer, texture, currentClip, &chickenRect, 0, nullptr, facingLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}
