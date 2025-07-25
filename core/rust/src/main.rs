// src/main.rs

mod api;
mod cli;

use cli::run_cli;
use cli::clear;

fn main() {
    clear();
    run_cli();
}
