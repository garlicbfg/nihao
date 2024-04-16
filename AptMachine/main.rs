mod amount_selector;
mod apt_lib;
mod apt_machine;
mod conditional_reporter;
mod logger;
mod parse_json;
mod reporter;
mod settings;
mod system_routines;

use apt_machine::AptMachine;




#[tokio::main]
async fn main() {
    let mut apt_machine = AptMachine::new();
    apt_machine.run().await;
}
