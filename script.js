var app = new Vue({
    el: '#app',
    data: {
        message: 'Hello Vue.js!',
	bingResults1: [],
	bingResults2: [],
	bingResults3: [],
	bingResultsAll: [],
        bingSearchTerm: "",
        waiting: false
    },
    methods: {
        startWorker: function() {
            this.waiting = true;
            console.log("set waiting to true!");
            setTimeout(() => { this.doTransfer(); }, 1000);
        },
        
	bingIt: function() {
	    makeQuery(this.bingSearchTerm, 9, 0, () => {
		this.bingResults1 = [imageUrls[0], imageUrls[1], imageUrls[2]];
		this.bingResults2 = [imageUrls[3], imageUrls[4], imageUrls[5]];
		this.bingResults3 = [imageUrls[6], imageUrls[7], imageUrls[8]];
		this.bingResultsAll = [this.bingResults1, this.bingResults2, this.bingResults3];
	    });
	},

        doTransfer: function() {
            console.log("Doing transfer!");
            let ctx = document.getElementById("test").getContext("2d");
            transferFromURL("/quiltlib/images/Fabric.0001.png",
                            "/quiltlib/images/soapy.png",
                            0.5, 16, 4)
                .then((e) => {
                    ctx.putImageData(e, 0, 0);
                    this.waiting = false;
                }).catch(console.log);


            
        }
    }
})
