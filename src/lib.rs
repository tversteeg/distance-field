 #![crate_name = "distance_field"]

extern crate image;

use image::*;

pub struct Options {
    pub size: (u32, u32),
    pub max_distance: u32
}

impl Default for Options {
    fn default() -> Self {
        Options {
            size: (64, 64),
            max_distance: 512
        }
    }
}

pub trait DistanceFieldExt {
    fn distance_field(&self, options: Options) -> ImageBuffer<Luma<u8>, Vec<u8>>;
}

impl DistanceFieldExt for DynamicImage {
    fn distance_field(&self, options: Options) -> ImageBuffer<Luma<u8>, Vec<u8>> {
        ImageBuffer::from_fn(options.size.0, options.size.1, |x, y| {
            Luma([get_nearest_pixel_distance(self, x, y, &options)])
        })
    }
}

fn get_nearest_pixel_distance(input: &DynamicImage, x: u32, y: u32, options: &Options) -> u8 {
    let (orig_width, orig_height) = input.dimensions();

    let p = input.get_pixel(x, y).to_luma();

    return p.data[0];
}
