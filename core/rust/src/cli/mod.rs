// src/cli/mod.rs

mod commands_cli;  // enregistre help, stop, etc.
mod loop_cli;
mod clear;

pub use loop_cli::{run_cli, stop_cli, RUNNING};
pub use clear::clear;

