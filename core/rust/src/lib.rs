#[link(name = "echo_dice", kind = "static")]
extern "C" {
    fn add_from_cpp(a: i32, b: i32) -> i32;
}

pub fn add_via_cpp(a: i32, b: i32) -> i32 {
    unsafe { add_from_cpp(a, b) }
}
