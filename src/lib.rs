 #![crate_name = "distance_field"]

extern crate image;
extern crate spiral;

use image::*;
use spiral::ChebyshevIterator;

/// The options passed as the argument to the `distance_field` method.
pub struct Options {
    /// The dimensions of the output image (width, height). The default value is: (64,64).
    pub size: (u32, u32),

    /// The maximum distance for the projected point of the output image on the input image to
    /// search for the nearest point. The defaul value is: 512.
    pub max_distance: u16,

    /// The image value at which to apply the treshold. In a black-white vector image 127 is
    /// probably the best value. The default value is: 127.
    pub image_treshold: u8,
}

/// Returns:
/// ```Options {
///     size: (64, 64),
///     max_distance: 512,
///     image_treshold: 127
/// }```
impl Default for Options {
    fn default() -> Self {
        Options {
            size: (64, 64),
            max_distance: 512,
            image_treshold: 127
        }
    }
}

/// A trait adding the `distance_field` function to image types.
pub trait DistanceFieldExt {
    /// Generates a grayscale output image with the dimensions as specified in the `Options`
    /// struct.
    fn distance_field(&self, options: Options) -> ImageBuffer<Luma<u8>, Vec<u8>>;
}

/// A implementation of the `distance_field` function for the `DynamicImage` type. To call this
/// from a normal RGB image use `image.grayscale().distance_field(options)`.
impl DistanceFieldExt for DynamicImage {
    fn distance_field(&self, options: Options) -> ImageBuffer<Luma<u8>, Vec<u8>> {
        ImageBuffer::from_fn(options.size.0, options.size.1, |x, y| {
            Luma([get_nearest_pixel_distance(self, x, y, &options)])
        })
    }
}

fn get_nearest_pixel_distance(input: &DynamicImage, out_x: u32, out_y: u32, options: &Options) -> u8 {
    let orig_size = input.dimensions();

    // Calcule the projected center of the output pixel on the source image
    let center = ((out_x * orig_size.0) / options.size.0, (out_y * orig_size.1) / options.size.1);

    // Check if we are inside a filled area so we can get the 127-255 range
    let is_inside = input.get_pixel(center.0, center.1).to_luma().data[0] > options.image_treshold;

    let mut closest_distance = options.max_distance as f32;
    for (x, y) in ChebyshevIterator::new(center.0 as i32, center.1 as i32, options.max_distance as u16) {
        if x < 0 || y < 0 || x >= orig_size.0 as i32 || y >= orig_size.1 as i32 {
            continue;
        }

        let p = input.get_pixel(x as u32, y as u32).to_luma().data[0];
        // Continue if the center and this pixel are inside the filled area or
        // the pixels are both outside the filled area
        if (p >= options.image_treshold) == is_inside {
            continue;
        }

        // We found the nearest pixel, calculate the distance
        let dx = (center.0 as i32 - x).abs();
        let dy = (center.1 as i32 - y).abs();
        closest_distance = ((dx * dx + dy * dy) as f32).sqrt();

        break;
    }

    // Convert the outside to a 0.0-0.5 and the inside to a 0.5-1.0 range
    let distance_fraction = if is_inside {
        0.5 + (closest_distance / 2.0) / options.max_distance as f32
    }else{
        0.5 - (closest_distance / 2.0) / options.max_distance as f32
    };

    // Convert the 0.0-1.0 range to a u8
    (distance_fraction * u8::max_value() as f32) as u8
}
