//! Generate distance field bitmaps for rendering as pseudo-vector images in shaders.
//!
//! An example usecase for the library would be to automatically convert asset images. You can achieve this by having a `build.rs` similar to this:
//!
//! ```rust
//! use std::fs::File;
//! use distance_field::DistanceFieldExt;
//!
//! fn convert_image_to_dfield(input: &str, output: &str) {
//!     // Load the 'input' image
//!     let img = image::open(input).unwrap();
//!
//!     // Generate a distance field from the image
//!     let outbuf = img.grayscale().distance_field(distance_field::Options {
//!         size: (128, 128),
//!         max_distance: 256,
//!         ..Default::default()
//!     });
//!
//!     // Save it to 'output' as a PNG
//!     image::DynamicImage::ImageLuma8(outbuf).save(output).unwrap();
//! }
//!
//! fn main() {
//!     convert_image_to_dfield("img/input.png", "output.png");
//! }
//! ```

use bitvec::vec::BitVec;
use image::{DynamicImage, ImageBuffer, Luma};
use spiral::ManhattanIterator;

/// The options passed as the argument to the `distance_field` method.
pub struct Options {
    /// The dimensions of the output image (width, height). The default value is: (64,64).
    pub size: (usize, usize),

    /// The maximum distance for the projected point of the output image on the input image to
    /// search for the nearest point. The defaul value is: 512.
    pub max_distance: usize,

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
            image_treshold: 127,
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
        let treshold = TresholdImage::from_dynamic_image(self, options.image_treshold);

        ImageBuffer::from_fn(options.size.0 as u32, options.size.1 as u32, |x, y| {
            Luma([get_nearest_pixel_distance(
                &treshold, x as usize, y as usize, &options,
            )])
        })
    }
}

/// Treshold image as a boolean vector.
struct TresholdImage {
    bits: BitVec,
    width: usize,
    height: usize,
}

impl TresholdImage {
    /// Convert a dynamic image to this type of image by applying a treshold.
    pub fn from_dynamic_image(image: &DynamicImage, treshold: u8) -> Self {
        // Get the image as grayscale
        let luma_img = image.to_luma8();

        // Convert the image to a boolean buffer, applying a treshold
        let bits = luma_img
            .pixels()
            .map(|pixel| pixel.0[0] > treshold)
            .collect::<BitVec>();

        Self {
            bits,
            width: luma_img.width() as usize,
            height: luma_img.height() as usize,
        }
    }

    /// Get a pixel as a boolean.
    pub fn get_pixel(&self, x: usize, y: usize) -> bool {
        let index = x + y * self.width;

        self.bits[index]
    }
}

fn get_nearest_pixel_distance(
    input: &TresholdImage,
    out_x: usize,
    out_y: usize,
    options: &Options,
) -> u8 {
    // Calcule the projected center of the output pixel on the source image
    let center = (
        (out_x * input.width) / options.size.0,
        (out_y * input.height) / options.size.1,
    );

    // Check if we are inside a filled area so we can get the 127-255 range
    let is_inside = input.get_pixel(center.0, center.1);

    let closest_distance = ManhattanIterator::new(center.0, center.1, options.max_distance)
        // Ignore the boundary conditions
        .filter(|(x, y)| *x < input.width && *y < input.height)
        // Continue if the center and this pixel are inside the filled area or
        // the pixels are both outside the filled area
        .find(|(x, y)| input.get_pixel(*x, *y) != is_inside)
        // We found the nearest pixel, calculate the distance
        .map(|(x, y)| {
            let dx = center.0.abs_diff(x);
            let dy = center.1.abs_diff(y);

            ((dx * dx + dy * dy) as f32).sqrt()
        })
        .unwrap_or(options.max_distance as f32);

    // Convert the outside to a 0.0-0.5 and the inside to a 0.5-1.0 range
    let distance_fraction = if is_inside {
        0.5 + (closest_distance / 2.0) / options.max_distance as f32
    } else {
        0.5 - (closest_distance / 2.0) / options.max_distance as f32
    };

    // Convert the 0.0-1.0 range to a u8
    (distance_fraction * u8::max_value() as f32) as u8
}
