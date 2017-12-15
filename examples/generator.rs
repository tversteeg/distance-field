extern crate docopt;

use docopt::Docopt;

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
}
