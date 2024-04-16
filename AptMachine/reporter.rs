use crate::system_routines::spawn_process;

pub struct Reporter {
    message_format: String,
    instruction_format: String,
}

impl Default for Reporter {
    fn default() -> Self {
        Self {
            message_format: String::new(),
            instruction_format: String::new(),
        }
    }
}

impl Reporter {
    pub fn set_message_format(&mut self, message_format: &str) {
        self.message_format = message_format.to_string();
    }

    pub fn set_instruction_format(&mut self, instruction_format: &str) {
        self.instruction_format = instruction_format.to_string();
    }

    pub fn report(&self, msg: &str) -> Result<(), String> {
        let text = self.message_format.replace("{%MSG}", msg);
        let b64 = base64::encode(text);

        let instruction = self.instruction_format.replace("{%MSG}", &b64);

        let tokens: Vec<&str> = instruction.split_whitespace().collect();

        if tokens.is_empty() {
            return Err("wrong instruction".to_string());
        }

        let proc = tokens[0];
        let args = tokens[1..].to_vec();

        spawn_process(proc, Some(args), None, None)
    }
}
