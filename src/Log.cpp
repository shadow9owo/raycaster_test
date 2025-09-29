#pragma once

#include <string>
#include <fstream>

void Log(const std::string& message)
{
    std::ofstream file("log.txt", std::ios::app);
    if (file.is_open())
    {
        file << message << "\n";
    }
}