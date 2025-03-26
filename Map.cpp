#include "Map.h"
#include <iostream>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 700;

Map::Map(SDL_Renderer* renderer) : renderer(renderer) {
    bgTexture = loadTexture("images/bg.png");
    if (!bgTexture) {
        std::cout << "Không thể tải bg.png\n";
    }
}

Map::~Map() {
    SDL_DestroyTexture(bgTexture);
}

SDL_Texture* Map::loadTexture(const char* path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) std::cout << "Không thể tải " << path << "\n";
    return texture;
}

void Map::render() {
    SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect);
}