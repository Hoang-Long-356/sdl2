#include "Map.h"
#include <iostream>
#include <string>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 700;

Map::Map(SDL_Renderer* renderer) : renderer(renderer), isGameOverMenu(false) {
    bgTexture = loadTexture("images/bg.png");
    if (!bgTexture) {
        std::cout << "Không thể tải bg.png\n";
    }

    menuTexture = loadTexture("images/menu1.png");
    if (!menuTexture) {
        std::cout << "Không thể tải menu1.png\n";
    }

    gameOverMenuTexture = loadTexture("images/menu2.png");
    if (!gameOverMenuTexture) {
        std::cout << "Không thể tải menu2.png\n";
    }

    startTexture = loadTexture("images/start.png");
    if (!startTexture) {
        std::cout << "Không thể tải start.png\n";
    }

    numberTexture = loadTexture("images/number.png");
    if (!numberTexture) {
        std::cout << "Không thể tải number.png\n";
    }

    stopTexture = loadTexture("images/stop.png"); // Tải stop.png
    if (!stopTexture) {
        std::cout << "Không thể tải stop.png\n";
    }

    numberClips[0] = {0, 0, 94, 121};
    numberClips[1] = {94, 0, 94, 121};
    numberClips[2] = {188, 0, 94, 121};
    numberClips[3] = {282, 0, 94, 121};
    numberClips[4] = {0, 121, 94, 121};
    numberClips[5] = {94, 121, 94, 121};
    numberClips[6] = {188, 121, 94, 121};
    numberClips[7] = {282, 121, 94, 121};
    numberClips[8] = {0, 242, 94, 121};
    numberClips[9] = {94, 242, 94, 121};
}

Map::~Map() {
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(gameOverMenuTexture);
    SDL_DestroyTexture(startTexture);
    SDL_DestroyTexture(numberTexture);
    SDL_DestroyTexture(stopTexture); // Giải phóng stopTexture
}

SDL_Texture* Map::loadTexture(const char* path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) std::cout << "Không thể tải " << path << "\n";
    return texture;
}

void Map::render(bool isMenu) {
    if (isMenu) {
        SDL_Rect menuRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        if (isGameOverMenu) {
            SDL_RenderCopy(renderer, gameOverMenuTexture, nullptr, &menuRect);
        } else {
            SDL_RenderCopy(renderer, menuTexture, nullptr, &menuRect);

            int startWidth = 182;
            int startHeight = 60;
            SDL_Rect startRect = {
                (SCREEN_WIDTH - startWidth) / 2,
                (SCREEN_HEIGHT - startHeight) / 2,
                startWidth,
                startHeight
            };
            SDL_RenderCopy(renderer, startTexture, nullptr, &startRect);
        }
    } else {
        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, bgTexture, nullptr, &bgRect);
    }
}

void Map::setGameOverMenu(bool state) {
    isGameOverMenu = state;
}

void Map::renderScore(int score, SDL_Renderer* renderer, int scale) {
    std::string scoreStr = std::to_string(score);
    int digitWidth = 94 / scale;
    int digitHeight = 121 / scale;
    int totalWidth = scoreStr.length() * digitWidth;
    int startX = (SCREEN_WIDTH - totalWidth) / 2;
    int y = SCREEN_HEIGHT / 2 - digitHeight / 2 - 195;

    if (scale > 1) {
        startX = 50;
        y = 50;
    }

    for (size_t i = 0; i < scoreStr.length(); i++) {
        int digit = scoreStr[i] - '0';
        SDL_Rect destRect = {static_cast<int>(startX + i * digitWidth), y, digitWidth, digitHeight};
        SDL_RenderCopy(renderer, numberTexture, &numberClips[digit], &destRect);
    }
}

void Map::renderPause(SDL_Renderer* renderer) {
    // Giả sử stop.png có kích thước 400x200 (có thể điều chỉnh tùy theo ảnh thực tế)
    int stopWidth = 120;
    int stopHeight = 100;
    int stopX = (SCREEN_WIDTH - stopWidth) / 2;
    int stopY = (SCREEN_HEIGHT - stopHeight) / 2;
    SDL_Rect stopRect = {stopX, stopY, stopWidth, stopHeight};
    SDL_RenderCopy(renderer, stopTexture, nullptr, &stopRect);
}