extern crate image;

use image::{GenericImage, ImageBuffer, Pixel};

pub fn distance_field<I: GenericImage + 'static>(image: &mut I, width: u32, height: u32)
-> ImageBuffer<I::Pixel, Vec<<I::Pixel as Pixel>::Subpixel>> {
    let mut out = ImageBuffer::new(width, height);

    let (orig_width, orig_height) = image.dimensions();

    for y in 0 .. orig_height {
        for x in 0 .. orig_height {
            let p = image.get_pixel(x, y);

            out.put_pixel(x, y, p);
        }
    }

    out
}
