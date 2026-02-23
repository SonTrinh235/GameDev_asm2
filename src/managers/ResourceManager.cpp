#include "../../include/managers/ResourceManager.h"

ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

bool ResourceManager::loadTexture(SDL_Renderer* renderer, const std::string& id, const std::string& path) {
    if (textures.find(id) != textures.end()) return true;

    SDL_Surface* surface = SDL_LoadBMP(path.c_str());
    if (!surface) return false;

    SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGB(surface, 0, 0, 0));
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (texture) {
        textures[id] = texture;
        return true;
    }
    return false;
}

SDL_Texture* ResourceManager::getTexture(const std::string& id) {
    if (textures.find(id) != textures.end()) {
        return textures[id];
    }
    return nullptr;
}

void ResourceManager::clean() {
    for (auto& pair : textures) {
        SDL_DestroyTexture(pair.second);
    }
    textures.clear();
}