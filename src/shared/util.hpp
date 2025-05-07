#pragma once
#include <vector>
#include <string>
#include <sstream>

typedef struct Coordonnees {
    float x, y;
} Coordonnees;

std::vector<std::string> split_string(const std::string& str, char delim = '|');