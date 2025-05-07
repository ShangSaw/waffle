#include "util.hpp"

std::vector<std::string> split_string(const std::string& str, char delim) {
      std::vector<std::string> tokens;
      std::stringstream ss(str);
      std::string token;
      while (std::getline(ss, token, delim)) {
          if (!token.empty()) tokens.push_back(token);
      }
      return tokens;
}