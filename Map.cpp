#include "Map.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>
#include <vector>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 700;
const float SPEED = 5.0f;
const int LEFT_BOUNDARY = 193;
const int RIGHT_BOUNDARY = 728;
const int GROUND_LEVEL = 505;
const int TOP_BOUNDARY = 61;
const int SAW_SIZE = 50;

Map::Map(SDL_Renderer* renderer) : renderer(renderer) {
    sawTexture = loadTexture("images/saw.png");
    bgTexture = loadTexture("images/bg.png");

    if (!bgTexture) {
        std::cout << "Background image failed to load.\n";
    }

    srand(static_cast<unsigned>(time(0)));

    int numSaws = rand() % 5 + 3;  
    for (int i = 0; i < numSaws; i++) {
        float angle = (rand() % 31 + 30) * M_PI / 180;
        saws.push_back({
            {rand() % (RIGHT_BOUNDARY - LEFT_BOUNDARY) + LEFT_BOUNDARY, rand() % 200 + 50, SAW_SIZE, SAW_SIZE},
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

void Map::update() {
    for (auto& saw : saws) {
        saw.rect.x += saw.vx;
        saw.rect.y += saw.vy;

        // Va vào tường trái/phải
        if (saw.rect.x <= LEFT_BOUNDARY || saw.rect.x + saw.rect.w >= RIGHT_BOUNDARY) {
            saw.vx = -saw.vx; 
        }

        // Va vào mặt đất
        if (saw.rect.y + saw.rect.h >= GROUND_LEVEL) {
            saw.rect.y = GROUND_LEVEL - saw.rect.h;
            saw.vy = -saw.vy;
        }

        // Biến mất khi bay lên trên
        if (saw.rect.y < TOP_BOUNDARY) {
            float angle = (rand() % 31 + 30) * M_PI / 180;
            saw.rect = {rand() % (RIGHT_BOUNDARY - LEFT_BOUNDARY) + LEFT_BOUNDARY, TOP_BOUNDARY, SAW_SIZE, SAW_SIZE};
            saw.vx = SPEED * cos(angle);
            saw.vy = SPEED * sin(angle);
        }
    }
}

void Map::render() {
    SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect);

    for (auto& saw : saws) {
        SDL_RenderCopy(renderer, sawTexture, nullptr, &saw.rect);
    }
}
