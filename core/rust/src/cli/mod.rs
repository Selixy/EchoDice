// src/cli/mod.rs

mod commands_cli;
mod loop_cli;
mod clear;

pub use loop_cli::{run_cli, stop_cli};
pub use clear::clear;

