#include "Map.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 700;

Map::Map(SDL_Renderer* renderer) : renderer(renderer) {
    sawTexture = loadTexture("images/saw.png");
    bgTexture = loadTexture("images/bg.png");
    
    if (!bgTexture) {
        std::cout << "Background image failed to load.\n";
    }

    srand(static_cast<unsigned>(time(0)));
    for (int i = 0; i < 5; i++) {
        saws[i] = {rand() % (SCREEN_WIDTH - 200) + 100, 50, 50, 50};
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
    for (int i = 0; i < 5; i++) {
        saws[i].y += 5;
        if (saws[i].y > SCREEN_HEIGHT - 50) {
            saws[i].y = 50;
            saws[i].x = rand() % (SCREEN_WIDTH - 200) + 100;
        }
    }
}

void Map::render() {
    // Định nghĩa khung viền frameBox
    SDL_Rect frameBox = {100, 50, SCREEN_WIDTH - 200, SCREEN_HEIGHT - 100};

    // Giới hạn khu vực vẽ trong frameBox
    SDL_RenderSetClipRect(renderer, &frameBox);

    // Vẽ background bên trong frameBox
    SDL_RenderCopy(renderer, bgTexture, nullptr, &frameBox);

    // Vẽ các lưỡi cưa bên trong frameBox
    for (int i = 0; i < 5; i++) {
        SDL_RenderCopy(renderer, sawTexture, nullptr, &saws[i]);
    }

    // Xóa giới hạn clip (để các đối tượng khác không bị ảnh hưởng)
    SDL_RenderSetClipRect(renderer, nullptr);

    // Vẽ khung viền frameBox
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Màu đỏ
    SDL_RenderDrawRect(renderer, &frameBox);
}
