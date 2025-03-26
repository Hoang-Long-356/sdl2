#include "Map.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 700;
const float SPEED = 5.0f;
const int LEFT_BOUNDARY = 193;
const int RIGHT_BOUNDARY = 728;
const int GROUND_LEVEL = 505;
const int TOP_BOUNDARY = 61;
const int SAW_SIZE = 150;  // Kích thước frame gốc trong sprite sheet (150x150)
const int DISPLAY_SIZE = 50; // Kích thước hiển thị nhỏ hơn trên màn hình
const int FRAME_DELAY = 50;
const float SAW_RADIUS = DISPLAY_SIZE / 2.0f; // Bán kính cho va chạm hình tròn

Map::Map(SDL_Renderer* renderer) : renderer(renderer), currentFrame(0), lastFrameTime(0) {
    sawTexture = loadTexture("images/saw.png");
    if (!sawTexture) {
        std::cout << "Failed to load saw.png\n";
    }

    bgTexture = loadTexture("images/bg.png");
    if (!bgTexture) {
        std::cout << "Failed to load bg.png\n";
    }

    setupAnimation();

    srand(static_cast<unsigned>(time(0)));
    int numSaws = rand() % 5 + 3;
    for (int i = 0; i < numSaws; i++) {
        float angle = (rand() % 31 + 30) * M_PI / 180;
        saws.push_back({
            {rand() % (RIGHT_BOUNDARY - LEFT_BOUNDARY) + LEFT_BOUNDARY, rand() % 200 + 50, DISPLAY_SIZE, DISPLAY_SIZE},
            SPEED * cos(angle),
            SPEED * sin(angle)
        });
    }
}

Map::~Map() {
    SDL_DestroyTexture(sawTexture);
    SDL_DestroyTexture(bgTexture);
}

SDL_Texture* Map::loadTexture(const char* path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) std::cout << "Failed to load " << path << "\n";
    return texture;
}

void Map::setupAnimation() {
    int frameWidth = SAW_SIZE;
    int frameHeight = SAW_SIZE;
    for (int i = 0; i < FRAME_COUNT; i++) {
        sawClips[i] = {i * frameWidth, 0, frameWidth, frameHeight};
    }
}

void Map::update() {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime > lastFrameTime + FRAME_DELAY) {
        currentFrame = (currentFrame + 1) % FRAME_COUNT;
        lastFrameTime = currentTime;
    }

    for (auto& saw : saws) {
        saw.rect.x += saw.vx;
        saw.rect.y += saw.vy;

        // Tâm của hình tròn
        float centerX = saw.rect.x + SAW_RADIUS;
        float centerY = saw.rect.y + SAW_RADIUS;

        // Kiểm tra va chạm với biên trái và phải
        if (centerX - SAW_RADIUS <= LEFT_BOUNDARY || centerX + SAW_RADIUS >= RIGHT_BOUNDARY) {
            saw.vx = -saw.vx;
        }

        // Kiểm tra va chạm với mặt đất
        if (centerY + SAW_RADIUS >= GROUND_LEVEL) {
            saw.rect.y = GROUND_LEVEL - DISPLAY_SIZE; // Đặt lại vị trí dựa trên đường kính
            saw.vy = -saw.vy;
        }

        // Kiểm tra va chạm với biên trên
        if (centerY - SAW_RADIUS < TOP_BOUNDARY) {
            float angle = (rand() % 31 + 30) * M_PI / 180;
            saw.rect = {rand() % (RIGHT_BOUNDARY - LEFT_BOUNDARY) + LEFT_BOUNDARY, TOP_BOUNDARY, DISPLAY_SIZE, DISPLAY_SIZE};
            saw.vx = SPEED * cos(angle);
            saw.vy = SPEED * sin(angle);
        }
    }
}

void Map::render() {
    SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect);

    for (auto& saw : saws) {
        SDL_Rect scaledRect = {saw.rect.x, saw.rect.y, DISPLAY_SIZE, DISPLAY_SIZE};
        SDL_RenderCopy(renderer, sawTexture, &sawClips[currentFrame], &scaledRect);
    }
}