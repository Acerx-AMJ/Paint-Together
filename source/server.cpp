#include "packet.hpp"
#include <iostream>
#include <unordered_map>

struct Client {
   sf::IpAddress ip;
   unsigned short port;

   bool operator == (const Client &other) const {
      return ip == other.ip && port == other.port;
   }
   
   bool operator != (const Client &other) const {
      return !(*this == other);
   }
};

// Helper functions

int nextFreeSpot(const std::unordered_map<int, Client> &clients) {
   for (int i = 0;; ++i)
      if (clients.find(i) == clients.end())
         return i;
}

// Main

int main() {
   unsigned short port = 0;
   printf("Input server port: ");
   std::cin >> port;

   sf::UdpSocket socket;
   if (socket.bind(port) != sf::Socket::Status::Done) {
      printf("Could not bind to port %d!\n", port);
      return EXIT_FAILURE;
   }

   printf("Server listening on port %d...\n", port);
   std::unordered_map<int, Client> clients;

   while (true) {
      sf::Packet packet;
      sf::IpAddress senderIp;
      unsigned short senderPort;

      if (socket.receive(packet, senderIp, senderPort) != sf::Socket::Status::Done) {
         continue;
      }

      Client sender {senderIp, senderPort};
      PacketType data;
      int id;
      packet >> data >> id;

      if (data == PTconnect) {
         int id = nextFreeSpot(clients);

         clients[id] = sender;
         printf("Player %d joined %s:%d\n", id, senderIp.toString().c_str(), senderPort);

         sf::Packet info;
         info << id;
         socket.send(info, sender.ip, sender.port);
      }

      if (data == PTdisconnect) {
         printf("Player %d disconnected.\n", id);
         clients.erase(id);
         continue;
      }

      if (data == PTupdate) {

      }

      // sf::Packet echoPacket;
      // echoPacket << data;
      // for (const auto &[id, client]: clients) {
      //    if (client != sender) {
      //       socket.send(echoPacket, client.ip, client.port);
      //    }
      // }
   }
}