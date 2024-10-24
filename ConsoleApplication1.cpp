// traffic_profiler.cpp

#include <iostream>
#include <fstream>
#include <string>

#include "traffic_profile.pb.h"

int main() {
    // Read traffic profile data from a file (you can replace this with your actual network traffic data processing)
    std::ifstream input("traffic_profile.pb");
    if (!input.is_open()) {
        std::cerr << "Failed to open input file: traffic_profile.pb" << std::endl;
        return 1;
    }

    // Deserialize the traffic profile from the file
    TrafficProfile profile;
    if (!profile.ParseFromIstream(&input)) {
        std::cerr << "Failed to parse traffic profile from file." << std::endl;
        return 1;
    }

    // Now you can access the traffic profile data efficiently
    std::cout << "Source IP: " << profile.source_ip() << std::endl;
    std::cout << "Destination IP: " << profile.destination_ip() << std::endl;
    std::cout << "Source Port: " << profile.source_port() << std::endl;
    std::cout << "Destination Port: " << profile.destination_port() << std::endl;
    std::cout << "Packet Count: " << profile.packet_count() << std::endl;
    std::cout << "Byte Count: " << profile.byte_count() << std::endl;
    std::cout << "Average Packet Size: " << profile.average_packet_size() << std::endl;

    return 0;
}