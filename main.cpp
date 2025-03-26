#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "Chicken.h"
#include "Map.h"
#include "Saw.h"
#include <vector>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 700;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    window = SDL_CreateWindow("Chicken and Sawblades", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    IMG_Init(IMG_INIT_PNG);
    return true;
}

void cleanUp() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (!init()) return -1;

    Chicken chicken(renderer);
    Map gameMap(renderer);

    // Tạo danh sách cưa từ Saw::createSaws
    std::vector<Saw> saws = Saw::createSaws(renderer);

    bool running = true;
    Uint32 frameStart;
    int frameTime;
    const int FRAME_TARGET_TIME = 1000 / 60;

    while (running) {
        frameStart = SDL_GetTicks();

        running = chicken.handleInput();
        chicken.update();
        for (auto& saw : saws) {
            saw.update(); // Cập nhật từng cưa
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        gameMap.render(); // Chỉ vẽ nền
        chicken.render();
        for (auto& saw : saws) {
            saw.render(); // Vẽ từng cưa
        }

        SDL_RenderPresent(renderer);

        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_TARGET_TIME) {
            SDL_Delay(FRAME_TARGET_TIME - frameTime);
        }
    }

    cleanUp();
    return 0;
}