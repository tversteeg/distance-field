# distance-field [![Build Status](https://travis-ci.org/tversteeg/distance-field.svg?branch=master)](https://travis-ci.org/tversteeg/distance-field) [![Cargo](https://img.shields.io/crates/v/distance-field.svg)](https://crates.io/crates/distance-field) [![Documentation](https://docs.rs/distance-field/badge.svg)](https://docs.rs/distance-field/) [![License: GPL-3.0](https://img.shields.io/crates/l/distance-field.svg)](#license) [![Downloads](https://img.shields.io/crates/d/distance-field.svg)](#downloads)

Library for generating distance field bitmaps to render as pseudo-vector images in a shader

### [Documentation](https://docs.rs/distance-field/)

### How to run

Checkout the repository and `cd` into it; then run:

    cargo run --example generator -- img/input.png output.png 50 50

This will take the following image as input:

![Rust logo](img/input.png?raw=true)

And generate a distance field:

![Distance field](img/output.png?raw=true)
