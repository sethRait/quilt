const q = require("q");

module.exports.getImage = getImage;
function getImage(url) {
    const img = new Image();
    const toR = q.defer();
    
    img.addEventListener("load", function() {
        const canvas = document.createElement("canvas");
        canvas.width = this.width;
        canvas.height = this.height;

        const ctx = canvas.getContext("2d");
        ctx.drawImage(this, 0, 0);

        const imgData = ctx.getImageData(0, 0, this.width, this.height);
        toR.resolve(imgData);
    });

    img.addEventListener("error", function(e) {
        toR.reject(e);
    });

    // image will automatically load after we set the src property
    // Wacky JS side effects from assignment!! OooooooOooo spooky!
    img.src = url;

    return toR.promise;
}

