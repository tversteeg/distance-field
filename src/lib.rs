 #![crate_name = "distance_field"]

extern crate image;

use image::*;

pub trait DistanceFieldExt {
    fn distance_field(&self, width: u32, height: u32) -> ImageBuffer<Luma<u8>, Vec<u8>>;
}

impl DistanceFieldExt for DynamicImage {
    fn distance_field(&self, width: u32, height: u32) -> ImageBuffer<Luma<u8>, Vec<u8>> {
        ImageBuffer::from_fn(width, height, |x, y| {
            Luma([get_nearest_pixel_distance(self, x, y, width, height)])
        })
    }
}

fn get_nearest_pixel_distance(input: &DynamicImage, x: u32, y: u32, width: u32, height: u32) -> u8 {
    let (orig_width, orig_height) = input.dimensions();

    let p = input.get_pixel(x, y).to_luma();

    return p.data[0];
}
