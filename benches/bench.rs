use criterion::{criterion_group, criterion_main, Criterion};

use distance_field::{DistanceFieldExt, Options};
use pprof::criterion::{Output, PProfProfiler};

pub fn bench(c: &mut Criterion) {
    let img = image::load_from_memory(include_bytes!("../img/input.png"))
        .unwrap()
        .grayscale();

    c.bench_function("example image, 64x64 output", |b| {
        b.iter(|| {
            img.distance_field(Options {
                size: (64, 64),
                ..Default::default()
            })
        });
    });
}

criterion_group! {
    name = benches;
    config = Criterion::default().with_profiler(PProfProfiler::new(100, Output::Flamegraph(None)));
    targets = bench
}

criterion_main!(benches);
