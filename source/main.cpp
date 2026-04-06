#include <raylib.h>

constexpr int minWindowWidth  = 800;
constexpr int minWindowHeight = 600;

int main() {
   SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
   InitWindow(minWindowWidth, minWindowHeight, "Paint Together");
   SetWindowMinSize(minWindowWidth, minWindowHeight);

   while (!WindowShouldClose()) {
      BeginDrawing();
      ClearBackground(WHITE);
      DrawText("Hello, Raylib!", 10, 10, 50, BLACK);
      EndDrawing();
   }
}
