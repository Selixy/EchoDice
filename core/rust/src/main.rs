// core/rust/src/main.rs

use echodice::get_sdp_offer;

fn main() {
    let sdp = get_sdp_offer();
    if sdp.is_empty() {
        eprintln!("Impossible de générer le SDP offer.");
        std::process::exit(1);
    }
    println!("=== SDP Offer ===\n{}", sdp);
}
