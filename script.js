var app = new Vue({
    el: '#app',
    data: {
      message: 'Hello Vue.js!',
	  bingResults1: [],
	  bingResults2: [],
	  bingResults3: [],
	  bingResultsAll: [],
      bingSearchTerm: "",
	  image2: ""
    },
    methods: {
        startWorker: function() {
            var c = document.getElementById("simple_sketch");
            var ctx = c.getContext("2d");

            if (typeof(w) == "undefined") {
                w = new Worker("worker.js");
            }
//            w.postMessage(ctx.getImageData(0,0,800,300));
			//console.log(makeQuery("cats", 10, 0));
			//w.postMessage(makeQuery("cats", 10, 0));
            w.onmessage = function(event){
                var c2 = document.getElementById("test");
                var ctx2 = c2.getContext("2d");
                var array = event.data.data;
                var imageData = new ImageData(array, 800,300);
                ctx2.putImageData(imageData, 0, 0);
            };
        },
		bingIt: function() {
			makeQuery(this.bingSearchTerm, 9, 0, () => {
				this.bingResults1 = [imageUrls[0], imageUrls[1], imageUrls[2]];
				this.bingResults2 = [imageUrls[3], imageUrls[4], imageUrls[5]];
				this.bingResults3 = [imageUrls[6], imageUrls[7], imageUrls[8]];
				this.bingResultsAll = [this.bingResults1, this.bingResults2, this.bingResults3];
			});
		},
		selectImage: function(img){
            this.image2 = img.srcElement.src;
			alert(this.image2);
		}
    }
});
