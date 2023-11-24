#pragma once

#include <iostream>
#include <sstream>

// Use a helper class for atomic std::cout streaming.
class Writer{
    std::ostringstream buffer;
public:
    ~Writer(){
        std::cout << buffer.str() << std::endl;
    }

    Writer& operator << (auto input){
        buffer << input;
        return *this;
    }
};
