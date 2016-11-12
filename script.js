var app = new Vue({
    el: '#app',
    data: {
      message: 'Hello Vue.js!'
    },
    methods: {
        startWorker: function() {
            var c = document.getElementById("simple_sketch");
            var ctx = c.getContext("2d");

            if (typeof(w) == "undefined") {
                w = new Worker("worker.js");
            }
            w.postMessage(ctx.getImageData(0,0,800,300));
            w.onmessage = function(event){
                var c2 = document.getElementById("test");
                var ctx2 = c2.getContext("2d");
                var array = event.data.data;
                var idata = new ImageData(array, 800,300);
                ctx2.putImageData(idata, 0, 0);
            };
        }
    }
})
