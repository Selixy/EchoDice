// src/main.rs

mod api;
mod cli;
mod info;

use cli::run_cli;
use cli::clear;
use info::set_ID;

fn main() {
    clear();
    set_ID();
    run_cli();
}
