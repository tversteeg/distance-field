 #![crate_name = "distance_field"]

extern crate image;
extern crate spiral;

use image::*;
use spiral::ManhattanIterator;

pub struct Options {
    pub size: (u32, u32),
    pub max_distance: u16,
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

fn get_nearest_pixel_distance(input: &DynamicImage, out_x: u32, out_y: u32, options: &Options) -> u8 {
    let orig_size = input.dimensions();

    let center = ((out_x * orig_size.0) / options.size.0, (out_y * orig_size.1) / options.size.1);

    let mut closest_distance = options.max_distance as f32;
    for (x, y) in ManhattanIterator::new(center.0 as i32, center.1 as i32, options.max_distance as u16) {
        if x < 0 || y < 0 || x >= orig_size.0 as i32 || y >= orig_size.1 as i32 {
            continue;
        }

        let p = input.get_pixel(x as u32, y as u32).to_luma().data[0];
        if p < options.image_treshold {
            continue;
        }

        let dx = (center.0 as i32 - x).abs();
        let dy = (center.1 as i32 - y).abs();
        closest_distance = ((dx * dx + dy * dy) as f32).sqrt();

        break;
    }

    let distance_fraction = 1.0 - closest_distance / options.max_distance as f32;

    (distance_fraction * u8::max_value() as f32) as u8
}
