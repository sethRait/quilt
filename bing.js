var xhr = new XMLHttpRequest();
var imageUrls = [];
var endpoint = "https://api.cognitive.microsoft.com/bing/v5.0/images/search";
var apiKey = "446fb5ff1b48443b80734e86da4d564c";

//shrinkImages();

// Entrypoint.  Returns array of image urls.
function makeQuery(searchTerm, numResults, offset, callback) {
	var query = "?q=" + searchTerm + "&count=" + numResults + "&offset=" + offset;
	xhr.open('GET', endpoint+query, true);
	xhr.setRequestHeader("Ocp-Apim-Subscription-key", apiKey);
	xhr.send();
	xhr.addEventListener("readystatechange", function(e) {
		getJSON(e);
		callback();
	});
}

function getJSON(e) {
	// State 4 = DONE, status 200 = success
	if (xhr.readyState == 4 && xhr.status == 200) {
		var response = JSON.parse(xhr.responseText);
		// Prevent multiple DONE states from triggering.
		xhr.onreadystatechange = null;
		response.value.forEach(function(image) {
			imageUrls.push(image.contentUrl);
		});
	//	shrinkImages(response);
	}
}

function shrinkImages(response) {
	for (var i = 0; i < response.value.length; i++) {
		var img = document.createElement('img');
		img.src = imageUrls[i];
		var scale = findScale(response.value[i].width, response.value[i].height);
		img.width = scale[0];
		img.height = scale[1];
		document.body.appendChild(img);
	}
}

function findScale(width, height) {
	var x = Math.max(width, height);
	var y = Math.min(width, height);
	// flip x width and height if the width isn't the long side
	var flip = (width != x);
	y = (350 * y) / x;
	x = 350;
	if (!flip) {
		return [x, y];
	}
	else {
		return [y, x];
	}
}
