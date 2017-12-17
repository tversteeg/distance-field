 #![crate_name = "distance_field"]

extern crate image;

use image::*;
use std::cmp;

pub struct Options {
    pub size: (u32, u32),
    pub max_distance: u32,
    pub image_treshold: u8,
}

impl Default for Options {
    fn default() -> Self {
        Options {
            size: (64, 64),
            max_distance: 512,
            image_treshold: 128
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

fn get_image_window(pos: (u32, u32), max_distance: i32, size: (u32, u32)) -> (u32, u32, u32, u32) {
    let minx = cmp::max(pos.0 as i32 - max_distance, 0) as u32;
    let miny = cmp::max(pos.1 as i32 - max_distance, 0) as u32;
    let maxx = cmp::min(pos.0 as i32 + max_distance, size.0 as i32) as u32;
    let maxy = cmp::min(pos.1 as i32 + max_distance, size.1 as i32) as u32;

    (minx, miny, maxx, maxy)
}

fn get_nearest_pixel_distance(input: &DynamicImage, out_x: u32, out_y: u32, options: &Options) -> u8 {
    let orig_size = input.dimensions();

    let center = ((out_x * orig_size.0) / options.size.0, (out_y * orig_size.1) / options.size.1);
    let (i_x, i_y, i_width, i_height) = get_image_window(center, options.max_distance as i32, orig_size);

    let max_distance_squared = options.max_distance * options.max_distance;

    let mut closest_distance = max_distance_squared;
    for y in i_y .. i_height {
        let mut dist_y = center.1 - i_y;
        dist_y *= dist_y;

        for x in i_x .. i_width {
            let p = input.get_pixel(x, y).to_luma().data[0];
            if p < options.image_treshold {
                continue;
            }

            let mut dist_x = center.0 - i_x;
            dist_x *= dist_x;

            let distance = dist_x + dist_y;
            if distance < closest_distance {
                closest_distance = distance;
            }
        }
    }

    let distance_fraction = closest_distance as f32 / max_distance_squared as f32;

    (distance_fraction * u8::max_value() as f32) as u8
}
