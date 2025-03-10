#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Kích thước ký tự trên spritesheet
const int CHAR_WIDTH = 16;
const int CHAR_HEIGHT = 16;

// Bảng ánh xạ ký tự (dựa trên ảnh font)
std::string CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!? ";

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* fontTexture = nullptr;

// Hàm tải ảnh font
SDL_Texture* LoadTexture(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::cerr << "Lỗi load ảnh: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

// Hàm vẽ chữ lên màn hình
void RenderText(const std::string& text, int x, int y) {
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        size_t index = CHARACTERS.find(c);
        if (index == std::string::npos) continue; // Bỏ qua nếu không tìm thấy

        SDL_Rect srcRect = { (int)(index % 10) * CHAR_WIDTH, (int)(index / 10) * CHAR_HEIGHT, CHAR_WIDTH, CHAR_HEIGHT };
        SDL_Rect dstRect = { x + (int)i * CHAR_WIDTH, y, CHAR_WIDTH, CHAR_HEIGHT };
        SDL_RenderCopy(renderer, fontTexture, &srcRect, &dstRect);
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || IMG_Init(IMG_INIT_PNG) == 0) {
        std::cerr << "SDL không thể khởi tạo: " << SDL_GetError() << std::endl;
        return -1;
    }

    window = SDL_CreateWindow("SDL2 Bitmap Font", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    if (!window || !renderer) {
        std::cerr << "Lỗi tạo cửa sổ hoặc renderer" << std::endl;
        return -1;
    }

    // Tải ảnh font chữ (ảnh PNG có sẵn)
    fontTexture = LoadTexture("font.png");
    if (!fontTexture) {
        std::cerr << "Lỗi tải font!" << std::endl;
        return -1;
    }

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        RenderText("HELLO SDL2!", 100, 100);
        RenderText("1234567890", 100, 150);
        
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(fontTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
