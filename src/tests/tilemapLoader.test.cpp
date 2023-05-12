#include <iostream>
#include "./tilemapLoader.test.h"


void testTileMapLoader() {
    TileMapLoader tilemapLoader{"./assets/tilemaps/jungle.map", "./assets/tilemaps/jungle.png"};
    auto map = tilemapLoader.getMap();
    auto res = tilemapLoader.getMapResolution();
    std::cout << "Resolution of image is " << res.x << " pixels width"
             << " and " << res.y << " pixels height" << std::endl;
    printMap(map);
}

void printMap(const Map& map) {
    std::cout << " Printing generated map ... " << std::endl;
    for (const auto &tile : map) {
        std::cout << tile.relativePosition.x << ", " << tile.relativePosition.y << " --> " << 
            tile.pixelSrcPosition.x << ", " << tile.pixelSrcPosition.y << std::endl;
    }

}