#include "util.hpp"

// Lit le contenu d'un fichier, le crée s'il n'existe pas
std::string readFile(const std::string& filePath) {
    // Crée le fichier s'il n'existe pas
    if (!std::filesystem::exists(filePath)) {
        std::ofstream createFile(filePath);
        if (!createFile) {
            std::cerr << "Erreur lors de la création du fichier: " << filePath << std::endl;
        }
        return "";
    }

    // Lit le contenu du fichier
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Erreur d'ouverture du fichier: " << filePath << std::endl;
        return "";
    }

    // Lit tout le contenu
    std::string content(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    return content;
}

// Remplace le contenu d'un fichier
void writeFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath, std::ios::binary | std::ios::trunc);
    if (!file) {
        std::cerr << "Erreur d'écriture dans le fichier: " << filePath << std::endl;
        return;
    }

    file.write(content.c_str(), content.size());
    if (!file.good()) {
        std::cerr << "Erreur lors de l'écriture dans le fichier: " << filePath << std::endl;
    }
}
