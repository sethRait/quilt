all: bundle.js textures.js

local: cpp/*
	g++ -Wall -O3 -g cpp/lodepng.cpp cpp/textures.cpp cpp/driver.cpp -o local


bundle.js: node_modules js/*
	./node_modules/browserify/bin/cmd.js -d js/index.js -t babelify -t uglifyify -o bundle.js

textures.js: cpp/*
	emcc -s EXPORTED_FUNCTIONS="['_doSynth', '_doTransfer', '_malloc']" -O3 -Wall -std=c++11 cpp/textures.cpp -o textures.js

node_modules: package.json
	npm install

.phony: clean all

clean:
	rm -f bundle.js
	rm -f textures.js*
	rm -f local
