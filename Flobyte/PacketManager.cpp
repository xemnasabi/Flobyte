#include "PacketManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cstdint>

std::vector<char> hex_to_char_vector(const std::string& hex_stream) {
    // �berpr�fen, ob die L�nge des Hex-Streams gerade ist
    if (hex_stream.length() % 2 != 0) {
        throw std::invalid_argument("Hex stream length must be even.");
    }

    std::vector<char> char_vector;

    for (size_t i = 0; i < hex_stream.length(); i += 2) {
        std::string byte_string = hex_stream.substr(i, 2);

        // �berpr�fen, ob die Zeichen g�ltige Hexadezimalzeichen sind
        if (!std::isxdigit(byte_string[0]) || !std::isxdigit(byte_string[1])) {
            throw std::invalid_argument("Hex stream contains invalid characters.");
        }

        // Konvertiere Hexadezimalzeichen in ein Byte
        char byte = static_cast<char>(std::stoi(byte_string, nullptr, 16));
        char_vector.push_back(byte);
    }

    return char_vector;
}

std::vector<char> PacketManager::process_packet(const char* data, std::size_t length) {
    std::vector<char> response;

    if (length < 4) {
        std::cerr << "Invalid packet received." << std::endl;
        return response;
    }

    uint16_t opcode = (data[0] << 8) | (data[1]);
    std::cout << "Processing opcode: 0x" << std::hex << opcode << std::dec << std::endl;

    switch (opcode) {
    case 0x3a02: { 
        //Received packet: 0c 00 00 02 46 df 59 bf d8 53 1d 11
        std::cout << "Request Login" << std::endl;
        response = hex_to_char_vector("350002024dd263cfdad39668bc7444164dd263cfaff0ae517d411c32c111433036e0dab2c7040741007ed9e1b44180c7c079b2b6d4");
        std::cout << "Sending Login Success" << std::endl;

        break;
    }
    case 0x1100: { 
        std::cout << "Request Serverlist" << std::endl;
        response = hex_to_char_vector("c00002044dd263cfdad39668bc7444164dd263cfdb23360d49d2a9324f13a227cf4f03fc9d0779466e0d9e1feba400688ef38db0af3d4b0b081e1357ef8311643e3cd45254bf3fd970fecaa037d9ac3887b2890763041f65d2e0ac4b0b1284d12761c6ee1fe123c54f17f02083b21d927f5e5582c7477297e26f03302f5c1629e24f01eaecca36894755230ae4a2dc92ddb1382cf369e531de1c1907a86e0c919d324022a6916695ed1c12935e960eaf07c64ded9ce3771b6c7020d7c64a023d");
        std::cout << "Sending Serverlist" << std::endl;

            break;
    }
    case 0x1200: {
        std::cout << "Request World Server" << std::endl;
        response = hex_to_char_vector("1e0002064dd263cfca84e22f9187b7bb4dd263cfdd9cd489f5df7389ed79");
        std::cout << "Sending World Server IP" << std::endl;

        break;
    }
    default:
        std::cerr << "Unknown opcode: 0x" << std::hex << opcode << std::dec << std::endl;
        break;
    }

    return response;
}

std::string PacketManager::hex_dump(const char* data, std::size_t length) {
    std::ostringstream oss;
    for (std::size_t i = 0; i < length; ++i) {
        if (i % 16 == 0 && i != 0) {
            oss << "\n";
        }
        oss << std::hex << std::setw(2) << std::setfill('0') << (static_cast<unsigned>(data[i]) & 0xFF) << " ";
    }
    return oss.str();
}