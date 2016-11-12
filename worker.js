importScripts('textures.js', 'bundle.js');

self.addEventListener('message', function(e) {
    var p;
    if (e.type == "url") {
        p = textureFromURL(e.textureURL, e.imageURL, e.alpha,
                           e.tileSize, e.overlap);
    } else if (e.type == "canvas") {
        p = textureFromCanvas(e.textureURL, e.imgData, e.alpha,
                              e.tileSize, e.overlap);
    }

    p.then(function (d) {
        self.postMessage({data: d});        
    });

});
