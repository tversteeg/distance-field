# distance-field

[![Build Status](https://github.com/tversteeg/distance-field/workflows/CI/badge.svg)](https://github.com/tversteeg/distance-field/actions?workflow=CI)
[![Crates.io](https://img.shields.io/crates/v/distance-field.svg)](https://crates.io/crates/distance-field)
[![Documentation](https://docs.rs/distance-field/badge.svg)](https://docs.rs/distance-field)
[![License: GPL-3.0](https://img.shields.io/crates/l/distance-field.svg)](#license)
[![Downloads](https://img.shields.io/crates/d/distance-field.svg)](#downloads)

### [Documentation](https://docs.rs/distance-field/)

<!-- cargo-rdme start -->

Generate distance field bitmaps for rendering as pseudo-vector images in shaders.

An example usecase for the library would be to automatically convert asset images. You can achieve this by having a `build.rs` similar to this:

```rust
use std::fs::File;
use distance_field::DistanceFieldExt;

fn convert_image_to_dfield(input: &str, output: &str) {
    // Load the 'input' image
    let img = image::open(input).unwrap();

    // Generate a distance field from the image
    let outbuf = img.grayscale().distance_field(distance_field::Options {
        size: (128, 128),
        max_distance: 256,
        ..Default::default()
    });

    // Save it to 'output' as a PNG
    image::DynamicImage::ImageLuma8(outbuf).save(output).unwrap();
}

fn main() {
    convert_image_to_dfield("img/input.png", "output.png");
}
```

<!-- cargo-rdme end -->

# Generator binary

Checkout the repository and `cd` into it; then run:

    cargo run --example generator -- img/input.png output.png 64 64

This will take the following image as input:

![Rust logo](img/input.png?raw=true)

And generate a 64x64 distance field:

![Distance field](img/output.png?raw=true)

Now we can use the generated distance field to create a vector image.

### Using the distance field

First we need to scale it up with a linear interpolation:

![Upscaled linear](img/linear.png?raw=true)

Then we apply a treshold function:

![Treshold](img/treshold.png?raw=true)

You can see that we have something which looks very similar to the original input image and that just from a 64x64 image! But it's still very pixelated and doesn't look like a vector image. This can be fixed by not doing a hard treshold but allowing some shades of gray.

