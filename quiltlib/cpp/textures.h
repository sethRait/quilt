#include <stdlib.h>
#include <cstdint>
#include <vector>
#define IMG_INDEX(x, y, channel, w) (((x)*4) + ((y)*(w)*4) + (channel))


class Texture {
    unsigned int width, height;
    unsigned int destWidth, destHeight;
    uint8_t* data;

    uint8_t* corro;
    double alpha;
    
    unsigned int tileSize = 12;
    unsigned int overlap = 4;

public:
    Texture(unsigned int width, unsigned int height, uint8_t* data);
    void setDestDimensions(unsigned int destW, unsigned int destH);
    void synthesize(uint8_t* dest);

    void setTileSize(unsigned int newTileSize);
    void setOverlapAmount(unsigned int newOverlapAmount);
    void setCorroImg(uint8_t* corro);
    void setAlpha(double alpha);

private:
    uint8_t getPixel(unsigned int x,
                     unsigned int y,
                     uint8_t channel);

    uint8_t getPixelLum(uint8_t* firstChannel);

    void copyTile(unsigned int srcX, unsigned int srcY,
                  unsigned int destX, unsigned int destY,
                  uint8_t* dest,
                  unsigned int xMargin = 0, unsigned int yMargin = 0);

    void doRandomTiling(uint8_t* dest);
    void replaceWithBestOverlap(uint8_t* dest, bool doCut);
    double measureErrorForTile(uint8_t* dest, unsigned int destX, unsigned int destY,
                               unsigned int replacementX, unsigned int replacementY);
    
    void getIndexOfBestReplacementTile(uint8_t* dest,
                                       unsigned int destX, unsigned int destY,
                                       unsigned int* outX, unsigned int* outY);

    void doLowestCostCut(uint8_t* dest, unsigned int srcX, unsigned int srcY,
                         unsigned int destX, unsigned int destY);

    void addNeighbors(std::vector<unsigned int>* neighbors,
                      unsigned int x,
                      unsigned int y);
    void tileIndexToCoords(unsigned int idx, unsigned int* x,
                           unsigned int* y);
    unsigned int coordsToTileIndex(unsigned int x, unsigned int y);
    bool coordInOverlap(unsigned int x, unsigned int y);
};
