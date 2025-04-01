#include "Map.h"
#include <iostream>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 700;

Map::Map(SDL_Renderer* renderer) : renderer(renderer) {
    bgTexture = loadTexture("images/bg.png");
    if (!bgTexture) {
        std::cout << "Không thể tải bg.png\n";
    }

    menuTexture = loadTexture("images/menu1.png");
    if (!menuTexture) {
        std::cout << "Không thể tải menu1.png\n";
    }

    startTexture = loadTexture("images/start.png");
    if (!startTexture) {
        std::cout << "Không thể tải start.png\n";
    }
}

Map::~Map() {
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(startTexture);
}

SDL_Texture* Map::loadTexture(const char* path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) std::cout << "Không thể tải " << path << "\n";
    return texture;
}

void Map::render(bool isMenu) {
    if (isMenu) {
        // Hiển thị menu full màn hình
        SDL_Rect menuRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, menuTexture, nullptr, &menuRect);

        // Hiển thị nút start ở chính giữa
        int startWidth = 182;  // Giả sử chiều rộng của start.png là 182px (có thể điều chỉnh)
        int startHeight = 60; // Giả sử chiều cao của start.png là 60px (có thể điều chỉnh)
        SDL_Rect startRect = {
            (SCREEN_WIDTH - startWidth) / 2,  // Chính giữa theo trục x
            (SCREEN_HEIGHT - startHeight) / 2, // Chính giữa theo trục y
            startWidth,
            startHeight
        };
        SDL_RenderCopy(renderer, startTexture, nullptr, &startRect);
    } else {
        // Hiển thị background game
        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect);
    }
}