// talie_textures/Lemon2.png, 0.5 14 4
// talie_textures/Chicken_Nugget2.png 0.6 10 4
/*document.addEventListener("DOMContentLoaded", function() {
 transfer("/talie_textures/Chicken_Nugget2.png", "/images/potato.png", 0.6, 10, 4).then(function(d) {
 console.log("done!");
 document.getElementById("dest").getContext("2d").putImageData(d, 0, 0);
 }).catch((e) => {
 console.error(e);
 });
 });*/

document.addEventListener("DOMContentLoaded", function () {
    // Grab elements, create settings, etc.
    var video = document.getElementById('video');

    // Get access to the camera!
    if(navigator.mediaDevices && navigator.mediaDevices.getUserMedia) {
        // Not adding `{ audio: true }` since we only want video now
        navigator.mediaDevices.getUserMedia({ video: true })
            .then(function(stream) {
                video.src = window.URL.createObjectURL(stream);
                video.play();
            }).catch(function (e) { console.log(e); });
    }


    var canvas = document.getElementById('canvas');
    var context = canvas.getContext('2d');
    
    // Trigger photo take
    document.getElementById("snap").addEventListener("click", function() {
        context.drawImage(video, 0, 0, 320, 240);
        var corro = context.getImageData(0, 0,
                                         canvas.width, canvas.height);
        /*transferFromCanvas("/images/Food.0010.png", corro, 0.5,
                           8, 2).then(function(imd) {
                               context.putImageData(imd, 0, 0);
                               console.log("drawn!");
                           }).catch(console.log);*/

    });

    
});

function doTransfer(url, alpha, tileSize, overlap) {
    var canvas = document.getElementById('canvas');
    var context = canvas.getContext('2d');

    var corro = context.getImageData(0, 0,
                                     canvas.width, canvas.height);

    document.getElementById("coverup").style.display = "block";
    setTimeout(function () {
        transferFromCanvas(url, corro, alpha, tileSize, overlap)
            .then(function(imd) {
                context.putImageData(imd, 0, 0);
                document.getElementById("coverup").style.display = "none";
            });
    }, 2000);
    
}

// talie_textures/Lemon2.png, 0.5 14 4
// talie_textures/Chicken_Nugget2.png 0.6 10 4
// images/Clouds.0000.png 0.9 10 6 SUPER SLOW
// images/Fabric.0005.png 0.6 12 4
// images/Metal.0005.png 0.5 12 4
// images/Fabric.0008.png 0.35 10 3
// images/Leaves.00013.png 0.85 6 2

function do1() { doTransfer("talie_textures/Lemon2.png", 0.50, 14, 3); }
function do2() { doTransfer("images/Food.0003.png", 0.60, 9, 3); }
function do3() { doTransfer("images/Clouds.0000.png", 0.90, 10, 6); }
function do4() { doTransfer("images/Fabric.0005.png", 0.60, 12, 3); }
function do5() { doTransfer("images/Metal.0005.png", 0.50, 12, 3); }
function do6() { doTransfer("images/Fabric.0008.png", 0.35, 10, 3); }
function do7() { doTransfer("images/Leaves.0013.png", 0.85, 6, 2); }
function do8() { doTransfer("images/Paintings.1.0000.png", 0.6, 10, 4); }
















/*
// talie_textures/Lemon2.png, 0.4 8 2
// talie_textures/Chicken_Nugget2.png 0.6 10 4
// images/Clouds.0000.png 0.9 10 6 SUPER SLOW
// images/Fabric.0005.png 0.6 12 4
// images/Metal.0005.png 0.5 12 4
// images/Fabric.0008.png 0.35 10 3
// images/Leaves.00013.png 0.85 6 2
*/
