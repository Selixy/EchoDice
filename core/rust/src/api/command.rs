// src/api/command.rs

use inventory::collect;

/// Description d’une commande CLI
pub struct CommandDesc {
    /// clé tapée par l’utilisateur
    pub name: &'static str,
    /// courte description pour « help »
    pub description: &'static str,
    /// message à afficher en exécutant la commande
    pub message: &'static str,
    /// fonction à appeler
    pub callback: fn(Option<&str>),
}

// instancie l’inventaire
inventory::collect!(CommandDesc);
