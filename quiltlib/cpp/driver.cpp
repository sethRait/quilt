#include "textures.h"
#include "lodepng.h"
#include <iostream>
#include <stdlib.h>

int main(int argc, char** argv) {
    std::vector<unsigned char> image; //the raw pixels
    unsigned width, height;
    
    //decode
    unsigned error = lodepng::decode(image, width, height, "/home/ryan/projects/textures/images/Food.0010.png");
    
    //if there's an error, display it
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    std::vector<unsigned char> image2;
    unsigned w2, h2;

    error = lodepng::decode(image2, w2, h2, "/home/ryan/projects/textures/images/soapy.png");

    //if there's an error, display it
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    

    uint8_t* src = (uint8_t*) malloc(width * height * 4);
    uint8_t* dest = (uint8_t*) malloc(256 * 256 * 4);
    uint8_t* corro = (uint8_t*) malloc(256 * 256 * 4);
    for (unsigned int i = 0; i < width * height * 4; i++) {
        src[i] = image[i];
    }

    for (unsigned int i = 0; i < 256 * 256; i++) {
        dest[i] = 0;
        corro[i] = image2[i];
    }
    

    Texture t(width, height, src);
    t.setTileSize(20);
    t.setOverlapAmount(4);
    t.setDestDimensions(256, 256);
    t.synthesize(dest);
        
    

    
}
