use std::ffi::CString;
use std::io::{self, Write};
use std::os::raw::c_char;

use crate::api::bindings::Set_ID;

use lazy_static::lazy_static;
use std::sync::Mutex;

#[derive(Debug)]
pub struct Info {
    pub id: String,
}

lazy_static! {
    pub static ref INFO: Mutex<Info> = Mutex::new(Info {
        id: String::new(),
    });
}

pub fn set_ID() {
    print!("Entrez votre ID : ");
    io::stdout().flush().unwrap();

    let mut input = String::new();
    io::stdin().read_line(&mut input).unwrap();
    let input = input.trim();

    {
        let mut info = INFO.lock().unwrap();
        info.id = input.to_string();
    }

    let c_str = CString::new(input).expect("CString::new failed");
    unsafe {
        Set_ID(c_str.as_ptr() as *const c_char);
    }

    println!("ID défini : {}", input);
}
