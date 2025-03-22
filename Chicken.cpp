#include "Chicken.h"
#include <iostream>

const int CHICKEN_SPEED = 5;
const int GRAVITY = 1;  // Trọng Lực
const int FRAME_DELAY = 5;
const int BASE_JUMP_FORCE = -10;  // Lực nhảy cơ bản lần đầu
const int JUMP_LIMIT = 2;         // Giới hạn số lần nhảy
const int MAX_JUMP_CHARGE = -18;  // Lực nhảy tối đa lần đầu khi giữ lâu
const int CHARGE_RATE = 2;        // Tốc độ tích lực nhảy khi giữ phím
const int MAX_CHARGE_TIME = 15;   // Giới hạn thời gian tích lực (30 frames ~ 0.5 giây ở 60 FPS)
const float SECOND_JUMP_FACTOR = 0.7f;  // Lực nhảy lần 2 bằng 70% lần 1
const int SCREEN_WIDTH = 800; // Chiều rộng màn hình
const int CHICKEN_WIDTH = 200; 

Chicken::Chicken(SDL_Renderer* renderer)
    : renderer(renderer), velocityY(0), isJumping(false), facingLeft(false),
      frame(0), frameCounter(0), jumpTime(0), jumpCount(0), jumpCharge(0), savedJumpForce(0) {
    texture = loadTexture("images/chicken.png");
    chickenRect = {400, 550, 200, 200};
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
        // Xử lý nhảy khi nhấn phím UP
        if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_UP) {
            if (jumpCount < JUMP_LIMIT) {  // Kiểm tra giới hạn nhảy
                if (jumpCount == 0) {
                    // Nhảy lần đầu
                    jumpCharge = BASE_JUMP_FORCE;  // Lực cơ bản lần đầu
                    if (jumpCharge > -10) jumpCharge = -10;  // Nếu lực nhảy lần đầu > -10 thì đặt = -10
                    velocityY = jumpCharge;
                    savedJumpForce = jumpCharge;  // Lưu lực nhảy lần đầu
                } else if (jumpCount == 1) {
                    // Nhảy lần thứ hai bằng 70% lực nhảy lần đầu
                    jumpCharge = static_cast<int>(savedJumpForce * SECOND_JUMP_FACTOR);  
                    if (jumpCharge > MIN_JUMP_FORCE) jumpCharge = MIN_JUMP_FORCE;  // Giới hạn lực nhảy thấp nhất
                    velocityY = jumpCharge;
                }
                isJumping = true;
                jumpTime = 0;  // Reset thời gian tích lực
                jumpCount++;   // Tăng số lần nhảy
            }  
        }
    }

    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    // Di chuyển sang trái và phải
    if (keystate[SDL_SCANCODE_LEFT]) {
        if (chickenRect.x > 0) { // Giới hạn biên trái
            chickenRect.x -= CHICKEN_SPEED;
        }
        facingLeft = true;
    } else if (keystate[SDL_SCANCODE_RIGHT]) {
        if (chickenRect.x + CHICKEN_WIDTH < SCREEN_WIDTH) { // Giới hạn biên phải
            chickenRect.x += CHICKEN_SPEED;
        }
        facingLeft = false;
    }

    // Tích lực nhảy khi giữ phím UP (chỉ cho lần đầu)
    if (keystate[SDL_SCANCODE_UP] && isJumping && jumpCount == 1) {
        if (jumpTime < MAX_CHARGE_TIME) {
            jumpCharge -= CHARGE_RATE;  // Tăng lực nhảy khi giữ phím
            if (jumpCharge < MAX_JUMP_CHARGE) {
                jumpCharge = MAX_JUMP_CHARGE;  // Giới hạn lực tối đa
            }
            if (jumpCharge > MIN_JUMP_FORCE) jumpCharge = MIN_JUMP_FORCE;  // Giới hạn lực nhảy thấp nhất
            velocityY = jumpCharge;  // Cập nhật vận tốc
            savedJumpForce = jumpCharge;  // Cập nhật lực nhảy lần đầu nếu giữ phím
            jumpTime++;              // Tăng thời gian tích lực
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

        // Kiểm tra hạ cánh
        if (chickenRect.y >= 550) {
            chickenRect.y = 550;
            isJumping = false;
            velocityY = 0;
            jumpCount = 0;  // Reset số lần nhảy khi chạm đất
        }
    }
}

void Chicken::render() {
    SDL_Rect* currentClip = &chickenClips[frame];
    SDL_RenderCopyEx(renderer, texture, currentClip, &chickenRect, 0, nullptr, facingLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
}