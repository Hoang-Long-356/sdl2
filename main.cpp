#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include "Chicken.h"
#include "Map.h"
#include "Saw.h"
#include <vector>
#include <cmath>
#include <fstream>

// Các hằng số màn hình
const int SCREEN_WIDTH = 900;  // Chiều rộng màn hình game
const int SCREEN_HEIGHT = 700; // Chiều cao màn hình game
const int GROUND_LEVEL = 528;  // Mức mặt đất (từ Chicken.h)

// Các biến toàn cục quan trọng
SDL_Window* window = nullptr;       // Con trỏ đến cửa sổ game
SDL_Renderer* renderer = nullptr;   // Con trỏ đến renderer để vẽ đồ họa
Mix_Music* backgroundMusic = nullptr; // Con trỏ đến nhạc nền

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

struct SawInfo {
    Saw* saw;              // Con trỏ đến đối tượng cưa
    Uint32 lastAppearTime; // Thời gian cưa xuất hiện lần cuối (ms)
    Uint32 initialDelay;   // Thời gian trì hoãn ban đầu trước khi cưa xuất hiện (ms)
    Uint32 reappearInterval; // Khoảng thời gian tái xuất hiện của cưa (ms)
    bool jumpedOver;       // Cờ đánh dấu cưa đã bị gà nhảy qua chưa

    SawInfo(SDL_Renderer* renderer, Uint32 initDelay, Uint32 reappear) 
        : saw(nullptr), lastAppearTime(0), initialDelay(initDelay), reappearInterval(reappear), jumpedOver(false) {}
};

void updateSaws(std::vector<SawInfo>& sawInfos) {
    for (auto& info : sawInfos) {
        if (info.saw) {
            if (info.saw->update()) {
                delete info.saw;
                info.saw = nullptr;
                info.jumpedOver = false;
            }
        }
    }
}

void renderSaws(const std::vector<SawInfo>& sawInfos, SDL_Renderer* renderer) {
    for (const auto& info : sawInfos) {
        if (info.saw) {
            info.saw->render();
        }
    }
}

bool checkCircleCollision(const SDL_Rect& rect1, const SDL_Rect& rect2) {
    float centerX1 = rect1.x + rect1.w / 2.0f;
    float centerY1 = rect1.y + rect1.h / 2.0f;
    float centerX2 = rect2.x + rect2.w / 2.0f;
    float centerY2 = rect2.y + rect2.h / 2.0f;

    float radius1 = (rect1.w - 4) / 2.0f;
    float radius2 = (rect2.w - 4) / 2.0f;

    float dx = centerX2 - centerX1;
    float dy = centerY2 - centerY1;
    float distance = std::sqrt(dx * dx + dy * dy);

    return distance < (radius1 + radius2);
}

bool checkJumpOverSaw(const SDL_Rect& chickenRect, const SDL_Rect& sawRect) {
    bool isHigher = chickenRect.y + chickenRect.h < sawRect.y;
    float centerX1 = chickenRect.x + chickenRect.w / 2.0f;
    float centerX2 = sawRect.x + sawRect.w / 2.0f;
    float dx = std::abs(centerX2 - centerX1);
    float totalWidth = (chickenRect.w + sawRect.w) / 2.0f;
    bool isAligned = dx < totalWidth;

    return isHigher && isAligned;
}

int main(int argc, char* argv[]) {
    if (!init()) return -1;

    Mix_PlayMusic(backgroundMusic, -1);

    Chicken chicken(renderer);
    Map gameMap(renderer);

    std::vector<SawInfo> sawInfos; // Danh sách các cưa
    Uint32 gameStartTime = 0;      // Thời gian bắt đầu game (ms)
    bool isPlaying = false;        // Trạng thái đang chơi game
    bool isGameOver = false;       // Trạng thái game over
    bool isPaused = false;         // Trạng thái tạm dừng game
    int score = 0;                 // Điểm số hiện tại
    int highScore = 0;             // Điểm cao nhất

    std::ifstream inFile("highscore.txt");
    if (inFile.is_open()) {
        inFile >> highScore;
        inFile.close();
    }

    sawInfos.emplace_back(renderer, 500, 10000);
    sawInfos.emplace_back(renderer, 2000, 10000);
    sawInfos.emplace_back(renderer, 4000, 10000);
    sawInfos.emplace_back(renderer, 6000, 10000);
    sawInfos.emplace_back(renderer, 8000, 10000);
    sawInfos.emplace_back(renderer, 10000, 10000);
    sawInfos.emplace_back(renderer, 12000, 10000);
    sawInfos.emplace_back(renderer, 14000, 10000);

    bool running = true;           // Trạng thái vòng lặp chính của game
    Uint32 frameStart;             // Thời gian bắt đầu khung hình (ms)
    int frameTime;                 // Thời gian thực hiện một khung hình (ms)
    const int FRAME_TARGET_TIME = 1000 / 60; // Thời gian mục tiêu cho 60 FPS

    while (running) {
        frameStart = SDL_GetTicks();
    
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
                continue;
            }
            if (!isPlaying && !isGameOver && event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                int startWidth = 182;
                int startHeight = 60;
                int startX = (SCREEN_WIDTH - startWidth) / 2;
                int startY = (SCREEN_HEIGHT - startHeight) / 2;
                if (x >= startX && x <= startX + startWidth && y >= startY && y <= startY + startHeight) {
                    isPlaying = true;
                    isGameOver = false;
                    gameStartTime = SDL_GetTicks();
                    gameMap.setGameOverMenu(false);
                    score = 0;
                    chicken.resetPosition();
                    for (auto& info : sawInfos) {
                        if (info.saw) {
                            delete info.saw;
                            info.saw = nullptr;
                        }
                        info.lastAppearTime = 0;
                        info.jumpedOver = false;
                    }
                }
            }
            if (isGameOver && event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                int restartWidth = 200;
                int restartHeight = 100;
                int restartX = (SCREEN_WIDTH - restartWidth) / 2;
                int restartY = (SCREEN_HEIGHT - restartHeight) / 2;
                if (x >= restartX && x <= restartX + restartWidth && y >= restartY && y <= restartY + restartHeight) {
                    isPlaying = true;
                    isGameOver = false;
                    gameStartTime = SDL_GetTicks();
                    gameMap.setGameOverMenu(false);
                    score = 0;
                    chicken.resetPosition();
                    for (auto& info : sawInfos) {
                        if (info.saw) {
                            delete info.saw;
                            info.saw = nullptr;
                        }
                        info.lastAppearTime = 0;
                        info.jumpedOver = false;
                    }
                }
                int outgameWidth = 200;
                int outgameHeight = 100;
                int outgameX = (SCREEN_WIDTH - outgameWidth) / 2;
                int outgameY = restartY + restartHeight + 10;
                if (x >= outgameX && x <= outgameX + outgameWidth && y >= outgameY && y <= outgameY + outgameHeight) {
                    running = false;
                }
            }
            if (isPlaying && !isGameOver && !isPaused && event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                int pauseWidth = 38;
                int pauseHeight = 40;
                int pauseX = SCREEN_WIDTH - pauseWidth - 70;
                int pauseY = 30;
                if (x >= pauseX && x <= pauseX + pauseWidth && y >= pauseY && y <= pauseY + pauseHeight) {
                    isPaused = true; // Tạm dừng game khi nhấn nút pause
                }
            }
            if (isPaused && event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                int stopWidth = 116;  // Chiều rộng của stop.png
                int stopHeight = 101; // Chiều cao của stop.png
                int stopX = (SCREEN_WIDTH - stopWidth) / 2; // Tọa độ x căn giữa
                int stopY = (SCREEN_HEIGHT - stopHeight) / 2; // Tọa độ y căn giữa
                // Phần trên (tiếp tục chơi): 116x101px
                if (x >= stopX && x <= stopX + stopWidth && y >= stopY && y <= stopY + stopHeight / 2) {
                    isPaused = false; // Tiếp tục chơi
                }
                // Phần dưới (chuyển sang menu2.png): 116x101px
                if (x >= stopX && x <= stopX + stopWidth && y > stopY + stopHeight / 2 && y <= stopY + stopHeight) {
                    isPaused = false;
                    isPlaying = false;
                    isGameOver = true;
                    gameMap.setGameOverMenu(true);
                    if (score > highScore) {
                        highScore = score;
                        std::ofstream outFile("highscore.txt");
                        if (outFile.is_open()) {
                            outFile << highScore;
                            outFile.close();
                        }
                    }
                }
            }
    
            if (isPlaying && !isGameOver) {
                running = chicken.handleInput(event);
            }
        }
    
        // Phần còn lại của vòng lặp main giữ nguyên
        if (isPlaying && !isGameOver && !isPaused) {
            chicken.update();
            running = chicken.handleInput(event);
    
            Uint32 currentTime = SDL_GetTicks() - gameStartTime;
    
            for (auto& info : sawInfos) {
                if (!info.saw) {
                    if (info.lastAppearTime == 0) {
                        if (currentTime >= info.initialDelay) {
                            info.saw = new Saw(renderer);
                            info.lastAppearTime = currentTime;
                            info.jumpedOver = false;
                        }
                    } else {
                        if (currentTime - info.lastAppearTime >= info.reappearInterval) {
                            info.saw = new Saw(renderer);
                            info.lastAppearTime = currentTime;
                            info.jumpedOver = false;
                        }
                    }
                }
            }
    
            updateSaws(sawInfos);
    
            const SDL_Rect& chickenRect = chicken.getRect();
            for (auto& info : sawInfos) {
                if (info.saw) {
                    const SDL_Rect& sawRect = info.saw->getRect();
                    if (checkJumpOverSaw(chickenRect, sawRect)) {
                        info.jumpedOver = true;
                    }
                    if (checkCircleCollision(chickenRect, sawRect)) {
                        isPlaying = false;
                        isGameOver = true;
                        gameMap.setGameOverMenu(true);
                        if (score > highScore) {
                            highScore = score;
                            std::ofstream outFile("highscore.txt");
                            if (outFile.is_open()) {
                                outFile << highScore;
                                outFile.close();
                            }
                        }
                        for (auto& sawInfo : sawInfos) {
                            if (sawInfo.saw) {
                                delete sawInfo.saw;
                                sawInfo.saw = nullptr;
                                sawInfo.lastAppearTime = 0;
                                sawInfo.jumpedOver = false;
                            }
                        }
                        break;
                    }
                }
            }
    
            if (!chicken.isChickenJumping()) {
                for (auto& info : sawInfos) {
                    if (info.saw && info.jumpedOver) {
                        delete info.saw;
                        info.saw = nullptr;
                        info.jumpedOver = false;
                        score++;
                    }
                }
            }
        }
    
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    
        gameMap.render(isGameOver || !isPlaying);
    
        if (isPlaying && !isGameOver) {
            chicken.render();
            renderSaws(sawInfos, renderer);
        }
    
        if (isGameOver) {
            gameMap.renderScore(score, renderer);
            gameMap.renderScore(highScore, renderer, 7);
        }
    
        if (isPaused) {
            gameMap.renderPause(renderer);
        }
    
        SDL_RenderPresent(renderer);
    
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_TARGET_TIME) {
            SDL_Delay(FRAME_TARGET_TIME - frameTime);
        }
    }
    
    // Phần còn lại của main (clean up, v.v.) giữ nguyên
    std::ofstream outFile("highscore.txt");
    if (outFile.is_open()) {
        outFile << highScore;
        outFile.close();
    }
    
    for (auto& info : sawInfos) {
        if (info.saw) {
            delete info.saw;
        }
    }
    
    cleanUp();
    return 0;
}