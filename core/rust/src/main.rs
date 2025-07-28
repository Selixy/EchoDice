// src/main.rs

mod api;
mod cli;
mod info;

use api::handler_message::init_message_handler;
use cli::run_cli;
use cli::clear;
use info::set_ID;

fn main() {
    clear();
    set_ID();
    init_message_handler();
    run_cli();
}
