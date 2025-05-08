#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "include/json.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

// Helper function to get current timestamp
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

struct Packet {
    std::string symbol;
    char buysellindicator;
    int32_t quantity;
    int32_t price;
    int32_t packetSequence;
};

class ABXClient {
private:
    SOCKET sockfd;
    struct sockaddr_in servaddr;
    std::map<int32_t, Packet> receivedPackets;
    
    bool connectToServer() {
        // Initialize Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            logError("WSAStartup failed");
            return false;
        }

        // Create socket
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            logError("Socket creation failed");
            WSACleanup();
            return false;
        }

        // Set server address
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(3000);
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Use localhost for local testing

        // Connect to server
        if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
            logError("Connection failed");
            closesocket(sockfd);
            WSACleanup();
            return false;
        }

        return true;
    }

    Packet parsePacket(const std::vector<uint8_t>& buffer) {
        Packet packet;
        size_t offset = 0;

        // Read symbol (4 bytes)
        packet.symbol = std::string(buffer.begin() + offset, buffer.begin() + offset + 4);
        offset += 4;

        // Read buysellindicator (1 byte)
        packet.buysellindicator = buffer[offset];
        offset += 1;

        // Read quantity (4 bytes)
        packet.quantity = ntohl(*(int32_t*)(buffer.data() + offset));
        offset += 4;

        // Read price (4 bytes)
        packet.price = ntohl(*(int32_t*)(buffer.data() + offset));
        offset += 4;

        // Read packetSequence (4 bytes)
        packet.packetSequence = ntohl(*(int32_t*)(buffer.data() + offset));

        return packet;
    }

    void logError(const std::string& message) {
        std::ofstream logFile("abx_client.log", std::ios::app);
        logFile << "[" << getCurrentTimestamp() << "] " << message << std::endl;
    }

public:
    ABXClient() : sockfd(INVALID_SOCKET) {}

    ~ABXClient() {
        if (sockfd != INVALID_SOCKET) {
            closesocket(sockfd);
            WSACleanup();
        }
    }

    bool start() {
        auto start = std::chrono::high_resolution_clock::now();
        
        if (!connectToServer()) {
            return false;
        }

        // Request stream of packets (Type 1)
        uint8_t requestType1[] = {0x01, 0x00};
        if (send(sockfd, (char*)requestType1, sizeof(requestType1), 0) == SOCKET_ERROR) {
            logError("Failed to send request");
            return false;
        }

        // Receive and process packets
        std::vector<uint8_t> buffer(17); // Packet size is 17 bytes
        while (true) {
            int bytesRead = recv(sockfd, (char*)buffer.data(), buffer.size(), 0);
            if (bytesRead <= 0) break;

            Packet packet = parsePacket(buffer);
            receivedPackets[packet.packetSequence] = packet;
        }

        // Close the connection after receiving stream
        closesocket(sockfd);
        sockfd = INVALID_SOCKET;

        // Request missing packets
        for (int32_t seq = 1; seq <= 14; ++seq) {
            if (receivedPackets.find(seq) == receivedPackets.end()) {
                // Create new connection for each request
                if (!connectToServer()) {
                    continue;
                }

                uint8_t requestType2[] = {0x02, static_cast<uint8_t>(seq)};
                if (send(sockfd, (char*)requestType2, sizeof(requestType2), 0) == SOCKET_ERROR) {
                    logError("Failed to send request for sequence " + std::to_string(seq));
                    closesocket(sockfd);
                    sockfd = INVALID_SOCKET;
                    continue;
                }

                int bytesRead = recv(sockfd, (char*)buffer.data(), buffer.size(), 0);
                if (bytesRead > 0) {
                    Packet packet = parsePacket(buffer);
                    receivedPackets[packet.packetSequence] = packet;
                }

                closesocket(sockfd);
                sockfd = INVALID_SOCKET;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Processing time: " << duration.count() << "ms" << std::endl;
        
        return true;
    }

    void saveToJson(const std::string& filename) {
        JsonValue jsonArray = Json::array();
        
        for (const auto& pair : receivedPackets) {
            const Packet& packet = pair.second;
            JsonValue packetObj = Json::object();
            packetObj["symbol"] = packet.symbol;
            packetObj["buysellindicator"] = std::string(1, packet.buysellindicator);
            packetObj["quantity"] = packet.quantity;
            packetObj["price"] = packet.price;
            packetObj["packetSequence"] = packet.packetSequence;
            
            jsonArray.push_back(packetObj);
        }

        std::ofstream file(filename);
        file << jsonArray.toString();
    }
};

int main() {
    ABXClient client;
    if (client.start()) {
        client.saveToJson("abx_packets.json");
        std::cout << "Packets saved to abx_packets.json" << std::endl;
    } else {
        std::cerr << "Failed to connect to server" << std::endl;
        return 1;
    }
    return 0;
} 