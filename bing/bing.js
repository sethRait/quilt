var xhr = new XMLHttpRequest();
var imageUrls = [];
var endpoint = "https://api.cognitive.microsoft.com/bing/v5.0/images/search";
var apiKey = "446fb5ff1b48443b80734e86da4d564c";

// Entrypoint.  Returns array of image urls.
function makeQuery(searchTerm, numResults, offset) {
	var query = "?q=" + searchTerm + "&count=" + numResults + "&offset=" + offset;
	xhr.open('GET', endpoint+query, true);
	xhr.setRequestHeader("Ocp-Apim-Subscription-key", apiKey);
	xhr.send();
	xhr.addEventListener("readystatechange", getJSON, false);
	xhr.onreadystatechange = getJSON;
	return imageUrls;
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
	}
}
