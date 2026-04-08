#include "packet.hpp"
#include <atomic>
#include <iostream>
#include <mutex>
#include <raylib.h>
#include <thread>

struct Point {
   float x, y, size;
};

std::vector<Point> queue;
std::atomic<bool> quit = false;
std::mutex messageMutex;

float brushSize = 5.0f;
unsigned short port = 0;
int id = 0;

void sendMessages(sf::UdpSocket &socket);
void receiveMessages(sf::UdpSocket &socket);
void gameLoop();

// Main

int main() {
   printf("Input server's port: ");
   std::cin >> port;

   sf::UdpSocket socket;
   if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Status::Done) {
      printf("Could not connect to port %d!\n", port);
      exit(EXIT_FAILURE);
   }

   sf::Packet helloPacket;
   helloPacket << PTconnect << 0;
   socket.send(helloPacket, "127.0.0.1", port);

   sf::Packet startingPacket;
   sf::IpAddress address;
   socket.receive(startingPacket, address, port);
   startingPacket >> id;

   socket.setBlocking(false);
   printf("Connected to the server!\n");

   std::thread send(sendMessages, std::ref(socket));
   std::thread receive(receiveMessages, std::ref(socket));
   std::thread game(gameLoop);

   if (send.joinable())    send.join();
   if (receive.joinable()) receive.join();
   if (game.joinable())    game.join();

   sf::Packet endPacket;
   endPacket << PTdisconnect << id;
   socket.send(endPacket, "127.0.0.1", port);
   
   printf("Disconnected.\n");
   socket.unbind();
}

void sendMessages(sf::UdpSocket &socket) {
   while (!quit) {
      sf::Packet packet;
      StrokeEvent event {GetMousePosition().x, GetMousePosition().y, brushSize, id, IsMouseButtonDown(MOUSE_BUTTON_LEFT)};
      packet << event;

      std::lock_guard<std::mutex> lock(messageMutex);
      socket.send(packet, "127.0.0.1", port);
   }
}

void receiveMessages(sf::UdpSocket &socket) {
   while (!quit) {
      sf::Packet packet;
      std::lock_guard<std::mutex> lock(messageMutex);

      sf::IpAddress sender;
      unsigned short senderPort;

      if (socket.receive(packet, sender, senderPort) == sf::Socket::Status::Done) {
         PacketType type;
         packet >> type;

         int id;
         packet >> id;

         StrokeEvent event;
         packet >> event;

         if (event.drawing) {
            queue.push_back({event.x, event.y, event.size});
         }
      }
   }
}

void gameLoop() {
   SetConfigFlags(FLAG_VSYNC_HINT);
   InitWindow(800, 600, "Paint Together");   
   RenderTexture2D target = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

   while (!WindowShouldClose()) {
      BeginTextureMode(target);
      for (const Point &point: queue) {
         DrawCircle(point.x, point.y, point.size, BLACK);
      }
      queue.clear();

      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
         DrawCircle(GetMouseX(), GetMouseY(), brushSize, BLACK);
      }
      EndTextureMode();

      BeginDrawing();
      ClearBackground(WHITE);
      DrawTextureRec(target.texture, {0, 0, (float)target.texture.width, (float)-target.texture.height}, {0, 0}, WHITE);
      DrawCircle(GetMouseX(), GetMouseY(), brushSize, BLACK);
      EndDrawing();
   }

   UnloadRenderTexture(target);
   CloseWindow();
   quit = true;
}
