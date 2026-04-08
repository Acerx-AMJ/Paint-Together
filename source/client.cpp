#include "packet.hpp"
#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

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

   if (send.joinable())    send.join();
   if (receive.joinable()) receive.join();

   sf::Packet endPacket;
   endPacket << PTdisconnect << id;
   socket.send(endPacket, "127.0.0.1", port);
   
   printf("Disconnected.\n");
   socket.unbind();
}

void sendMessages(sf::UdpSocket &socket) {
   while (true) {
      // printf("Input your message ('exit' to exit): ");
      // std::string message;
      // std::getline(std::cin >> std::ws, message);

      // if (message == "exit") {
      //    quit = true;
      //    printf("Exiting...\n");
      //    return;
      // }

      // printf("%s\n", message.c_str());
      // sf::Packet packet;
      // packet << message;

      // std::lock_guard<std::mutex> lock(messageMutex);
      // socket.send(packet, "127.0.0.1", port);
   }
}

void receiveMessages(sf::UdpSocket &socket) {
   while (!quit) {
      // sf::Packet packet;
      // std::lock_guard<std::mutex> lock(messageMutex);
      // sf::IpAddress sender;
      // unsigned short senderPort;

      // if (socket.receive(packet, sender, senderPort) == sf::Socket::Status::Done) {
      //    std::string message;
      //    packet >> message;

      //    clearLine(0);
      //    printf("%s\n", message.c_str());
      //    printf("Input your message ('exit' to exit): ");
      // }
   }
}

void gameLoop() {

}
