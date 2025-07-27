// src/cli/commands_cli.rs

use crate::api::command::CommandDesc;
use crate::cli::loop_cli::stop_cli;
use inventory;

// Callback pour afficher la liste des commandes
fn help() {
    println!("[CLI] Commandes disponibles :");
    // on itère directement sur l’inventaire
    for cmd in inventory::iter::<CommandDesc> {
        println!("  {:<20} → {}", cmd.name, cmd.description);
    }
}

// Enregistre la commande "help"
inventory::submit! {
    CommandDesc {
        name:        "help",
        description: "afficher la liste des commandes",
        message:     "[CLI] help:",
        callback:    |_arg| help(),
    }
}

// Enregistre la commande "stop" pour quitter la boucle
inventory::submit! {
    CommandDesc {
        name:        "stop",
        description: "arrêter la boucle CLI",
        message:     "[CLI] au revoir !",
        callback:    |_arg| stop_cli(),
    }
}
