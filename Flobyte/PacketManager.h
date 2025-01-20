#ifndef PACKETMANAGER_H
#define PACKETMANAGER_H

#include <string>
#include <vector>

class PacketManager {
public:
    static std::vector<char> process_packet(const char* data, std::size_t length);
    static std::string hex_dump(const char* data, std::size_t length);
};

#endif // PACKETMANAGER_H