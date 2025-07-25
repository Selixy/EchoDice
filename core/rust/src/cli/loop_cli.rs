// src/cli/loop_cli.rs

use crate::api::command::CommandDesc;
use inventory;
use std::{
    io::{self, BufRead},
    sync::{
        atomic::{AtomicBool, Ordering},
        mpsc,
    },
    thread,
    time::Duration,
};

/// Flag global pour contrôler la boucle CLI
pub static RUNNING: AtomicBool = AtomicBool::new(true);

/// Fonction à appeler pour demander l'arrêt de la boucle CLI
pub fn stop_cli() {
    RUNNING.store(false, Ordering::SeqCst);
}

/// Lance la boucle REPL en auto-découvrant toutes les commandes
pub fn run_cli() {
    // 1) Charger toutes les CommandDesc déposées (API + CLI)
    let mut commands = std::collections::HashMap::new();
    for cmd in inventory::iter::<CommandDesc> {
        commands.insert(cmd.name, cmd);
    }

    // 2) Thread de lecture stdin
    let (tx, rx) = mpsc::channel::<String>();
    thread::spawn(move || {
        for line in io::stdin().lock().lines().flatten() {
            let _ = tx.send(line);
        }
    });

    println!("Tapez une commande (par ex. `help` ou `stop`).");

    // 3) Boucle principale
    while RUNNING.load(Ordering::SeqCst) {
        // traiter les commandes en file
        while let Ok(line) = rx.try_recv() {
            let key = line.trim();
            if let Some(cmd) = commands.get(key) {
                // affiche le message puis exécute
                println!("{}", cmd.message);
                (cmd.callback)();
            } else {
                println!("Commande inconnue : `{}` (tapez `help`)", key);
            }
        }
        // pause légère pour ne pas monopoliser le CPU
        thread::sleep(Duration::from_millis(40));
    }
}
