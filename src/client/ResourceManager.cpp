#include "ResourceManager.hpp"

static size_t WriteFileCallback(void* ptr, size_t size, size_t nmemb, void* stream) {
    std::ofstream* out = static_cast<std::ofstream*>(stream);
    size_t totalSize = size * nmemb;
    out->write(static_cast<char*>(ptr), totalSize);
    return totalSize;
}

ResourceManager::ResourceManager(SDL_Renderer* ren) : ren_(ren) {}

ResourceManager::~ResourceManager() {
	for (auto& [_, tex] : cache_) SDL_DestroyTexture(tex);
    cache_.clear();
}

SDL_Texture* ResourceManager::get(const std::string& key) {
	auto it = cache_.find(key);
	return it == cache_.end() ? nullptr : it->second;
}

bool ResourceManager::downloadPNG(const std::string& url, const std::string& outputFilePath) {
    // Vérifier si l'URL se termine bien par .png
    if (url.size() < 4 || url.substr(url.size() - 4) != ".png") {
        std::cerr << "Erreur : L'URL ne pointe pas vers une image PNG." << std::endl;
        return false;
    }

    CURL* curl;
    CURLcode res;
    bool success = false;

    curl = curl_easy_init();
    if (curl) {
        std::ofstream outFile(outputFilePath, std::ios::binary);
        if (!outFile.is_open()) {
            std::cerr << "Erreur : Impossible d'ouvrir le fichier de destination." << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Suivre les redirections
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            success = true;
        }
        else {
            std::cerr << "Erreur CURL : " << curl_easy_strerror(res) << std::endl;
        }

        outFile.close();
        curl_easy_cleanup(curl);
    }

    cache_[outputFilePath] = loadTexture(outputFilePath);

    return success;
}

SDL_Texture* ResourceManager::loadTexture(const std::string& path) {
    SDL_Surface* surf = IMG_Load(path.c_str());
    if (!surf) {
        std::cerr << "IMG_Load failed: " << SDL_GetError() << "\n";
        return nullptr;
    }
    SDL_Texture* t = SDL_CreateTextureFromSurface(ren_, surf);
    SDL_DestroySurface(surf);
    return t;
}