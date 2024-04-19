use std::collections::HashSet;
use crate::reporter::Reporter;




pub struct ConditionalReporter {
    enabled: HashSet<u32>,
    reported: HashSet<u32>,
    r: Reporter
}



impl Default for ConditionalReporter {
    fn default() -> Self {
        Self {
            enabled: HashSet::default(),
            reported: HashSet::default(),
            r: Reporter::default()
        }
    }
}



impl ConditionalReporter {
    pub fn set_message_format(&mut self, message_format: &str) {
        self.r.set_message_format(message_format);
    }



    pub fn set_instruction_format(&mut self, instruction_format: &str) {
        self.r.set_instruction_format(instruction_format);
    }



    pub fn enable(&mut self, id: u32) {
        self.enabled.insert(id);
    }



    pub fn disable(&mut self, id: u32) {
        self.enabled.remove(&id);
    }



    pub fn reset_reported(&mut self, id: u32) {
        self.reported.remove(&id);
    }



    pub fn set_reported(&mut self, id: u32) {
        self.reported.insert(id);
    }



    pub fn report(&mut self, id: u32, msg: &str) -> Result<bool, String> {
        if !self.enabled.contains(&id) {
            return Ok(false);
        }

        if !self.reported.contains(&id) {
            self.reported.insert(id);
            self.r.report(msg).map(|_| true)
        } else {
            Ok(false)
        }
    }
}

