// { Driver Code Starts
// Initial template for C++

/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/



#include <iostream>
#include <fstream>

// We'll use Protocol Buffers for serialization/deserialization.
#include "example.pb.h"  // Generated file from the proto file

// Our data struct that we want to serialize/deserialize
struct Person {
    std::string name;
    int32_t id;
    double height;
};

// Function to serialize data using Protocol Buffers
void SerializeData(const Person& data, std::string& serialized_data) {
    tutorial::Person person;
    person.set_name(data.name);
    person.set_id(data.id);
    person.set_height(data.height);

    serialized_data.clear();
    if (!person.SerializeToString(&serialized_data)) {
        std::cerr << "Failed to serialize data." << std::endl;
    }
}

// Function to deserialize data using Protocol Buffers
void DeserializeData(const std::string& serialized_data, Person* data) {
    tutorial::Person person;
    if (!person.ParseFromString(serialized_data)) {
        std::cerr << "Failed to deserialize data." << std::endl;
        return;
    }
    data->name = person.name();
    data->id = person.id();
    data->height = person.height();
}

int main() {
    Person person_data = {
        .name = "Alice",
        .id = 1,
        .height = 178.5,
    };

    // **Serialization:**
    std::string serialized_data;
    SerializeData(person_data, serialized_data);

    // Save serialized data to a file (optional)
    std::ofstream file("person.dat", std::ios::binary);
    file.write(serialized_data.data(), serialized_data.size());
    file.close();

    // **Deserialization:**
    Person deserialized_person;
    DeserializeData(serialized_data, &deserialized_person);

    // Print the deserialized data
    std::cout << "Deserialized Data: " << std::endl;
    std::cout << "Name: " << deserialized_person.name << std::endl;
    std::cout << "ID: " << deserialized_person.id << std::endl;
    std::cout << "Height: " << deserialized_person.height << std::endl;

    return 0;
}
