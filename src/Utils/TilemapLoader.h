#ifndef TILEMAPLOADER_H
#define TILEMAPLOADER_H

#include <string>
#include <vector>
#include "glm/glm.hpp"
#include "SDL2/SDL_image.h"

struct Tile {
    glm::ivec2 relativePosition;
    glm::ivec2 pixelSrcPosition;
};

using Map = std::vector<Tile>;
class TileMapLoader {
    public:
        explicit TileMapLoader(const std::string& fileMap, const std::string& filePng, int tileSize=32);
        ~TileMapLoader();
        const Map& getMap() const;
        const glm::ivec2 getMapResolution() const;

    private:
        void loadFilemap(const std::string& fileMap);
        void loadImg(const::std::string& filePng);
        int tileSize;
        glm::ivec2 pixelPosFromTilePos(int tilePos);
        Map map;
        glm::ivec2 mapResolution;
        SDL_Surface* mapSurface;

};

#endif