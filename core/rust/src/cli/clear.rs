// src/cls/clear.rs

use std::io::{self, Write};

/// Efface l’écran et replace le curseur en haut à gauche
fn clear_terminal() {
    // ESC[2J = clear screen, ESC[1;1H = move cursor to (1,1)
    print!("\x1B[2J\x1B[1;1H");
    io::stdout().flush().ok();
}

/// Efface l’écran **seulement** si stdout est un terminal (via atty)
pub fn clear() {
    if atty::is(atty::Stream::Stdout) {
        clear_terminal();
    }
}

