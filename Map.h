#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <SDL_image.h>

class Map {
public:
    Map(SDL_Renderer* renderer);
    ~Map();

    void render(bool isMenu); // Thêm tham số để xác định trạng thái

private:
    SDL_Renderer* renderer;
    SDL_Texture* bgTexture;    // Texture cho background game
    SDL_Texture* menuTexture;  // Texture cho menu (menu1.png)
    SDL_Texture* startTexture; // Texture cho nút start (start.png)

    SDL_Texture* loadTexture(const char* path);
};

#endif