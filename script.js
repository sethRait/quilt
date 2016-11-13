var app = new Vue({
    el: '#app',
    data: {
        message: 'Hello Vue.js!',
	bingResults1: [],
	bingResults2: [],
	bingResults3: [],
	bingResultsAll: [],
        bingSearchTerm: "",
	image2: "",
        waiting: false,
        textureURL: "/quiltlib/images/Fabric.0005.png"
    },
    methods: {
        startWorker: function(opts) {
            this.waiting = true;
            console.log("set waiting to true!");
            setTimeout(() => { this.doTransfer(opts); }, 1000);
        },
        
	bingIt: function() {
	    makeQuery(this.bingSearchTerm, 9, 0, () => {
		this.bingResults1 = [imageUrls[0], imageUrls[1], imageUrls[2]];
		this.bingResults2 = [imageUrls[3], imageUrls[4], imageUrls[5]];
		this.bingResults3 = [imageUrls[6], imageUrls[7], imageUrls[8]];
		this.bingResultsAll = [this.bingResults1, this.bingResults2, this.bingResults3];
	    });
	},

        dismiss: function () {
            this.waiting = false;
        },
        
        doTransfer: function(opts) {
            console.log("Doing transfer!");
            let ctx = document.getElementById("test").getContext("2d");

            if (opts.type == "canvas") {
                transferFromCanvas(this.textureURL,
                                   opts.imgData, 0.6, 12, 4)
                    .then((e) => {
                        ctx.putImageData(e, 0, 0);
                    }).catch(console.log);
                
            } else if (opts.type == "urls") {
                transferFromURL(this.textureURL,
                                opts.corroURL,
                                0.5, 16, 4)
                    .then((e) => {
                        ctx.putImageData(e, 0, 0);
                    }).catch(console.log);
            }
            
        },
        
	selectImage: function(id){
            var img = document.getElementById(id);
            console.log("/bing/?url=" + encodeURIComponent(img.src));

            this.startWorker({"type": "urls",
                              "corroURL": "/bing/?url=" + encodeURIComponent(img.src)});
	},

        takePhoto: function() {
            
            var video = document.getElementById('video');
            var canvas = document.getElementById('photo');
            var context = canvas.getContext('2d');
            context.drawImage(video, 0, 0, 320, 240);
            var corro = context.getImageData(0, 0,
                                             canvas.width, canvas.height);
            
            this.startWorker({"type": "canvas",
                              "imgData": corro});
        }

    },

    mounted: function () {
        // Grab elements, create settings, etc.
        var video = document.getElementById('video');
        
        // Get access to the camera!
        if(navigator.mediaDevices && navigator.mediaDevices.getUserMedia) {
            navigator.mediaDevices.getUserMedia({ video: true })
                .then(function(stream) {
                    console.log("attaching video!");
                    video.src = window.URL.createObjectURL(stream);
                    video.play();
                    console.log("attached!");
                }).catch(function (e) { console.log(e); });
        }

    }


});
