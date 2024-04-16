use chrono::offset::Utc;
use chrono::DateTime;
use chrono::Datelike;
use chrono::Timelike;
use std::fs;
use std::fs::OpenOptions;
use std::io::{BufWriter, Write};
use std::path::PathBuf;

pub struct Logger {
    path: PathBuf,
}

impl Default for Logger {
    fn default() -> Self {
        Self {
            path: PathBuf::new(),
        }
    }
}

impl Logger {
    pub fn init(&mut self, path: PathBuf) -> Result<(), String> {
        self.path = path;
        let dir = self.path.parent().unwrap();
        fs::create_dir_all(dir).map_err(|e| e.to_string())?;
        Ok(())
    }

    pub fn log_with_time(&self, s: &str) {
        self.log(&(Self::now() + ": " + s));
    }

    pub fn log(&self, s: &str) {
        println!("{}", s);
        let f = OpenOptions::new()
            .append(true)
            .create(true)
            .open(self.path.as_path())
            .expect("Unable to open file");
        let mut f = BufWriter::new(f);
        f.write_all(s.as_bytes()).expect("Unable to write data");
        f.write_all("\r\n".as_bytes())
            .expect("Unable to write data");
    }

    fn now() -> String {
        Self::date_time_to_string(&Utc::now())
    }

    fn date_time_to_string(dt: &DateTime<Utc>) -> String {
        format!(
            "{:04}/{:02}/{:02} {:02}:{:02}:{:02}",
            dt.year(),
            dt.month(),
            dt.day(),
            dt.hour(),
            dt.minute(),
            dt.second()
        )
    }
}
