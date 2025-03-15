#include "Chicken.h"
#include <iostream>

const int CHICKEN_SPEED = 5;
const int GRAVITY = 1;
const int FRAME_DELAY = 5;
const int BASE_JUMP_FORCE = -20;  // Tăng lực nhảy
const int JUMP_LIMIT = 2;  // Giới hạn số lần nhảy

Chicken::Chicken(SDL_Renderer* renderer)
    : renderer(renderer), velocityY(0), isJumping(false), facingLeft(false),
      frame(0), frameCounter(0), jumpTime(0), jumpCount(0) {
    texture = loadTexture("images/chicken.png");
    chickenRect = {400, 600, 100, 100};
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
    // Đọc tất cả sự kiện 1 lần
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return false;
    }

    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    // Di chuyển sang trái và phải
    if (keystate[SDL_SCANCODE_LEFT]) {
        chickenRect.x -= CHICKEN_SPEED;
        facingLeft = true;
    } else if (keystate[SDL_SCANCODE_RIGHT]) {
        chickenRect.x += CHICKEN_SPEED;
        facingLeft = false;
    }

    // Xử lý nhảy đôi
    if (keystate[SDL_SCANCODE_UP] && jumpCount < JUMP_LIMIT) {
        if (!isJumping) {
            velocityY = BASE_JUMP_FORCE;  // Lực nhảy ban đầu
            isJumping = true;
        } else if (jumpCount == 1) {
            // Thực hiện nhảy lần thứ hai nếu con gà vẫn đang ở trong không trung
            velocityY = BASE_JUMP_FORCE;
            jumpCount++;  // Tăng số lần nhảy lên
        }
    }

    // Cập nhật hoạt ảnh khi di chuyển
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
        velocityY += GRAVITY;  // Áp dụng trọng lực
        chickenRect.y += velocityY;

        // Kiểm tra xem con gà đã hạ cánh chưa
        if (chickenRect.y >= 600) {
            chickenRect.y = 600;  // Đặt lại vị trí y khi hạ cánh
            isJumping = false;
            velocityY = 0;
            jumpCount = 0;  // Reset jump count sau khi hạ cánh
        }
    }
}

void Chicken::render() {
    SDL_Rect* currentClip = &chickenClips[frame];
    SDL_RenderCopyEx(renderer, texture, currentClip, &chickenRect, 0, nullptr, facingLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}
