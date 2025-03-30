#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include "Chicken.h"
#include "Map.h"
#include "Saw.h"
#include <vector>
#include <queue>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 700;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Mix_Music* backgroundMusic = nullptr;

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cout << "SDL Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Chicken and Sawblades", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    IMG_Init(IMG_INIT_PNG);
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer Init failed: " << Mix_GetError() << std::endl;
        return false;
    }

    backgroundMusic = Mix_LoadMUS("music.mp3");
    if (!backgroundMusic) {
        std::cout << "Failed to load music: " << Mix_GetError() << std::endl;
        return false;
    }

    return true;
}

void cleanUp() {
    Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

// Cấu trúc để lưu thời gian xuất hiện và chu kỳ của mỗi cưa
struct SawSpawn {
    Uint32 initialTime; // Thời gian xuất hiện đầu tiên
    Uint32 lastSpawnTime; // Thời gian xuất hiện cuối
    Uint32 repeatInterval; // Chu kỳ lặp lại

    SawSpawn(Uint32 init) : initialTime(init), lastSpawnTime(0), repeatInterval(7000) {}
};

int main(int argc, char* argv[]) {
    if (!init()) return -1;

    Mix_PlayMusic(backgroundMusic, -1);

    Chicken chicken(renderer);
    Map gameMap(renderer);
    std::vector<Saw> saws;
    Uint32 gameStartTime = SDL_GetTicks();

    // Hàng đợi chứa các cưa sẽ xuất hiện
    std::vector<SawSpawn> sawSpawns = {
        SawSpawn(500),  // s1: 0.5s
        SawSpawn(2000), // s2: 2s
        SawSpawn(4000), // s3: 4s
        SawSpawn(6000), // s4: 6s
        SawSpawn(8000), // s5: 8s
        SawSpawn(10000), // s6: 10s
        SawSpawn(12000), // s7: 12s
    };
    const int MAX_SAWS = 8;

    bool running = true;
    Uint32 frameStart;
    int frameTime;
    const int FRAME_TARGET_TIME = 1000 / 60;

    while (running) {
        frameStart = SDL_GetTicks();

        running = chicken.handleInput();
        chicken.update();

        Uint32 gameTime = SDL_GetTicks() - gameStartTime;

        // Kiểm tra và thêm cưa
        for (auto& spawn : sawSpawns) {
            Uint32 timeSinceLastSpawn = (spawn.lastSpawnTime == 0) ? gameTime : (frameStart - spawn.lastSpawnTime);
            Uint32 nextSpawnTime = (spawn.lastSpawnTime == 0) ? spawn.initialTime : spawn.repeatInterval;

            if (gameTime >= spawn.initialTime && timeSinceLastSpawn >= nextSpawnTime && saws.size() < MAX_SAWS) {
                saws.emplace_back(renderer);
                spawn.lastSpawnTime = frameStart;
            }
        }

        Saw::updateSaws(saws);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        gameMap.render();
        chicken.render();
        Saw::renderSaws(saws, renderer);

        SDL_RenderPresent(renderer);

        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_TARGET_TIME) {
            SDL_Delay(FRAME_TARGET_TIME - frameTime);
        }
    }

    cleanUp();
    return 0;
}