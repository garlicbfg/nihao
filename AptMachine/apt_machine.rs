//use aptos_sdk::coin_client::CoinClient;
//use aptos_sdk::coin_client::TransferOptions;
//use aptos_sdk::rest_client::FaucetClient;
//use aptos_sdk::rest_client::Transaction;
use aptos_sdk::rest_client::Client;
use aptos_sdk::types::LocalAccount;
use aptos_sdk::move_types::language_storage::TypeTag;
use aptos_sdk::bcs;
use std::path::PathBuf;
use std::str::FromStr;
use url::Url;
use std::cmp;
use tokio::time::sleep;
use crate::logger::Logger;
use crate::conditional_reporter::ConditionalReporter;
use crate::settings::Settings;
use crate::amount_selector::AmountSelector;
use crate::apt_lib::AptLib;
use crate::apt_lib::TransactionOptions;
use crate::apt_lib::EntryFuncCallResult;
use std::time::SystemTime;
use std::time::UNIX_EPOCH;




static PROG_NAME: &str = "AptMachine";
static VERSION: &str = "1.8";
static CONF_FILE: &str = "aptmachine.conf";
static STAPT_COIN: &str = "0xd11107bdf0d6d7040c6c0bfbdecb6545191fdf13e8d8d259952f53e1713f61b5::staked_coin::StakedAptos";
static DITTO_CONTRACT_ADDR: &str = "0xd11107bdf0d6d7040c6c0bfbdecb6545191fdf13e8d8d259952f53e1713f61b5";
static DITTO_MODULE_NAME: &str = "ditto_staking";
static DITTO_FUNC_NAME: &str = "instant_unstake";
static LIQUIDSWAP_CONTRACT_ADDR: &str = "0x99465840349525b3d648e609eacb056d5b61e3915e8a3972b431afc26e3f05a1";
static LIQUIDSWAP_MODULE_NAME: &str = "router";
static LIQUIDSWAP_FUNC_NAME: &str = "swap_coin_for_coin_x1";
static LIQUIDSWAP_CURVE_0: &str = "0x190d44266241744264b964a37b8f09863167a12d3e70cda39376cfb4e3561e12::curves::";
static LIQUIDSWAP_CURVE_05: &str = "0x163df34fccbf003ce219d3f1d9e70d140b60622cb9dd47599c25fb2f797ba6e::curves::";
static LIQUIDSWAP_STABLE: &str = "Stable";
static LIQUIDSWAP_UNCORRELATED: &str = "Uncorrelated";
static MNEMONICS_HIDE: &str = "word1 word2 word3 word4 word5 word6 word7 word8 word9 word10 word11 word12";

static EVENT_START_LOOP: u32 = 0;
static EVENT_ZERO_BALANCE: u32 = 1;
static EVENT_NONZERO_BALANCE: u32 = 2;
static EVENT_THRESHOLD_BALANCE: u32 = 3;
static EVENT_RECHARGE: u32 = 4;




fn get_full_name() -> String
{
    format!("{} version {}", PROG_NAME, VERSION)
}


#[derive(Debug)]
pub struct DittoCallResult {
    pub sim_success: bool,
    pub wrong_sn: bool,
    pub insufficient_buffer: bool,
    pub vm_status: String,
    pub real_success: bool
}



pub struct AptMachine {
    settings: Settings,
    logger: Logger,
    reporter: ConditionalReporter,
    amount_selector: AmountSelector,
    recharge_attempt_time: u128
}



impl AptMachine {
    pub fn new() -> AptMachine {
        AptMachine {
            settings: Settings::default(),
            logger: Logger::default(),
            reporter: ConditionalReporter::default(),
            amount_selector: AmountSelector::default(),
            recharge_attempt_time: 0
        }
    }


    pub async fn run(&mut self) {
        // try to read settings
        match Settings::from(&PathBuf::from(CONF_FILE)) {
            Ok(s) => {
                println!("read settings from \'{}\'", CONF_FILE);
                self.settings = s;
            },
            Err(e) => {
                println!("Error: {}\r\nUse default settings", e.to_string());
            }
        };

        // print settings
        let mnemonics = self.settings.mnemonics.clone();
        self.settings.mnemonics = String::from(MNEMONICS_HIDE);
        //println!("{:?}", self.settings);

        //init logger
        self.logger.init(PathBuf::from(&self.settings.log_file)).expect("can't create log dir");
    
        let node_url = Url::from_str(&self.settings.node_addr).unwrap();
        //let faucet_url = Url::from_str(&self.settings.faucet_addr).unwrap();
    
        self.logger.log("");
        self.logger.log("");
        self.logger.log(&get_full_name());
        self.logger.log_with_time(&format!("New session, node_url: {}, faucet_url: {}", self.settings.node_addr, self.settings.faucet_addr));

        // setup reporter
        self.reporter.set_message_format(&self.settings.message_format);
        self.reporter.set_instruction_format(&self.settings.instruction_format);
        if self.settings.report_start_machine {
            self.reporter.enable(EVENT_START_LOOP)
        }
        if self.settings.report_zero_balance {
            self.reporter.enable(EVENT_ZERO_BALANCE)
        }
        if self.settings.report_nonzero_balance {
            self.reporter.enable(EVENT_NONZERO_BALANCE)
        }
        if self.settings.report_threshold_balance {
            self.reporter.enable(EVENT_THRESHOLD_BALANCE)
        }
        if self.settings.report_recharge {
            self.reporter.enable(EVENT_RECHARGE)
        }

        self.reporter.set_reported(EVENT_NONZERO_BALANCE);

        // setup amount selector
        self.amount_selector.set_params(self.settings.amounts.clone(), self.settings.amount_increase_points.clone());

        // set transaction options
        let options = TransactionOptions {
            max_gas_amount: 5_000,
            gas_unit_price: 100,
            timeout_secs: 10
        };

        let rest_client = Client::new(node_url.clone());
        //let coin_client = CoinClient::new(&rest_client);
        //let faucet_client = FaucetClient::new(faucet_url, node_url);
    
        let acc = AptLib::create_account(&rest_client, &self.settings.derive_path, &mnemonics).await.expect("can't create account from mnemonics");
    
        // Print account address
        self.logger.log(&format!("Address: {}", acc.address().to_hex_literal()));

        // report 'Start loop'
        self.report(EVENT_START_LOOP, "Started loop");
    
        loop {
             // read balance
             let (apt_balance, stapt_balance) = match self.request_balance(&rest_client, &acc).await {
                Ok((a, s)) => (a, s),
                Err(e) => {
                    self.logger.log(&format!("Error: {}", e));
                    self.pause(self.settings.pause_after_balance_error).await;
                    continue;
                }
             };
            
            // Print balances
            self.logger.log(&format!("Sequence number: {}. Balance: {} APT, {} stAPT", acc.sequence_number(), Self::human(apt_balance), Self::human(stapt_balance)));
        
            // Report if low balance
            if stapt_balance < self.settings.recharge_threshold && stapt_balance > 0 {
                self.report(EVENT_THRESHOLD_BALANCE, &format!("Low balance: {} stAPT, (APT: {})", Self::human(stapt_balance), Self::human(apt_balance)));
            } else {
                self.reporter.reset_reported(EVENT_THRESHOLD_BALANCE);
            }

            // Report if zero or non-zero balance occured
            if stapt_balance == 0 {
                self.reporter.reset_reported(EVENT_NONZERO_BALANCE);
                self.report(EVENT_ZERO_BALANCE, &format!("stAPT zero balance. (APT: {})", Self::human(apt_balance)));

            } else {
                self.reporter.reset_reported(EVENT_ZERO_BALANCE);
                self.report(EVENT_NONZERO_BALANCE, &format!("Non-zero balance: {} stAPT, (APT: {})", Self::human(stapt_balance), Self::human(apt_balance)));
            }

            // try to recharge if neccessary
            if stapt_balance < self.settings.recharge_threshold {
                self.recharge(&rest_client, &acc, &options).await;
            }

            if stapt_balance == 0 {
                self.recharge(&rest_client, &acc, &options).await;
                self.pause(self.settings.pause_after_no_money).await;
                continue;
            }

            // get amount supposed
            let (level, amount) = self.amount_selector.get_amount();

            // try to unstake
            let amount_corr = cmp::min(amount, stapt_balance);
    
            self.logger.log(&format!("Trying to unstake: {}", amount_corr));
    
            let res = self.call_ditto_unstake(&rest_client, &acc, amount_corr, &options).await;
            
            //handle results
            match res {
                Err(e) => {
                    // Scenario 1. Unhandled error: continue.
                    self.logger.log(&format!("Unhandled error: {}", e));
                    self.pause(self.settings.pause_after_attempt).await;
                },
                Ok(call_res) => {
                    match call_res.real_success {
                        // Scenario 2. Success! No sleep - repeat action immediately
                        true => {
                            self.logger.log("=== Transaction succeeded! ===");
                            self.amount_selector.handle_success();
                        },
                        // Simulation failed.
                        false => {
                            // Scenario 3. Simulation succeeded, but real transaction failed: continue.
                            if call_res.sim_success {
                                self.logger.log(&format!("Transaction failed"));
                                self.pause(self.settings.pause_after_attempt).await;
                            }
                            // Scenario 4. Simulation failed (insufficient buffer): continue.
                            else if call_res.insufficient_buffer {
                                self.logger.log("Insufficient buffer");
                                self.amount_selector.handle_failure();
                                if level == 0 {
                                    self.pause(self.settings.pause_after_attempt).await;
                                }
                            }
                            // Scenario 5. Simulation failed (wrong sequence number): synchronize it.
                            else if call_res.wrong_sn {
                                self.logger.log("Wrong sequence number");
                                let _ = AptLib::synchronize_sequence_number(&rest_client, &acc).await;
                                self.pause(self.settings.pause_after_sync_sn).await;
                            }
                            // Scenario 6. Simulation failed (unknown error): sleep awhile, then continue.
                            else {
                                self.logger.log(&format!("Unknown error: {}", call_res.vm_status));
                                self.pause(self.settings.pause_after_attempt).await;
                            }
                        }
                    }
                }
            }
        }
    }



    async fn recharge(&mut self, rest_client: &Client, acc: &LocalAccount, options: &TransactionOptions) {
        let timestamp = SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_millis();
        if timestamp < self.recharge_attempt_time {
            return;
        }

        let min_receive_amount = (self.settings.recharge_amount as f64 / self.settings.max_stapt_price).round() as u64;
        self.logger.log(&format!("requesting swap {} APT --> {} stAPT", self.settings.recharge_amount, min_receive_amount));

        let res = self.call_liquidswap_swap(&rest_client, &acc,
            self.settings.recharge_amount, min_receive_amount, 0x05, false, options).await;
        let mut success = false;            
        match res {
            Err(e) => {
                self.logger.log(&format!("EntryFunc call error: {}", e));
            },
            Ok(call_result) => match call_result.transaction_result {
                Some(tr_res) => match tr_res {
                    Err(e) => {
                        self.logger.log(&format!("Simulation ok, but real transaction failed: {}", e));
                    },
                    Ok(tr) => {
                        self.logger.log(&format!("== Recharged successfully! =="));
                        success = true;
                        let bal = match self.request_balance(&rest_client, &acc).await {
                            Ok((apt_balance, stapt_balance)) => format!("Balance: {} APT, {} stAPT", Self::human(apt_balance), Self::human(stapt_balance)),
                            Err(e) => format!("Error: {}", e)
                        };
                        let msg = "Successfully recharged. ".to_string() + &bal;
                        self.report(EVENT_RECHARGE, &msg);
                    }
                },
                None => {
                    self.logger.log(&format!("Simulation failed: {}", call_result.user_transactions[0].info.vm_status));
                }
            }
        }
        // TODO: add
        //INSUFFICIENT_BALANCE
        //ERR_INSUFFICIENT_AMOUNT

        self.recharge_attempt_time = SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_millis() + self.settings.min_time_between_swap_requests as u128;
    }



    async fn call_ditto_unstake(&self, rest_client: &Client, sender: &LocalAccount, amount: u64, options: &TransactionOptions) -> Result<DittoCallResult, String> {
        let args = vec![bcs::to_bytes(&amount).unwrap()];
        let res = AptLib::call_entry_function_with_sim(rest_client, sender, DITTO_CONTRACT_ADDR, DITTO_MODULE_NAME, DITTO_FUNC_NAME, vec![], args, options).await?;

        let info = &res.user_transactions[0].info;
        Ok(DittoCallResult {
            sim_success: info.success,
            real_success: 
                match res.transaction_result {
                    Some(r) => match r {
                        Ok(_) => true,
                        Err(_) => false
                    }
                    None => false
                },
            wrong_sn: info.vm_status.contains("SEQUENCE_NUMBER_TOO_NEW") || info.vm_status.contains("SEQUENCE_NUMBER_TOO_OLD"),
            insufficient_buffer: info.vm_status.contains("ERR_INSUFFICIENT_APTOS_BUFFER"),
            vm_status: info.vm_status.clone(),
        })
    }



    async fn call_liquidswap_swap(&self, rest_client: &Client, sender: &LocalAccount, coin_in: u64, coin_out_min: u64, ls_version: u8, uncorrelated: bool, options: &TransactionOptions) -> Result<EntryFuncCallResult, String> {
        
        let args = vec![
            bcs::to_bytes(&coin_in).unwrap(),
            bcs::to_bytes(&vec![coin_out_min]).unwrap(),
            bcs::to_bytes(&vec![ls_version]).unwrap()
        ];
        
        let mut curve_addr = match ls_version {
            0 => LIQUIDSWAP_CURVE_0.to_string(),
            5 => LIQUIDSWAP_CURVE_05.to_string(),
            _ => "".to_string()
        };
        
        curve_addr += match uncorrelated {
            false => LIQUIDSWAP_STABLE,
            true => LIQUIDSWAP_UNCORRELATED
        };

        let ty_args = vec![
            TypeTag::from_str(&AptLib::get_default_token()).unwrap(),
            TypeTag::from_str(STAPT_COIN).unwrap(),
            TypeTag::from_str(&curve_addr).unwrap()
        ];

        AptLib::call_entry_function_with_sim(rest_client, sender, LIQUIDSWAP_CONTRACT_ADDR, LIQUIDSWAP_MODULE_NAME, LIQUIDSWAP_FUNC_NAME, ty_args, args, options).await
    }


    

    async fn request_balance(&self, rest_client: &Client, acc: &LocalAccount) -> Result<(u64, u64), String> {
        // read APT balance
        let apt_balance_res = AptLib::get_balance(&rest_client, &acc.address()).await;
        if let Err(e) = apt_balance_res {
            return Err(format!("Failed to get apt balance: {}", e));
        }
        let apt_balance = apt_balance_res.unwrap();

        // read stAPT balance
        let stapt_balance_res = AptLib::get_balance_token(&rest_client, &acc.address(), &STAPT_COIN).await;
        if let Err(e) = stapt_balance_res {
            return Err(format!("Failed to get stapt balance: {}", e));
        }
        let stapt_balance = stapt_balance_res.unwrap();        

        Ok((apt_balance, stapt_balance))
    }
    


    async fn pause(&self, t: u64) {
        self.logger.log(&format!("Sleeping for {} ms", t));
        sleep(std::time::Duration::from_millis(t)).await;
    }



    fn report(&mut self, id: u32, msg: &str) {
        match self.reporter.report(id, msg) {
            Ok(b) => if b {
                    self.logger.log("Reported successfully")
                },
            Err(s) => self.logger.log(&format!("Failed to report: {}", s))
        }
    }



    fn human(n: u64) -> f32 {
        n as f32 / 100_000_000.
    }
}

