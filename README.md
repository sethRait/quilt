## Inspiration
We can take a texture like this:

![Lemon texture](http://i.imgur.com/4LhKEek.png)

... and a potato like this:

![Potato](http://i.imgur.com/5szgi80.png)

... and we can create a lemon potato:

![Lemon potato](http://i.imgur.com/0Nyl7ub.png)

## What it does

It uses a machine learning / energy minimization algorithm (specifically, loopy belief propagation over a finite Markov field with a few heuristics) in order to a model a transposition of a texture onto another natural image.

Our web demo lets users take pictures from their webcams and retexture them using a number of default options.

![Web interface](http://i.imgur.com/Q4ULgdw.png)

tl;dr it copies the texture from one picture onto another

## How we built it

We wrote the entire algorithm from the **ground-up in C++**, including neighbor search data structures, energy minimization routines, and marshaling code. We compiled the C++ code using **emscripten** so that the entire algorithm can be ran **in native `asm.js` JavaScript on the client side.**

Based on the initial ideas presented by Efros and Freeman.

## Challenges we ran into

Debugging memory faults in `emscripten` compiled JavaScript is really hard! We had to write a light-weight version of Valgrind (and `libVEX`) to track down a few nasty bugs. The algorithm itself was also pretty tricky, requiring us to gain a little bit more mathematical knowledge than we had before.

## Accomplishments that we're proud of

It works! And we have some neat images that really show how everything works.

![Compare](http://i.imgur.com/e2MF2Wy.jpg)

Team member Sofiya pictured, comparing actual synthesis to an overlay / soft light blending. 

## What we learned

So, so much about low-level optimizations and image processing!

## What's next for Quilty

Expanding it beyond just the client, doing video texture transfer.
