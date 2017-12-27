# distance-field [![Build Status](https://travis-ci.org/tversteeg/distance-field.svg?branch=master)](https://travis-ci.org/tversteeg/distance-field) [![Cargo](https://img.shields.io/crates/v/distance-field.svg)](https://crates.io/crates/distance-field) [![Documentation](https://docs.rs/distance-field/badge.svg)](https://docs.rs/distance-field/) [![License: GPL-3.0](https://img.shields.io/crates/l/distance-field.svg)](#license) [![Downloads](https://img.shields.io/crates/d/distance-field.svg)](#downloads)

Library for generating distance field bitmaps to render as pseudo-vector images in a shader

### [Documentation](https://docs.rs/distance-field/)

### How to run

Checkout the repository and `cd` into it; then run:

    cargo run --example generator -- img/input.png output.png 64 64

This will take the following image as input:

![Rust logo](img/input.png?raw=true)

And generate a 64x64 distance field:

![Distance field](img/output.png?raw=true)

Now we can use the generated distance field to create a vector image. First we need to scale it up with a linear interpolation:

![Upscaled linear](img/linear.png?raw=true)

Then we apply a treshold function:

![Treshold](img/treshold.png?raw=true)

You can see that we have something which looks very similar to the original input image and that just from a 64x64 image! But it's still very pixelated and doesn't look like a vector image. This can be fixed by not doing a hard treshold but allowing some shades of gray. In GIMP we can do this by adjusting the levels:

![Adjust levels](img/adjust-levels.png?raw=true)

This gives us the following result:

![Result](img/aa.png?raw=true)
