extern crate docopt;
extern crate image;
extern crate distance_field;

use std::fs::File;

use docopt::Docopt;
use image::GenericImage;
use distance_field::DistanceFieldExt;

const USAGE: &'static str = "
Usage:
    generator <source> <dest> <width> <height>
    generator (-h | --help)

Options:
    -h --help      Show this screen.
";

fn main() {
    let args = Docopt::new(USAGE)
        .and_then(|d| d.parse())
        .unwrap_or_else(|e| e.exit());

    let img = image::open(args.get_str("<source>")).unwrap();

    println!("Loaded image with size {:?}", img.dimensions());

    println!("Converting image to distance field image...");
    let (width, height) = (args.get_count("<width>"), args.get_count("<height>"));
    let outbuf = img.grayscale().distance_field(width as u32, height as u32);

    println!("Saving distance field image to {:?}", args.get_str("<dest>"));
    let ref mut fout = File::create(args.get_str("<dest>")).unwrap();

    image::ImageLuma8(outbuf).save(fout, image::PNG).unwrap();
}
