#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>

class ResourceManager {
public:
    static ResourceManager& getInstance();
    
    bool loadTexture(SDL_Renderer* renderer, const std::string& id, const std::string& path);
    SDL_Texture* getTexture(const std::string& id);
    void clean();

private:
    ResourceManager() {}
    std::unordered_map<std::string, SDL_Texture*> textures;
};