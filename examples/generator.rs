extern crate docopt;
extern crate image;
extern crate distance_field;

use docopt::Docopt;
use distance_field::DistanceFieldExt;
use image::GenericImageView;

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

    let width = args.get_str("<width>").parse::<u32>().unwrap();
    let height = args.get_str("<height>").parse::<u32>().unwrap();
    println!("Converting image to distance field image with size ({:?}, {:?})...", width, height);

    let outbuf = img.grayscale().distance_field(distance_field::Options {
        max_distance: 256,
        ..Default::default()
    });

    println!("Saving distance field image to {:?}", args.get_str("<dest>"));
    image::ImageLuma8(outbuf).save(args.get_str("<dest>")).unwrap();
}
