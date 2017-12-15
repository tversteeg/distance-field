 #![crate_name = "distance_field"]

extern crate image;

use image::*;

pub fn distance_field<I: GenericImage + 'static>(image: &I, width: u32, height: u32)
-> ImageBuffer<Luma<u8>, Vec<u8>> {
    let mut out: ImageBuffer<Luma<u8>, Vec<u8>> = ImageBuffer::new(width, height);

    let (orig_width, orig_height) = image.dimensions();

    for (x, y, pixel) in out.enumerate_pixels_mut() {
        let p: Luma<u8> = image.get_pixel(x, y).to_luma();

        let i = p.data[0];
        *pixel = Luma([i as u8]);
    }

    out
}
