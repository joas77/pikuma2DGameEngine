#include <fstream>
#include <iostream>
#include <sstream>
#include "TilemapLoader.h"
#include "../Logger/Logger.h"

TileMapLoader::TileMapLoader(const std::string& fileMap, const std::string& filePng, int tileSize) : 
    tileSize{tileSize}, mapSurface{nullptr}
{
    loadImg(filePng);
    loadFilemap(fileMap);
}

TileMapLoader::~TileMapLoader() {
    if(mapSurface)
        SDL_free(mapSurface);
}

void TileMapLoader::loadImg(const::std::string& filePng) {
    mapSurface = IMG_Load(filePng.c_str());
    if(!mapSurface) {
        Logger::Err("Error loadgin PNG file maybe a wrong path provided!");
    }
    mapResolution.x = mapSurface->w;
    mapResolution.y = mapSurface->h;
}

void TileMapLoader::loadFilemap(const std::string& fileMap) {
    std::ifstream fmap{fileMap};
    std::string line;
    int y=0;
    if (fmap.is_open()) {
        while (fmap >> line) {
            std::stringstream s{line};
            std::string strNum;
            int x = 0;
            while (std::getline(s, strNum, ',')) {
                auto relativePosition = glm::ivec2(x, y);
                auto pixPosition = pixelPosFromTilePos(std::stoi(strNum));
                map.push_back(Tile{relativePosition, pixPosition});
                x++;
            }
            y++;
        }  
    }
}

glm::ivec2 TileMapLoader::pixelPosFromTilePos(int tilePos) {
    const int tilesCountHorizontal = mapResolution.x / tileSize;
    // const int tilesCountVertical  = mapResolution.y /tileSize;
    const int row = tilePos / tilesCountHorizontal;
    const int column = tilePos % tilesCountHorizontal;

    return glm::ivec2( column * tileSize, row * tileSize ); 
}

const Map& TileMapLoader::getMap() const {
    return map;
}

const glm::ivec2 TileMapLoader::getMapResolution() const {
    return mapResolution;
}