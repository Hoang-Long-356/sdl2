#include "Saw.h"
#include <SDL_image.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>

const int SAW_SIZE = 150; // Kích thước gốc của cưa trong texture (pixel)
const int DISPLAY_SIZE = 50; // Kích thước hiển thị của cưa trên màn hình (pixel)
const int FRAME_DELAY = 10; // Thời gian delay giữa các khung hình (ms), nhanh hơn animation
const int LEFT_BOUNDARY = 250; // Biên trái của khu vực di chuyển cưa
const int RIGHT_BOUNDARY = 651; // Biên phải của khu vực di chuyển cưa
const int GROUND_LEVEL = 580; // Mức mặt đất (biên dưới) mà cưa nảy lên
const int TOP_BOUNDARY = 60; // Biên trên mà cưa bị xóa khi chạm lại

Saw::Saw(SDL_Renderer* renderer)
    : renderer(renderer), currentFrame(0), lastFrameTime(0) {
    static bool seeded = false; // Biến tĩnh để chỉ seed random một lần
    if (!seeded) {
        srand(static_cast<unsigned>(time(0))); // Seed cho số ngẫu nhiên
        seeded = true;
    }
    texture = loadTexture("images/saw.png"); // Tải hình ảnh cưa
    int x = LEFT_BOUNDARY + rand() % (RIGHT_BOUNDARY - LEFT_BOUNDARY - DISPLAY_SIZE); // Vị trí x ngẫu nhiên trong giới hạn
    rect = {x, TOP_BOUNDARY, DISPLAY_SIZE, DISPLAY_SIZE}; // Khởi tạo vị trí từ biên trên

    // Ngẫu nhiên chọn một trong hai phạm vi góc: 35°-45° hoặc 125°-135° (đối xứng)
    bool direction = rand() % 2; // 0: 35°-45°, 1: 125°-135°
    float angle;
    if (direction == 0) {
        angle = (35 * M_PI / 180) + (rand() % 11) * (M_PI / 180); // Góc 35°-45° (xuống bên phải)
    } else {
        angle = (125 * M_PI / 180) + (rand() % 11) * (M_PI / 180); // Góc 125°-135° (xuống bên trái)
    }
    vx = cos(angle) * 4.0f; // Vận tốc ban đầu theo trục x
    vy = sin(angle) * 4.0f; // Vận tốc ban đầu theo trục y (hướng xuống)
    setupAnimation(); // Thiết lập animation
}

Saw::~Saw() {
    SDL_DestroyTexture(texture); // Giải phóng texture khi hủy cưa
}

SDL_Texture* Saw::loadTexture(const char* path) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path); // Tải texture từ đường dẫn
    if (!newTexture) {
        std::cout << "Không thể tải " << path << ": " << SDL_GetError() << std::endl;
    }
    return newTexture;
}

void Saw::setupAnimation() {
    // Thiết lập các vùng cắt từ texture cho từng khung hình animation
    clips[0] = {0, 0, SAW_SIZE, SAW_SIZE};
    clips[1] = {150, 0, SAW_SIZE, SAW_SIZE};
    clips[2] = {300, 0, SAW_SIZE, SAW_SIZE};
    clips[3] = {0, 150, SAW_SIZE, SAW_SIZE};
    clips[4] = {150, 150, SAW_SIZE, SAW_SIZE};
    clips[5] = {300, 150, SAW_SIZE, SAW_SIZE};
    clips[6] = {0, 300, SAW_SIZE, SAW_SIZE};
    clips[7] = {150, 300, SAW_SIZE, SAW_SIZE};
    clips[8] = {300, 300, SAW_SIZE, SAW_SIZE};
    clips[9] = {0, 450, SAW_SIZE, SAW_SIZE};
    clips[10] = {150, 450, SAW_SIZE, SAW_SIZE};
}

bool Saw::update() {
    rect.x += static_cast<int>(vx); // Cập nhật vị trí x theo vận tốc
    rect.y += static_cast<int>(vy); // Cập nhật vị trí y theo vận tốc

    // Va chạm biên trái
    if (rect.x < LEFT_BOUNDARY) {
        rect.x = LEFT_BOUNDARY;
        vx = -vx; // Nảy lại (đổi dấu vận tốc x)
    }
    // Va chạm biên phải
    else if (rect.x + DISPLAY_SIZE > RIGHT_BOUNDARY) {
        rect.x = RIGHT_BOUNDARY - DISPLAY_SIZE;
        vx = -vx; // Nảy lại (đổi dấu vận tốc x)
    }

    // Va chạm mặt đất
    if (rect.y + DISPLAY_SIZE > GROUND_LEVEL) {
        rect.y = GROUND_LEVEL - DISPLAY_SIZE;
        vy = -vy; // Nảy lại với cùng vận tốc (không giảm)
    }

    // Xóa khi chạm biên trên
    if (rect.y < TOP_BOUNDARY) {
        return true; // Trả về true để xóa cưa
    }

    Uint32 currentTime = SDL_GetTicks(); // Thời gian hiện tại
    if (currentTime - lastFrameTime >= FRAME_DELAY) { // Chuyển khung hình nếu đủ thời gian delay
        currentFrame = (currentFrame + 1) % FRAME_COUNT; // Chuyển sang khung hình tiếp theo
        lastFrameTime = currentTime; // Cập nhật thời gian chuyển khung cuối
    }

    return false; // Không xóa cưa nếu chưa chạm biên trên
}

void Saw::render() const {
    SDL_RenderCopy(renderer, texture, &clips[currentFrame], &rect); // Vẽ cưa với khung hình hiện tại
}

// Thêm phương thức để lấy rect của cưa
const SDL_Rect& Saw::getRect() const {
    return rect;
}