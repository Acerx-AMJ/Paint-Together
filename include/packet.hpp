#ifndef PACKET_HPP
#define PACKET_HPP

#include <SFML/Network.hpp>

using PacketType = short;
constexpr PacketType PTupdate = 0,
                     PTconnect = 1,
                     PTdisconnect = 2;

struct StrokeEvent {
   float x, y, size;

   sf::Packet &operator << (sf::Packet &packet) const {
      return packet << PTupdate << x << y << size;      
   }

   sf::Packet &operator >> (sf::Packet &packet) {
      PacketType t;
      return packet >> t >> x >> y >> size;
   }
};

#endif
