#include "textures.h"
#include "nanoflann.h"
#include <cstring>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <functional>
#include <cassert>

Texture::Texture(unsigned int width, unsigned int height,
                 uint8_t* data)
    : width(width), height(height), data(data) {

    corro = NULL;
    alpha = 0.5;

    minCorro = 0;
    maxCorro = 255;
    minText = 0;
    maxText = 255;

    // set the alpha channel of the image to 255
    for (unsigned int i = 3; i < width * height * 4; i += 4)
        data[i] = 255;
    
}

void Texture::setDestDimensions(unsigned int dW, unsigned int dH) {
    destWidth = dW;
    destHeight = dH;
}

void Texture::synthesize(uint8_t* dest) {
   // set the alpha channel of the image to 255
    for (unsigned int i = 3; i < destWidth * destHeight * 4; i += 4)
        dest[i] = 255;
    
    doRandomTiling(dest);
    
    if (corro != NULL)
      computeMinMaxLum();
    
    replaceWithBestOverlap(dest, true);
}

void Texture::computeMinMaxLum() {
    minCorro = std::numeric_limits<double>::infinity();
    maxCorro = std::numeric_limits<double>::min();
    minText = std::numeric_limits<double>::infinity();
    maxText = std::numeric_limits<double>::min();

    for (unsigned int y = 0; y < destHeight; y++) {
        for (unsigned int x = 0; x < destWidth; x++) {
            uint8_t lum = getPixelLum(corro + IMG_INDEX(x, y, 0, destWidth));
            minCorro = std::min((double)lum, minCorro);
            maxCorro = std::max((double)lum, maxCorro);
        }
    }

    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            uint8_t lum = getPixelLum(data + IMG_INDEX(x, y, 0, width));
            minText = std::min((double)lum, minText);
            maxText = std::max((double)lum, maxText);
        }
    }

    printf("%f %f %f %f\n", minCorro, maxCorro, minText, maxText);
}


void Texture::doRandomTiling(uint8_t* dest) {
    unsigned int increm = tileSize - overlap;

    std::default_random_engine generator;
    std::uniform_int_distribution<int> dist(0, std::max(width, height) - tileSize);
    auto r = std::bind(dist, generator);
    
    for (unsigned int y = 0; y < destHeight - tileSize; y += increm) {
        for (unsigned int x = 0; x < destWidth - tileSize; x += increm) {
            copyTile(r(), r(), x, y, dest);
        }
    }
}

void Texture::replaceWithBestOverlap(uint8_t* dest, bool doCut) {
    unsigned int increm = tileSize - overlap;

    // figure out where the last tile started.
    unsigned int startAt = (destHeight / increm) * increm;
    unsigned int startAtW = (destWidth / increm) * increm;
    
    for (int y = startAt; y >= 0; y -= increm) {
        for (int x = startAtW; x >= 0; x -= increm) {
            // for the tile starting at x,y, we need to find a new tile
            // to replace it with. this new tile should have the lowest possible
            // error with the overlapped region.

            
            unsigned int bx, by;
            getIndexOfBestReplacementTile(dest, x, y,
                                          &bx, &by);

            // now that we have the index, copy the tile, excluding the overlapped
            // region we just fit against
            copyTile(bx, by, x, y, dest,
                     overlap, overlap);

            if (doCut) {
                doLowestCostCut(dest, bx, by, x, y);
            }

            //printf("corrected tile row = %d, col = %d\n", y, x);
        }
    }

    // if the corrosp image is white (or transparent), make the dest image
    // the same.
    if (corro == NULL)
        return;
    
    for (unsigned int y = 0; y < destHeight; y++) {
        for (unsigned int x = 0; x < destWidth; x++) {
            if (corro[IMG_INDEX(x, y, 0, destWidth)] > 245 &&
                corro[IMG_INDEX(x, y, 1, destWidth)] > 245 &&
                corro[IMG_INDEX(x, y, 2, destWidth)] > 245) {

                dest[IMG_INDEX(x, y, 0, destWidth)] = 255;
                dest[IMG_INDEX(x, y, 1, destWidth)] = 255;
                dest[IMG_INDEX(x, y, 2, destWidth)] = 255;
            }
            dest[IMG_INDEX(x, y, 3, destWidth)] = corro[IMG_INDEX(x, y, 3, destWidth)];
        }
    }
}

void Texture::getIndexOfBestReplacementTile(uint8_t* dest,
                                            unsigned int destX,
                                            unsigned int destY,
                                            unsigned int* xOut,
                                            unsigned int* yOut) {
    // loop over every possible tile and track the best one
    double bestErrorSoFar = std::numeric_limits<double>::max();
    *xOut = 0;
    *yOut = 0;

    for (unsigned int y = 0; y < height - tileSize; y++) {
        for (unsigned int x = 0; x < width - tileSize; x++) {
            double errorForThisIdx = measureErrorForTile(dest, destX, destY, x, y);

            if (bestErrorSoFar > errorForThisIdx) {
                bestErrorSoFar = errorForThisIdx;
                *xOut = x;
                *yOut = y;
            }
        }
    }

}

double Texture::measureErrorForTile(uint8_t* dest,
                                    unsigned int destX, unsigned int destY,
                                    unsigned int replacementX,
                                    unsigned int replacementY) {
    // the left side of the right-overlapping strip starts at
    // x = destX + (tileSize - overlap)
    // and y = destY. The top side of the bottom-overlapping strip starts at x = destX
    // and y = destY + (tileSize - overlap)
    unsigned int increm = tileSize - overlap;
    double accum = 0.0; // the overlap tile error
    double corrospAccum = 0.0; // the corropsondence mapping error

    // first, compute the error for the entire right side
    // overlapping region
    for (unsigned int y = 0; y < tileSize; y++) {
        for (unsigned int x = 0; x < overlap; x++) {
            if (destX + increm + x >= destWidth
                || destY + y >= destHeight)
                continue;
                
            for (unsigned int c = 0; c < 3; c++) {
                double proposed = getPixel(replacementX + increm + x, replacementY + y, c);
                double actual = dest[IMG_INDEX(destX + increm + x,
                                               destY + y,
                                               c, destWidth)];

                accum += (proposed - actual) * (proposed - actual);
            }
        }
    }

    // second, compute the error for the bottom side
    // overlapping region, excluding the part we already got
    for (unsigned int y = 0; y < overlap; y++) {
        for (unsigned int x = 0; x < tileSize - overlap; x++) {
            if (destX + x >= destWidth
                || destY + increm + y >= destHeight)
                continue;
            
            for (unsigned int c = 0; c < 3; c++) {
                double proposed = getPixel(replacementX + x, replacementY + increm + y, c);
                double actual = dest[IMG_INDEX(destX + x,
                                               destY + increm + y,
                                               c, destWidth)];

                accum += (proposed - actual) * (proposed - actual);
            }
        }
    }

    if (corro != NULL) {
        // we need to add in the error from the corrospondence map
        // that's the difference in intensities over all the points

        for (unsigned int y = 0; y < tileSize - overlap; y++) {
            for (unsigned int x = 0; x < tileSize - overlap; x++) {
                if (destX + x >= destWidth
                    || destY + increm + y >= destHeight)
                    continue;
                
                double sourceLum = getPixelLum(data + IMG_INDEX(replacementX + x,
                                                                replacementY + y,
                                                                0, width));

                double mapLum = getPixelLum(corro + IMG_INDEX(destX + x, destY + y,
                                                             0, destWidth));

                // scale both based on the min and the max
                sourceLum = (sourceLum - minText) / (maxText - minText);
                mapLum = (mapLum - minCorro) / (maxCorro - minCorro);
                sourceLum *= 255.0;
                mapLum *= 255.0;

                
                corrospAccum += (sourceLum - mapLum) * (sourceLum - mapLum);
            }
        }

        return alpha * accum + (1.0-alpha)*corrospAccum;
    }
    
    // if no corrosp, don't bother with the weighting value
    return accum;
}

void Texture::tileIndexToCoords(unsigned int idx,
                                unsigned int* x,
                                unsigned int* y) {
    /*           |---------------|
                 |12  19  26  33 |
                 |13  20  27  34 |  
      |----------|14  21  28  35 |
      | 0  4  8   15  22  29  36 |
      | 1  5  9   16  23  30  37 |
      | 2  6  10  17  24  31  38 |  
      | 3  7  11  18  25  32  39 |  
      |--------------------------|
                 
    */
    // convert the index bestIdx into an x and y value.
    // first, figure out if our value is in the first segment (before it
    // gets taller). Then do the rest of the calculation.

    // if the idx is 22, and the tileSize is 7 and the overlap is 4,
    // the result should be 4,3. When idx is 6, the result should be
    // 1,5
    if (idx < (tileSize - overlap) * overlap) {
        // it is.
        *y = (idx % overlap) + (tileSize - overlap);
        *x = idx / overlap;
    } else {
        // it isn't.
        // make it so the first member of the tall column is numbered
        // zero.
        idx -= (tileSize - overlap) * overlap;
        *y = idx % tileSize;
        *x = idx / tileSize + (tileSize - overlap);
    } 
}

unsigned int Texture::coordsToTileIndex(unsigned int x, unsigned int y) {
    // check to see if it is in the first segement before it
    // gets taller. 5,3 should be 29 when tileSize is 7 and overlap is 4.
    // 1,4 should be 5 when tileSize is 7 and overlap is 4
    if (x < (tileSize - overlap)) {
        return x * overlap + (y - (tileSize - overlap));
    } else {
        return ((x - (tileSize - overlap)) * tileSize) + y +
            ((tileSize - overlap) * overlap);
    }
}

bool Texture::coordInOverlap(unsigned int x, unsigned int y) {
    if (x >= tileSize || y >= tileSize)
        return false;
    
    if (x < tileSize - overlap && y < tileSize - overlap)
        return false;
    return true;

}

void Texture::addNeighbors(std::vector<unsigned int>* neighbors,
                           unsigned int x,
                           unsigned int y) {
    if (coordInOverlap(x+1, y))
        neighbors->push_back(coordsToTileIndex(x+1, y));
    
    // can only go left if we are in the tall chunk
    if (x > tileSize - overlap)
        if (coordInOverlap(x-1, y))
            neighbors->push_back(coordsToTileIndex(x-1, y));
    
    if (coordInOverlap(x, y-1))
        neighbors->push_back(coordsToTileIndex(x, y-1));
    
    // can only go down in the fat chunk
    if (x < tileSize - overlap)
        if (coordInOverlap(x, y+1))
            neighbors->push_back(coordsToTileIndex(x, y+1));
}

void Texture::doLowestCostCut(uint8_t* dest,
                     unsigned int srcX, unsigned int srcY,
                     unsigned int destX, unsigned int destY) {
    unsigned int overlapSize =
        tileSize * overlap +
        overlap * (tileSize - overlap);
    
    double dist[overlapSize];
    int prev[overlapSize];
    std::vector<bool> Q;

    for (unsigned int i = 0; i < overlapSize; i++) {
        dist[i] = std::numeric_limits<double>::infinity();
        prev[i] = -1;
        Q.push_back(true);
    }

    // set the first column to zero (we can reach it with distance
    // zero).
    for (unsigned int i = 0; i < overlap; i++) {
        dist[coordsToTileIndex(0, i + (tileSize - overlap))] = 0;
    }

    
    while (std::any_of(Q.begin(), Q.end(), [](bool b){return b;})) {
        // heaps in C++ are hard! (there's PQ, but we can't change the weights
        // without scanning the whole thing.) So we'll just do O(n)...

        // find the index that is in Q and has the lowest dist
        int bestIdx = -1;
        for (unsigned int i = 0; i < overlapSize; i++) {
            if (Q[i] == false)
                continue;
            
            if (bestIdx == -1 || dist[bestIdx] > dist[i])
                bestIdx = i;
        }

        // bestIdx is now the lowest element in dist that is still
        // in Q. We need to remove it from Q.
        Q[bestIdx] = false;

        // we will number them like this (i.e. down the columns)
        
        /*           |---------------|
                     |12  19  26  33 |
                     |13  20  27  34 |  
          |----------|14  21  28  35 |
          | 0  4  8   15  22  29  36 |
          | 1  5  9   16  23  30  37 |
          | 2  6  10  17  24  31  38 |  
          | 3  7  11  18  25  32  39 |  
          |--------------------------|

         */

        unsigned int x, y;
        tileIndexToCoords(bestIdx, &x, &y);
        
        // now that we have our x and y values relative to our tile, we need to get
        // their neighbors that are still in Q
        std::vector<unsigned int> neighbors;
        addNeighbors(&neighbors, x, y);

        for (unsigned int neighbor : neighbors) {
            if (!Q[neighbor])
                continue;
            
            unsigned int nx, ny;
            tileIndexToCoords(neighbor, &nx, &ny);

            //assert(neighbor < overlapSize);

            double err = 0.0;
            for (int c = 0; c < 3; c++) {
                if (destX + nx >= destWidth || destY + ny >= destHeight)
                    continue;
                
                int cd = dest[IMG_INDEX(destX + nx, destY + ny, c, destWidth)];
                int cs = data[IMG_INDEX(srcX + nx, srcY + ny, c, width)];
                err += (double)(cd - cs) * (double)(cd - cs);
            }


            
            double alt = dist[bestIdx] + err;
            if (alt < dist[neighbor]) {
                //printf("%d, %d -> %d, %d = %f\n", x, y, nx, ny, alt);
                dist[neighbor] = alt;
                prev[neighbor] = bestIdx;
            }
        }

    }

    // now we've gone over every node! we have our dist and prev
    // arrays. find the best ending point
    int lowestEndPointIdx = -1;
    for (unsigned int i = 0; i < overlap; i++) {
        int idx = coordsToTileIndex(i + (tileSize - overlap), 0);
        if (lowestEndPointIdx == -1 || dist[lowestEndPointIdx] > dist[idx])
            lowestEndPointIdx = idx;
    }

    // reconstruct the path from this ending point
    std::unordered_set<int> path;
    for (int i = lowestEndPointIdx; i != -1; i = prev[i]) {
        path.insert(i);
        unsigned int x, y;
        tileIndexToCoords(i, &x, &y);
        //printf("(%d, %d) ", x, y);
    }
    //printf("\n-------\n");

    std::unordered_set<int> visited;
    std::vector<int> queue;

    // explicitly add every item along the inner border
    for (unsigned int i = 0; i < tileSize - overlap; i++) {
        queue.push_back(coordsToTileIndex(tileSize - overlap, i));
        queue.push_back(coordsToTileIndex(i, tileSize - overlap));
    }

    int counter = 0;
    while (!queue.empty()) {
        unsigned int x, y;
        unsigned int i = queue.back(); queue.pop_back();
        visited.insert(i);
        tileIndexToCoords(i, &x, &y);

        // replace the overlapped image here with my image.
        if (x + destX < destWidth && y + destY < destHeight) {
            counter++;
            for (int c = 0; c < 3; c++) {
                uint8_t p = data[IMG_INDEX(x + srcX, y + srcY, c, width)];
                dest[IMG_INDEX(x + destX, y + destY, c, destWidth)] = p;
                
            }
        }
       
                      
        
        // if it isn't on the path, add it's neighbors.
        if (path.find(i) != path.end()) {
            continue;
        }
        
        std::vector<unsigned int> neighbors;
        addNeighbors(&neighbors, x, y);

        for (unsigned int n : neighbors) {
            if (visited.find(n) != visited.end())
                continue; // we've already seen it
            queue.push_back(n);
        }
        
    }
    //printf("%d / %d\n", counter, overlapSize);

}


void Texture::setTileSize(unsigned int newTileSize) {
    tileSize = newTileSize;
}

void Texture::setOverlapAmount(unsigned int newAmt) {
    overlap = newAmt;
}

void Texture::setCorroImg(uint8_t* c) {
    corro = c;
}

void Texture::setAlpha(double a) {
    alpha = a;
}


uint8_t Texture::getPixel(unsigned int x, unsigned int y,
                          uint8_t channel) {

    unsigned int idx = IMG_INDEX(x, y, channel, width);
    return data[idx];
}

uint8_t Texture::getPixelLum(uint8_t* firstChannelLoc) {
    // lum = (0.2126*R + 0.7152*G + 0.0722*B)
    double val = 0.0;
    val += 0.2126 * (*firstChannelLoc);
    val += 0.7152 * (*(firstChannelLoc + 1));
    val += 0.0722 * (*(firstChannelLoc + 2));

    return (uint8_t) val;
}

void Texture::copyTile(unsigned int srcX, unsigned int srcY,
                       unsigned int destX, unsigned int destY,
                       uint8_t* dest,
                       unsigned int xMargin, unsigned int yMargin) {

    unsigned int copyWidth = std::min(tileSize - xMargin, width - srcX);
    copyWidth = std::min(copyWidth, destWidth - destX);
        
    for (unsigned int y = 0; y < tileSize - yMargin; y++) {
        if (srcY + y >= height) break; // don't write over the end!
        if (destY + y >= destHeight) break;
        
        // use memcpy to (quickly) do the row copy
        std::memcpy(dest + IMG_INDEX(destX, destY + y, 0, destWidth),
                    data + IMG_INDEX(srcX, srcY + y, 0, width),
                    4 * copyWidth);
    }
    


}



extern "C" {
    void doSynth(unsigned int srcWidth, unsigned int srcHeight,
                 unsigned int destWidth, unsigned int destHeight,
                 uint8_t* src, uint8_t* dest) {

        Texture t(srcWidth, srcHeight, src);
        t.setDestDimensions(destWidth, destHeight);
        t.setTileSize(20);
        t.setOverlapAmount(4);

        t.synthesize(dest);
    }

    void doTransfer(unsigned int srcWidth, unsigned int srcHeight,
                    unsigned int destWidth, unsigned int destHeight,
                    uint8_t* src, uint8_t* dest, uint8_t* corro,
                    double alpha, int tileSize, int overlap) {
        
        Texture t(srcWidth, srcHeight, src);
        t.setCorroImg(corro);
        t.setDestDimensions(destWidth, destHeight);
        t.setAlpha(alpha);
        t.setTileSize(tileSize);
        t.setOverlapAmount(overlap);
        
        t.synthesize(dest);
    }
}

