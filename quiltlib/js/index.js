const image_utils = require("./image_utils");
const q = require("q");

const texture = {
    "_malloc": Module._malloc,
    "_doSynth": Module._doSynth,
    "_doTransfer": Module._doTransfer,
    "HEAPU8": Module.HEAPU8
};

console.log(Object.keys(texture));


function transfer(textureURL, imgURL, alpha, tileSize, overlap) {
    return q.all([image_utils.getImage(textureURL),
                  image_utils.getImage(imgURL)])
        .then((items) => {
            const d = items[0];
            const corro = items[1];

            const width = corro.width;
            const height = corro.height;
            
            const dSize = width * height * 4;
            const dPtr = texture._malloc(dSize);
            const dView = new Uint8ClampedArray(texture.HEAPU8.buffer, dPtr, dSize);
            
            const sSize = d.width * d.height * 4;
            const sPtr = texture._malloc(sSize);
            const sView = new Uint8Array(texture.HEAPU8.buffer, sPtr, sSize);

            const cSize = width * height * 4;
            const cPtr = texture._malloc(cSize);
            const cView = new Uint8ClampedArray(texture.HEAPU8.buffer, cPtr, cSize);
            
            // copy data from d.data to oView
            sView.set(d.data);

            // copy data from corro.data to cView
            cView.set(corro.data);

            texture._doTransfer(d.width, d.height, width, height, sPtr, dPtr,
                                cPtr, alpha, tileSize, overlap);

            return new ImageData(dView, width, height);
        });
}

function synth() {
    const width = 256;
    const height = 256;

    const dSize = width * height * 4;
    const dPtr = texture._malloc(dSize);
    const dView = new Uint8ClampedArray(texture.HEAPU8.buffer, dPtr, dSize);

    return image_utils.getImage("/images/Food.0010.png")
        .then((d) => {
            const sSize = d.width * d.height * 4;
            const sPtr = texture._malloc(sSize);
            const sView = new Uint8Array(texture.HEAPU8.buffer, sPtr, sSize);

            // copy data from d.data to oView
            sView.set(d.data);

            texture._doSynth(d.width, d.height, width, height, sPtr, dPtr);

            return new ImageData(dView, width, height);
        });
}


window.transfer = transfer;


