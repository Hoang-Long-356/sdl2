#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>
#include "Chicken.h"
#include "Map.h"
#include "Saw.h"
#include <vector>

const int SCREEN_WIDTH = 900; // Chiều rộng màn hình game (pixel)
const int SCREEN_HEIGHT = 700; // Chiều cao màn hình game (pixel)

SDL_Window* window = nullptr; // Con trỏ đến cửa sổ game
SDL_Renderer* renderer = nullptr; // Con trỏ đến renderer để vẽ
Mix_Music* backgroundMusic = nullptr; // Con trỏ đến nhạc nền

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) { // Khởi tạo SDL với video và âm thanh
        std::cout << "SDL Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Chicken and Sawblades", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN); // Tạo cửa sổ game
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); // Tạo renderer tăng tốc phần cứng
    if (!renderer) return false;

    IMG_Init(IMG_INIT_PNG); // Khởi tạo SDL_image để tải file PNG
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { // Khởi tạo SDL_mixer để phát âm thanh
        std::cout << "SDL_mixer Init failed: " << Mix_GetError() << std::endl;
        return false;
    }

    backgroundMusic = Mix_LoadMUS("music.mp3"); // Tải file nhạc nền
    if (!backgroundMusic) {
        std::cout << "Failed to load music: " << Mix_GetError() << std::endl;
        return false;
    }

    return true;
}

void cleanUp() {
    Mix_FreeMusic(backgroundMusic); // Giải phóng nhạc nền
    Mix_CloseAudio(); // Đóng SDL_mixer
    SDL_DestroyRenderer(renderer); // Hủy renderer
    SDL_DestroyWindow(window); // Hủy cửa sổ
    IMG_Quit(); // Thoát SDL_image
    SDL_Quit(); // Thoát SDL
}

struct SawInfo {
    Saw* saw;
    Uint32 lastAppearTime; // Thời gian cưa cuối cùng xuất hiện
    Uint32 initialDelay;   // Thời gian trì hoãn ban đầu
    Uint32 reappearInterval; // Khoảng thời gian tái xuất hiện

    SawInfo(SDL_Renderer* renderer, Uint32 initDelay, Uint32 reappear) 
        : saw(nullptr), lastAppearTime(0), initialDelay(initDelay), reappearInterval(reappear) {}
};

void updateSaws(std::vector<SawInfo>& sawInfos) {
    for (auto& info : sawInfos) {
        if (info.saw) {
            if (info.saw->update()) { // Nếu cưa bị xóa (chạm biên trên)
                delete info.saw; // Giải phóng bộ nhớ
                info.saw = nullptr;
            }
        }
    }
}

void renderSaws(const std::vector<SawInfo>& sawInfos, SDL_Renderer* renderer) {
    for (const auto& info : sawInfos) {
        if (info.saw) {
            info.saw->render(); // Vẽ cưa nếu tồn tại
        }
    }
}

int main(int argc, char* argv[]) {
    if (!init()) return -1;

    Mix_PlayMusic(backgroundMusic, -1); // Phát nhạc nền lặp vô hạn

    Chicken chicken(renderer); // Đối tượng gà (nhân vật chính)
    Map gameMap(renderer); // Đối tượng bản đồ game

    std::vector<SawInfo> sawInfos; // Danh sách thông tin cưa
    Uint32 gameStartTime = 0; // Thời gian bắt đầu game (sẽ được đặt sau khi thoát menu)
    bool isMenu = true; // Trạng thái ban đầu là menu

    // Khởi tạo thông tin cho các cưa s1, s2, s3, s4, s5, s6
    sawInfos.emplace_back(renderer, 500, 10000);  // Cưa s1: xuất hiện sau 0.5s, tái xuất sau 10s
    sawInfos.emplace_back(renderer, 2000, 10000); // Cưa s2: xuất hiện sau 2s, tái xuất sau 10s
    sawInfos.emplace_back(renderer, 4000, 10000); // Cưa s3: xuất hiện sau 4s, tái xuất sau 10s
    sawInfos.emplace_back(renderer, 8000, 10000); // Cưa s4: xuất hiện sau 8s, tái xuất sau 10s
    sawInfos.emplace_back(renderer, 12000, 10000); // Cưa s5: xuất hiện sau 12s, tái xuất sau 10s
    sawInfos.emplace_back(renderer, 18000, 10000); // Cưa s6: xuất hiện sau 18s, tái xuất sau 10s

    bool running = true; // Biến điều khiển vòng lặp game
    Uint32 frameStart; // Thời gian bắt đầu mỗi khung hình (ms)
    int frameTime; // Thời gian thực hiện một khung hình (ms)
    const int FRAME_TARGET_TIME = 1000 / 60; // Thời gian mục tiêu cho mỗi khung hình (60 FPS)

    while (running) {
        frameStart = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event) && isMenu) {
            if (event.type == SDL_QUIT) {
                running = false; // Thoát khi đóng cửa sổ
            }
            if (isMenu && event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                // Kiểm tra nếu nhấp vào vùng nút start 182x60px
                int startWidth = 182;
                int startHeight = 60;
                int startX = (SCREEN_WIDTH - startWidth) / 2;
                int startY = (SCREEN_HEIGHT - startHeight) / 2;
                if (x >= startX && x <= startX + startWidth && y >= startY && y <= startY + startHeight) {
                    isMenu = false; // Thoát menu, bắt đầu game
                    gameStartTime = SDL_GetTicks(); // Đặt thời gian bắt đầu game
                }
            }
        }
        if (!isMenu) {
            // Xử lý input liên tục cho chicken (di chuyển trái/phải và nhảy)
            running = chicken.handleInput(event); // Gọi handleInput để xử lý tất cả đầu vào (bao gồm nhảy)
            chicken.update(); // Cập nhật trạng thái của gà

            Uint32 currentTime = SDL_GetTicks() - gameStartTime; // Thời gian đã trôi qua kể từ khi game bắt đầu

            // Kiểm tra và tạo/re-tạo cưa cho từng loại
            for (auto& info : sawInfos) {
                if (!info.saw) { // Nếu cưa không tồn tại
                    if (info.lastAppearTime == 0) { // Chưa từng xuất hiện
                        if (currentTime >= info.initialDelay) {
                            info.saw = new Saw(renderer); // Tạo cưa mới
                            info.lastAppearTime = currentTime; // Ghi lại thời gian xuất hiện
                        }
                    } else { // Đã từng xuất hiện, kiểm tra tái xuất
                        if (currentTime - info.lastAppearTime >= info.reappearInterval) {
                            info.saw = new Saw(renderer); // Tái tạo cưa
                            info.lastAppearTime = currentTime; // Cập nhật thời gian tái xuất
                        }
                    }
                }
            }

            // Cập nhật và xóa cưa
            updateSaws(sawInfos);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Đặt màu nền đen
        SDL_RenderClear(renderer); // Xóa màn hình

        gameMap.render(isMenu); // Vẽ bản đồ hoặc menu tùy trạng thái
        if (!isMenu) {
            chicken.render(); // Vẽ gà khi không ở menu
            renderSaws(sawInfos, renderer); // Vẽ tất cả cưa khi không ở menu
        }

        SDL_RenderPresent(renderer); // Hiển thị khung hình

        frameTime = SDL_GetTicks() - frameStart; // Tính thời gian khung hình
        if (frameTime < FRAME_TARGET_TIME) { // Giữ FPS ổn định
            SDL_Delay(FRAME_TARGET_TIME - frameTime);
        }
    }

    // Giải phóng tất cả cưa còn lại khi thoát
    for (auto& info : sawInfos) {
        if (info.saw) {
            delete info.saw;
        }
    }

    cleanUp();
    return 0;
}