#ifndef SAW_H
#define SAW_H

#include <SDL.h>
#include <vector>

class Saw {
public:
    Saw(SDL_Renderer* renderer); // Constructor: Khởi tạo một cưa với renderer
    ~Saw(); // Destructor: Giải phóng texture của cưa
    bool update(); // Cập nhật vị trí và trạng thái của cưa, trả về true nếu cưa cần bị xóa
    void render() const; // Vẽ cưa lên màn hình
    bool isOffScreen() const; // Kiểm tra xem cưa có nằm ngoài màn hình (trên TOP_BOUNDARY) không

    static void updateSaws(std::vector<Saw>& saws); // Cập nhật tất cả cưa trong danh sách
    static void renderSaws(const std::vector<Saw>& saws, SDL_Renderer* renderer); // Vẽ tất cả cưa

private:
    SDL_Renderer* renderer; // Con trỏ đến renderer để vẽ cưa
    SDL_Texture* texture; // Texture chứa hình ảnh cưa (saw.png)
    SDL_Rect rect; // Hình chữ nhật xác định vị trí và kích thước hiển thị của cưa
    float vx, vy; // Vận tốc theo trục x và y của cưa
    int currentFrame; // Khung hình hiện tại trong animation (0 đến FRAME_COUNT-1)
    Uint32 lastFrameTime; // Thời gian lần cuối chuyển khung hình (dùng để tính thời gian delay)
    static const int FRAME_COUNT = 11; // Tổng số khung hình trong animation của cưa
    SDL_Rect clips[FRAME_COUNT]; // Mảng các vùng cắt từ texture cho từng khung hình

    SDL_Texture* loadTexture(const char* path); // Tải texture từ file ảnh
    void setupAnimation(); // Thiết lập các vùng cắt (clips) cho animation
};

#endif