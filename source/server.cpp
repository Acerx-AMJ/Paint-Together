#include "packet.hpp"
#include <unordered_map>

struct Client {
   sf::IpAddress ip;
   unsigned short port;
};

// Helper functions

int nextFreeSpot(const std::unordered_map<int, Client> &clients) {
   for (int i = 0;; ++i)
      if (clients.find(i) == clients.end())
         return i;
}

// Main

int main() {
   // UDP
   sf::UdpSocket socket;
   if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Status::Done) {
      printf("Could not bind to port %d!\n", socket.getLocalPort());
      return EXIT_FAILURE;
   }

   printf("Server listening on %s:%d...\n", sf::IpAddress::getLocalAddress().toString().c_str(), socket.getLocalPort());
   std::unordered_map<int, Client> clients;

   // TCP (initial)
   std::vector<Point> history; // for sending it to the new clients
   history.reserve(512);

   sf::TcpListener listener;
   listener.listen(socket.getLocalPort() + 1);
   listener.setBlocking(false);

   sf::SocketSelector selector;
   selector.add(listener);
   selector.add(socket);

   // Server loop
   while (true) {
      if (selector.wait()) {
         if (selector.isReady(listener)) {
            sf::TcpSocket tcp;
            listener.accept(tcp);

            int newId = nextFreeSpot(clients);
            sf::Packet info;
            info << newId << (int)history.size();
            for (const Point &stroke: history) {
               info << stroke;
            }

            tcp.send(info);
            printf("Player %d joined.\n", newId);
            clients[newId] = Client{tcp.getRemoteAddress(), 0};
            tcp.disconnect();
         }
         else if (selector.isReady(socket)) {
            sf::Packet packet;
            sf::IpAddress senderIp;
            unsigned short senderPort;

            if (socket.receive(packet, senderIp, senderPort) != sf::Socket::Status::Done) {
               continue;
            }

            PacketType data;
            int id;
            packet >> data >> id;

            if (data == PTdisconnect) {
               printf("Player %d disconnected.\n", id);
               clients.erase(id);
            }
            else if (data == PTupdate) {
               Point point;
               packet >> point.color >> point.x >> point.y >> point.size;

               bool drawing;
               packet >> drawing;
               if (drawing) {
                  history.push_back(point);
               }

               for (auto &[cid, client]: clients) {
                  if (client.port == 0 && cid == id) {
                     client.port = senderPort;
                     printf("Player %d UDP port registered.\n", cid);
                  }
               }

               for (auto &[cid, client]: clients) {
                  if (client.port != 0 && (client.port != senderPort || client.ip != senderIp)) {
                     socket.send(packet, client.ip, client.port);
                  }
               }
            }
         }
      }
   }
}