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

pub static RUNNING: AtomicBool = AtomicBool::new(true);
pub fn stop_cli() { RUNNING.store(false, Ordering::SeqCst); }

pub fn run_cli() {
    // Charge toutes les CommandDesc
    let mut commands = std::collections::HashMap::new();
    for cmd in inventory::iter::<CommandDesc> {
        commands.insert(cmd.name, cmd);
    }

    // Thread de lecture stdin
    let (tx, rx) = mpsc::channel::<String>();
    thread::spawn(move || {
        for line in io::stdin().lock().lines().flatten() {
            let _ = tx.send(line);
        }
    });

    println!("Tapez une commande (tapez \"help\" pour visualiser la liste).");

    while RUNNING.load(Ordering::SeqCst) {
        while let Ok(line) = rx.try_recv() {
            let key = line.trim();

            // On cherche un match exact ou un préfixe "name/param"
            let mut handled = false;
            for (name, cmd) in &commands {
                if key == *name {
                    // commande sans param
                    println!("{}", cmd.message);
                    (cmd.callback)(None);
                    handled = true;
                    break;
                }
                // si la saisie commence par "name/"
                let prefix = format!("{}/", name);
                if key.starts_with(&prefix) {
                    let param = &key[prefix.len()..];
                    println!("{}", cmd.message);
                    (cmd.callback)(Some(param));
                    handled = true;
                    break;
                }
            }

            if !handled {
                println!("Commande inconnue : `{}` (tapez `help`)", key);
            }
        }

        thread::sleep(Duration::from_millis(40));
    }
}
