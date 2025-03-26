#include "Saw.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

const int LEFT_BOUNDARY = 193;
const int RIGHT_BOUNDARY = 728;
const int GROUND_LEVEL = 550;
const int TOP_BOUNDARY = 61;
const float SPEED = 5.0f;

Saw::Saw(SDL_Renderer* renderer, int x, int y, float angle)
    : renderer(renderer), vx(SPEED * cos(angle)), vy(SPEED * sin(angle)),
      currentFrame(0), lastFrameTime(0) {
    texture = loadTexture("images/saw.png");
    rect = {x, y, DISPLAY_SIZE, DISPLAY_SIZE};
    setupAnimation();
}

Saw::~Saw() {
    SDL_DestroyTexture(texture);
}

SDL_Texture* Saw::loadTexture(const char* path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) std::cout << "Không thể tải " << path << "\n";
    return texture;
}

void Saw::setupAnimation() {
    int frameWidth = SAW_SIZE;
    int frameHeight = SAW_SIZE;
    for (int i = 0; i < FRAME_COUNT; i++) {
        clips[i] = {i * frameWidth, 0, frameWidth, frameHeight};
    }
}

void Saw::update() {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime > lastFrameTime + FRAME_DELAY) {
        currentFrame = (currentFrame + 1) % FRAME_COUNT;
        lastFrameTime = currentTime;
    }

    rect.x += vx;
    rect.y += vy;

    float centerX = rect.x + SAW_RADIUS;
    float centerY = rect.y + SAW_RADIUS;

    if (centerX - SAW_RADIUS <= LEFT_BOUNDARY || centerX + SAW_RADIUS >= RIGHT_BOUNDARY) {
        vx = -vx;
    }

    if (centerY + SAW_RADIUS >= GROUND_LEVEL) {
        rect.y = GROUND_LEVEL - DISPLAY_SIZE;
        vy = -vy;
    }

    if (centerY - SAW_RADIUS < TOP_BOUNDARY) {
        float angle = (rand() % 31 + 30) * M_PI / 180;
        rect = {rand() % (RIGHT_BOUNDARY - LEFT_BOUNDARY) + LEFT_BOUNDARY, TOP_BOUNDARY, DISPLAY_SIZE, DISPLAY_SIZE};
        vx = SPEED * cos(angle);
        vy = SPEED * sin(angle);
    }
}

void Saw::render() {
    SDL_Rect scaledRect = {rect.x, rect.y, DISPLAY_SIZE, DISPLAY_SIZE};
    SDL_RenderCopy(renderer, texture, &clips[currentFrame], &scaledRect);
}

std::vector<Saw> Saw::createSaws(SDL_Renderer* renderer) {
    std::vector<Saw> saws;
    srand(static_cast<unsigned>(time(0)));
    int numSaws = rand() % 5 + 3;
    for (int i = 0; i < numSaws; i++) {
        float angle = (rand() % 31 + 30) * M_PI / 180;
        int x = rand() % (RIGHT_BOUNDARY - LEFT_BOUNDARY) + LEFT_BOUNDARY;
        int y = rand() % 200 + 50;
        saws.emplace_back(renderer, x, y, angle);
    }
    return saws;
}