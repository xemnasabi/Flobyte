#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "PacketManager.h"
#include <boost/version.hpp>

using boost::asio::ip::tcp;


class LoginServer : public std::enable_shared_from_this<LoginServer> {
public:
    LoginServer(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        start_accept();
    }

private:
    void start_accept() {
        auto new_connection = std::make_shared<tcp::socket>(acceptor_.get_executor());
        acceptor_.async_accept(*new_connection, [this, new_connection](const boost::system::error_code& error) {
            if (!error) {
                std::cout << "New client connected!" << std::endl;
                send_packet_in_hex(new_connection, "14000001d1259eff9adee037b913a7533c8908c7");
                std::cout << "Sending Challenge Request Response to Client" << std::endl;

            }
            else {
                std::cerr << "Accept error: " << error.message() << std::endl;
            }
            start_accept(); // Starte die nächste Accept-Operation
            });
    }

    void send_packet_in_hex(std::shared_ptr<tcp::socket> socket, const std::string& hex_stream) {
        try {

            if (hex_stream.length() % 2 != 0) {
                throw std::invalid_argument("Hex stream length must be even.");
            }

            auto data = std::make_shared<std::vector<unsigned char>>();
            for (size_t i = 0; i < hex_stream.length(); i += 2) {
                std::string byte_string = hex_stream.substr(i, 2);
                if (!std::isxdigit(byte_string[0]) || !std::isxdigit(byte_string[1])) {
                    throw std::invalid_argument("Hex stream contains invalid characters.");
                }
                unsigned char byte = static_cast<unsigned char>(std::stoi(byte_string, nullptr, 16));
                data->push_back(byte);
            }

            boost::asio::async_write(*socket, boost::asio::buffer(*data),
                [this, socket, data](const boost::system::error_code& error, std::size_t /*bytes_transferred*/) {
                    if (!error) {
                        std::cout << "Initial packet sent to client." << std::endl;
                        handle_client(socket); // Begin receiving data after sending the packet
                    }
                    else {
                        std::cerr << "Write edrror: " << error.message() << std::endl;
                    }
                });
        }
        catch (const std::exception& e) {
            std::cerr << "Error in send_packet: " << e.what() << std::endl;
        }
    }

    void handle_client(std::shared_ptr<tcp::socket> socket) {
        auto self = shared_from_this();
        auto buffer = std::make_shared<std::vector<char>>(4200);

        socket->async_read_some(boost::asio::buffer(*buffer),
            [this, socket, buffer, self](const boost::system::error_code& error, std::size_t bytes_transferred) {
                if (!error) {
                    buffer->resize(bytes_transferred);
                    std::cout << "Received packet: "
                        << PacketManager::hex_dump(buffer->data(), bytes_transferred) << std::endl;

                    auto response = PacketManager::process_packet(buffer->data(), bytes_transferred);
                    if (!response.empty()) {
                        auto response_data = std::make_shared<std::vector<unsigned char>>(response.begin(), response.end());
                        boost::asio::async_write(*socket, boost::asio::buffer(*response_data),
                            [socket, response_data](const boost::system::error_code& write_error, std::size_t /*bytes_written*/) {
                                if (!write_error) {
                                    std::cout << "Response sent to client." << std::endl;
                                }
                                else {
                                    std::cerr << "Write error: " << write_error.message() << std::endl;
                                }
                            });
                    }
                    handle_client(socket); // Continue reading data from the client
                }
                else {
                    std::cout << "Client disconnected: " << error.message() << std::endl;
                }
            });
    }

    tcp::acceptor acceptor_;
};

int main() {
    try {
        boost::asio::io_context io_context;
        short port = 3724;
        std::cout << "Florensia Private Login Server written in C++ :)" << std::endl;
        std::cout << "Flobyte Version: 1.0.0 | Boost version: " << BOOST_LIB_VERSION << std::endl;
        std::cout << "Starting Login Server on port " << port << "..." << std::endl;
        auto server = std::make_shared<LoginServer>(io_context, port);
        std::cout << "Waiting for connections ....." << std::endl;

        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
