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
            w.postMessage(ctx.getImageData(10,10,50,50));
            console.log(ctx.getImageData(10,10,50,50));
            w.onmessage = function(event){
                document.getElementById("result").innerHTML = event.data;
            };
        }
    }
})
