 #![crate_name = "distance_field"]

extern crate image;

use image::*;

pub trait DistanceFieldExt {
    fn distance_field(&self, width: u32, height: u32) -> ImageBuffer<Luma<u8>, Vec<u8>>;
}

impl DistanceFieldExt for DynamicImage {
    fn distance_field(&self, width: u32, height: u32)
        -> ImageBuffer<Luma<u8>, Vec<u8>> {
        let mut out: ImageBuffer<Luma<u8>, Vec<u8>> = ImageBuffer::new(width, height);

        let (orig_width, orig_height) = self.dimensions();

        for (x, y, pixel) in out.enumerate_pixels_mut() {
            let p = self.get_pixel(x, y);

            let i = p.data[0];
            *pixel = Luma([i as u8]);
        }

        out
    }
}
