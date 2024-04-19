#[cfg(target_os = "windows")]
use std::os::windows::process::CommandExt;

use std::process::Command;

pub fn spawn_process(
    proc: &str,
    args: Option<Vec<&str>>,
    working_folder: Option<&str>,
    creation_flags: Option<u32>,
) -> Result<(), String> {
    //println!("proc: {}, args: {:?}", proc, args);

    let mut cmd = Command::new(proc);
    let mut c = &mut cmd;

    if let Some(vec) = args {
        c = c.args(vec);
    };

    if let Some(path) = working_folder {
        c = c.current_dir(path);
    }

    #[cfg(target_os = "windows")]
    if let Some(f) = creation_flags {
        c = c.creation_flags(f);
    }

    match c.spawn() {
        Ok(_) => Ok(()),
        Err(err) => Err(err.to_string()),
    }
}
