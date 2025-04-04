#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <SDL_image.h>

class Map {
public:
    Map(SDL_Renderer* renderer);
    ~Map();

    void render(bool isMenu);
    void setGameOverMenu(bool state);
    void renderScore(int score, SDL_Renderer* renderer, int scale = 1);
    void renderPause(SDL_Renderer* renderer); // Thêm để vẽ màn hình tạm dừng

private:
    SDL_Renderer* renderer;
    SDL_Texture* bgTexture;
    SDL_Texture* menuTexture;
    SDL_Texture* gameOverMenuTexture;
    SDL_Texture* startTexture;
    SDL_Texture* numberTexture;
    SDL_Texture* stopTexture; // Texture cho màn hình tạm dừng
    SDL_Rect numberClips[10];
    bool isGameOverMenu;

    SDL_Texture* loadTexture(const char* path);
};

#endif