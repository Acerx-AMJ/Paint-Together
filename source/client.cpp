#include "packet.hpp"
#include <iostream>
#include <raylib.h>

// Some setup...

constexpr int colorCount = 15;
constexpr Color colors[colorCount] {
  {255, 0, 0, 255},     // rgb(255, 0, 0)
  {0, 255, 0, 255},     // rgb(0, 255, 0)
  {0, 0, 255, 255},     // rgb(0, 0, 255)
  {0, 255, 255, 255},   // rgb(0, 255, 255)
  {255, 0, 255, 255},   // rgb(255, 0, 255)
  {255, 255, 0, 255},   // rgb(255, 255, 0)
  {0, 255, 125, 255},   // rgb(0, 255, 125)
  {0, 125, 255, 255},   // rgb(0, 125, 255)
  {125, 0, 255, 255},   // rgb(125, 0, 255)
  {255, 0, 125, 255},   // rgb(255, 0, 125)
  {255, 125, 0, 255},   // rgb(255, 125, 0)
  {125, 255, 0, 255},   // rgb(125, 255, 0)
  {255, 255, 255, 255}, // rgb(255, 255, 255)
  {125, 125, 125, 255}, // rgb(125, 125, 125)
  {0, 0, 0, 255},       // rgb(0, 0, 0)
};

// Main

int main() {
   std::vector<Point> queue;
   queue.reserve(512);
   
   float brushSize = 5.0f;
   sf::IpAddress IPAddress;
   unsigned short port = 0;
   int id = 0;
   int color = 0;

   do {
      printf("Input server's IP: ");
      std::string IP;
      std::getline(std::cin >> std::ws, IP);
      IPAddress = sf::IpAddress(IP);
      if (IPAddress.toInteger() == 0) {
         printf("Invalid IP address format, try again. ");
      }
   }
   while (IPAddress.toInteger() == 0);

   printf("Input server's port: ");
   std::cin >> std::ws >> port;

   // Use TCP to retrieve the initial information such as ID and stroke history
   sf::TcpSocket tcp;
   if (tcp.connect(IPAddress, port + 1) != sf::Socket::Status::Done) {
      printf("Could not connect TCP to port %d (%d for UDP).\n", port + 1, port);
      exit(EXIT_FAILURE);
   }

   sf::Packet info;
   tcp.receive(info);
   info >> id;

   int historySize;
   info >> historySize;
   queue.resize(historySize);

   for (Point &stroke: queue) {
      info >> stroke;
   }
   tcp.disconnect();

   sf::UdpSocket socket;
   if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Status::Done) {
      printf("Could not connect to port %d!\n", port);
      exit(EXIT_FAILURE);
   }

   socket.setBlocking(false);
   printf("Connected to the server!\n");

   SetConfigFlags(FLAG_VSYNC_HINT);
   InitWindow(800, 600, "Paint Together");
   RenderTexture2D target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

   while (!WindowShouldClose()) {
      // Send UDP

      sf::Packet packet;
      StrokeEvent event {GetMousePosition().x, GetMousePosition().y, brushSize, id, color, IsMouseButtonDown(MOUSE_BUTTON_LEFT)};
      packet << event;
      socket.send(packet, IPAddress, port);

      while (true) {
         sf::Packet packet;
         sf::IpAddress sender;
         unsigned short senderPort;

         if (socket.receive(packet, sender, senderPort) != sf::Socket::Status::Done) {
            break;
         }
         PacketType type;
         packet >> type;

         int id;
         packet >> id;

         StrokeEvent event;
         packet >> event;

         if (event.drawing) {
            queue.push_back({event.x, event.y, event.size, event.color});
         }
      }

      // Update controls
      brushSize += GetMouseWheelMove() * 5;
      brushSize = std::clamp(brushSize, 2.0f, 50.0f);

      if (IsKeyPressed(KEY_LEFT)) {
         color = (color == 0 ? colorCount - 1 : color - 1);
      }

      if (IsKeyPressed(KEY_RIGHT)) {
         color = (color + 1) % colorCount;
      }

      // Painting
      BeginTextureMode(target);
      for (const Point &point: queue) {
         DrawCircle(point.x, point.y, point.size, colors[point.color]);
      }
      queue.clear();

      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
         DrawCircle(GetMouseX(), GetMouseY(), brushSize, colors[color]);
      }
      EndTextureMode();

      // Draw the application
      BeginDrawing();
      ClearBackground(WHITE);
      DrawTextureRec(target.texture, {0, 0, (float)target.texture.width, (float)-target.texture.height}, {0, 0}, WHITE);
      DrawCircle(GetMouseX(), GetMouseY(), brushSize, colors[color]);

      // Draw UI
      constexpr float padding = 5.0f;
      constexpr float linewidth = 2.0f;
      float square = (GetScreenWidth() - padding * (colorCount + 1)) / colorCount;
      float posX = padding;

      DrawRectangle(0, 0, GetScreenWidth(), square + padding * 2, WHITE);
      for (int i = 0; i < colorCount; ++i) {
         DrawRectangleLinesEx({posX - linewidth, padding - linewidth, square + linewidth * 2, square + linewidth * 2}, linewidth, i == color ? YELLOW : BLACK);
         DrawRectangle(posX, padding, square, square, colors[i]);

         if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), {posX, padding, square, square})) {
            color = i;
         }
         posX += padding + square;
      }
      EndDrawing();
   }

   UnloadRenderTexture(target);
   CloseWindow();

   sf::Packet endPacket;
   endPacket << PTdisconnect << id;
   socket.send(endPacket, IPAddress, port);
   
   printf("Disconnected.\n");
   socket.unbind();
}
