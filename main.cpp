#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include <ctime>

const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 400;
const int GRID_SIZE = 4;
const int CELL_SIZE = SCREEN_WIDTH / GRID_SIZE;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
int board[GRID_SIZE][GRID_SIZE] = {0};  // Bảng 4x4 chứa số

bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }
    window = SDL_CreateWindow("2048 Basic", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    if (!window || !renderer) {
        std::cerr << "Window/Renderer Error: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}

void closeSDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void drawGrid() {
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    for (int i = 0; i <= GRID_SIZE; i++) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT);
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, SCREEN_WIDTH, i * CELL_SIZE);
    }
}

// Vẽ số bằng các điểm nhỏ (mô phỏng số)
void drawNumber(int value, int x, int y) {
    if (value == 0) return; // Không vẽ gì nếu ô trống

    int numDots = value / 2; // Số lượng dấu chấm tương ứng với giá trị
    int dotSize = CELL_SIZE / 8; // Kích thước dấu chấm
    int startX = x * CELL_SIZE + CELL_SIZE / 4; 
    int startY = y * CELL_SIZE + CELL_SIZE / 4;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    
    for (int i = 0; i < numDots; i++) {
        SDL_Rect dot = { startX + (i % 3) * dotSize, startY + (i / 3) * dotSize, dotSize, dotSize };
        SDL_RenderFillRect(renderer, &dot);
    }
}

void drawTiles() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            SDL_Rect cell = {j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            if (board[i][j] > 0) {
                SDL_SetRenderDrawColor(renderer, 255, 150, 100, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 230, 230, 230, 255);
            }
            SDL_RenderFillRect(renderer, &cell);
            drawNumber(board[i][j], j, i);
        }
    }
}

void spawnNewTile() {
    int emptyCells = 0;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board[i][j] == 0) emptyCells++;
        }
    }
    
    if (emptyCells == 0) return;

    int randPos = rand() % emptyCells;
    int count = 0;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board[i][j] == 0) {
                if (count == randPos) {
                    board[i][j] = (rand() % 2 + 1) * 2;
                    return;
                }
                count++;
            }
        }
    }
}

bool moveTiles(int dx, int dy) {
    bool moved = false;
    for (int i = (dx == 1 ? GRID_SIZE - 2 : 1); i >= 0 && i < GRID_SIZE; i += (dx == 1 ? -1 : 1)) {
        for (int j = (dy == 1 ? GRID_SIZE - 2 : 1); j >= 0 && j < GRID_SIZE; j += (dy == 1 ? -1 : 1)) {
            if (board[i][j] != 0) {
                int x = i, y = j;
                while (x + dx >= 0 && x + dx < GRID_SIZE && y + dy >= 0 && y + dy < GRID_SIZE && board[x + dx][y + dy] == 0) {
                    board[x + dx][y + dy] = board[x][y];
                    board[x][y] = 0;
                    x += dx;
                    y += dy;
                    moved = true;
                }
                if (x + dx >= 0 && x + dx < GRID_SIZE && y + dy >= 0 && y + dy < GRID_SIZE && board[x + dx][y + dy] == board[x][y]) {
                    board[x + dx][y + dy] *= 2;
                    board[x][y] = 0;
                    moved = true;
                }
            }
        }
    }
    return moved;
}

void handleInput(SDL_Event& e, bool& running) {
    if (e.type == SDL_QUIT) {
        running = false;
    } else if (e.type == SDL_KEYDOWN) {
        bool moved = false;
        switch (e.key.keysym.sym) {
            case SDLK_UP: moved = moveTiles(-1, 0); break;
            case SDLK_DOWN: moved = moveTiles(1, 0); break;
            case SDLK_LEFT: moved = moveTiles(0, -1); break;
            case SDLK_RIGHT: moved = moveTiles(0, 1); break;
        }
        if (moved) spawnNewTile();
    }
}

void gameLoop() {
    bool running = true;
    SDL_Event e;
    spawnNewTile();
    spawnNewTile();

    while (running) {
        while (SDL_PollEvent(&e)) {
            handleInput(e, running);
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        drawTiles();
        drawGrid();

        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char* argv[]) {
    srand(time(0));
    if (!initSDL()) return -1;
    gameLoop();
    closeSDL();
    return 0;
}
