#[derive(Debug)]
pub struct AmountSelector {
    level: usize,
    amount_sizes: Vec<u64>,
    points_to_levelup: Vec<u8>,
    points: u8,
    ascending: bool,
}

impl Default for AmountSelector {
    fn default() -> Self {
        Self {
            level: 0,
            amount_sizes: Vec::new(),
            points_to_levelup: Vec::new(),
            points: 0,
            ascending: true,
        }
    }
}

impl AmountSelector {
    pub fn set_params(&mut self, amounts: Vec<u64>, points: Vec<u8>) {
        self.amount_sizes = amounts;
        self.points_to_levelup = points;
    }

    pub fn get_amount(&mut self) -> (usize, u64) {
        (self.level, self.amount_sizes[self.level])
    }

    pub fn handle_success(&mut self) {
        self.points += 1;

        if self.level == self.amount_sizes.len() - 1 {
            return;
        }

        let threshold_a = self.points_to_levelup[self.level];
        let threshold = ((self.amount_sizes[self.level + 1] as f32) / (self.amount_sizes[self.level] as f32))
            .ceil() as u8;

        if (self.ascending && self.points >= threshold_a) || self.points >= threshold {
            self.level += 1;
            self.points = 0;
            self.ascending = true;
        }
    }

    pub fn handle_failure(&mut self) {
        self.points = 0;

        if self.level == 0 {
            self.ascending = true;
        } else {
            self.level -= 1;
            self.ascending = false;
        }
    }
}
