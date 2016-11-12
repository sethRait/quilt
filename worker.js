self.addEventListener('message', function(e) {
    var imageData = e;
    var data = imageData.data.data;
    for (var i = 0; i < data.length; i++) {
        data[i] = 100;
    }
    self.postMessage({data: data});

}, false);
