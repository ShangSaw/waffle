#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

typedef struct Coordonnees {
    float x, y;
} Coordonnees;

std::string readFile(const std::string& filePath);
void writeFile(const std::string& filePath, const std::string& content);