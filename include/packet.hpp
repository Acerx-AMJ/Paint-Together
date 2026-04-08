#ifndef PACKET_HPP
#define PACKET_HPP

#include <SFML/Network.hpp>

using PacketType = short;
constexpr PacketType PTupdate = 0,
                     PTconnect = 1,
                     PTdisconnect = 2;

struct StrokeEvent {
   float x, y, size;
   int id, color;
   bool drawing;
};

inline sf::Packet &operator << (sf::Packet &packet, const StrokeEvent &e) {
   return packet << PTupdate << e.id << e.color << e.x << e.y << e.size << e.drawing;
}

// Id and packet type must be checked beforehand
inline sf::Packet &operator >> (sf::Packet &packet, StrokeEvent &e) {
   return packet >> e.color >> e.x >> e.y >> e.size >> e.drawing;
}

#endif
