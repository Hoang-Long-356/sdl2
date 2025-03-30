#include "Saw.h"
#include <SDL_image.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

const int SAW_SIZE = 150;
const int DISPLAY_SIZE = 50;
const int FRAME_DELAY = 25;
const int LEFT_BOUNDARY = 254;
const int RIGHT_BOUNDARY = 670;
const int GROUND_LEVEL = 600;
const int TOP_BOUNDARY = 61;

Saw::Saw(SDL_Renderer* renderer)
    : renderer(renderer), currentFrame(0), lastFrameTime(0) {
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(0)));
        seeded = true;
    }
    texture = loadTexture("images/saw.png");
    int x = LEFT_BOUNDARY + rand() % (RIGHT_BOUNDARY - LEFT_BOUNDARY - DISPLAY_SIZE);
    rect = {x, TOP_BOUNDARY, DISPLAY_SIZE, DISPLAY_SIZE};
    float angle = (M_PI / 4) + (rand() % 11) * (M_PI / 180);
    vx = cos(angle) * 4.0f;
    vy = sin(angle) * 4.0f;
    setupAnimation();
}

Saw::~Saw() {
    SDL_DestroyTexture(texture);
}

SDL_Texture* Saw::loadTexture(const char* path) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path);
    if (!newTexture) {
        std::cout << "Không thể tải " << path << ": " << SDL_GetError() << std::endl;
    }
    return newTexture;
}

void Saw::setupAnimation() {
    clips[0] = {0, 0, SAW_SIZE, SAW_SIZE};
    clips[1] = {150, 0, SAW_SIZE, SAW_SIZE};
    clips[2] = {300, 0, SAW_SIZE, SAW_SIZE};
    clips[3] = {0, 150, SAW_SIZE, SAW_SIZE};
    clips[4] = {150, 150, SAW_SIZE, SAW_SIZE};
    clips[5] = {300, 150, SAW_SIZE, SAW_SIZE};
    clips[6] = {0, 300, SAW_SIZE, SAW_SIZE};
    clips[7] = {150, 300, SAW_SIZE, SAW_SIZE};
    clips[8] = {300, 300, SAW_SIZE, SAW_SIZE};
    clips[9] = {0, 450, SAW_SIZE, SAW_SIZE};
    clips[10] = {150, 450, SAW_SIZE, SAW_SIZE};
}

bool Saw::update() {
    static bool hasBounced = false;

    rect.x += static_cast<int>(vx);
    rect.y += static_cast<int>(vy);

    if (rect.x < LEFT_BOUNDARY) {
        rect.x = LEFT_BOUNDARY;
        vx = -vx;
    }
    else if (rect.x + DISPLAY_SIZE > RIGHT_BOUNDARY) {
        rect.x = RIGHT_BOUNDARY - DISPLAY_SIZE;
        vx = -vx;
    }

    if (rect.y + DISPLAY_SIZE > GROUND_LEVEL) {
        rect.y = GROUND_LEVEL - DISPLAY_SIZE;
        vy = -vy;
        hasBounced = true;
    }

    if (hasBounced && rect.y < TOP_BOUNDARY) {
        return true;
    }

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastFrameTime >= FRAME_DELAY) {
        currentFrame = (currentFrame + 1) % FRAME_COUNT;
        lastFrameTime = currentTime;
    }
    return false;
}

void Saw::render() const {
    SDL_RenderCopy(renderer, texture, &clips[currentFrame], &rect);
}

bool Saw::isOffScreen() const {
    return rect.y < TOP_BOUNDARY;
}

void Saw::updateSaws(std::vector<Saw>& saws) {
    for (auto it = saws.begin(); it != saws.end();) {
        if (it->update()) {
            it = saws.erase(it);
        } else {
            ++it;
        }
    }
}

void Saw::renderSaws(const std::vector<Saw>& saws, SDL_Renderer* renderer) {
    for (const auto& saw : saws) {
        saw.render();
    }
}