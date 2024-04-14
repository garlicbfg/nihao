use anyhow::Result;
use aptos_sdk::coin_client::CoinClient;
use aptos_sdk::move_types::identifier::Identifier;
use aptos_sdk::move_types::language_storage::ModuleId;
use aptos_sdk::move_types::language_storage::TypeTag;
use aptos_sdk::rest_client::aptos::Balance;
use aptos_sdk::rest_client::aptos_api_types::Transaction;
use aptos_sdk::rest_client::aptos_api_types::UserTransaction;
use aptos_sdk::rest_client::Client;
use aptos_sdk::rest_client::FaucetClient;
use aptos_sdk::transaction_builder::TransactionBuilder;
use aptos_sdk::types::account_address::AccountAddress;
use aptos_sdk::types::chain_id::ChainId;
use aptos_sdk::types::transaction::EntryFunction;
use aptos_sdk::types::transaction::RawTransaction;
use aptos_sdk::types::transaction::TransactionPayload;
use aptos_sdk::types::LocalAccount;
use std::time::SystemTime;
use std::time::UNIX_EPOCH;



#[derive(Debug)]
pub struct EntryFuncCallResult {
    pub user_transactions: Vec<UserTransaction>,
    pub transaction_result: Option<Result<Transaction, String>>
}



pub struct TransactionOptions {
    pub max_gas_amount: u64,
    pub gas_unit_price: u64,
    /// This is the number of seconds from now you're willing to wait for the transaction to be committed.
    pub timeout_secs: u64,
}



pub struct AptLib {}




impl AptLib {
    pub async fn synchronize_sequence_number(rest_client: &Client, acc: &LocalAccount) -> Result<(), String> {
        let response = rest_client.get_account(acc.address()).await.map_err(|e| e.to_string())?;
        let net_acc = response.inner();
        acc.set_sequence_number(net_acc.sequence_number);
        Ok(())
    }



    pub async fn create_account(rest_client: &Client, d_path: &str, mnemonics: &str) -> Result<LocalAccount, String> {
        let acc = LocalAccount::from_derive_path(d_path, mnemonics, 0).map_err(|e| e.to_string())?;
    
        if let Err(_) = Self::synchronize_sequence_number(rest_client, &acc).await {
            println!("Account not found, using sequence_number = 0");
        }

        Ok(acc)
    }
    
    
    
    pub async fn create_account_on_chain(faucet_client: &FaucetClient, acc: &mut LocalAccount) -> Result<(), String>{
        faucet_client.create_account(acc.address()).await.map_err(|e| e.to_string())
    }
    
    
    
    pub async fn transfer(rest_client: &Client, coin_client: &CoinClient<'_>, src: &mut LocalAccount, dst: &LocalAccount, amount: u64) -> Result<(), String>{
        let txn_hash = coin_client
            .transfer(src, dst.address(), amount, None)
            .await.map_err(|e| e.to_string())?;
        rest_client
            .wait_for_transaction(&txn_hash)
            .await.map_err(|e| e.to_string())?;
    
            Ok(())
    }



    pub async fn call_entry_function_with_sim(rest_client: &Client, sender: &LocalAccount, hex_addr: &str, name: &str, func: &str, ty_args: Vec<TypeTag>, args: Vec<Vec<u8>>, options: &TransactionOptions) -> Result<EntryFuncCallResult, String> {
        let raw_trn = Self::build_raw_transaction(rest_client, sender, hex_addr, name, func, ty_args, args, options).await?;
        let raw_trn_sim = raw_trn.clone(); //make copy for simulation
    
        // simulate transaction
        let res = Self::simulate_transaction(rest_client, raw_trn_sim, sender).await;
        sender.decrement_sequence_number();

        let user_transactions = res?;

        // check response size
        if user_transactions.len() != 1 {
            return Err("Wrong response size".to_string());
        }

        // if simulation succeeded, execute real transaction
        let transaction_result = if user_transactions[0].info.success {
            Some(Self::submit_transaction(rest_client, raw_trn, sender).await)
        }
        else {
            None
        };

        Ok(EntryFuncCallResult { user_transactions, transaction_result })
    }



    pub async fn build_raw_transaction(rest_client: &Client, sender: &LocalAccount, hex_addr: &str, name: &str, func: &str, ty_args: Vec<TypeTag>, args: Vec<Vec<u8>>, options: &TransactionOptions) -> Result<RawTransaction, String> {
        let addr = AccountAddress::from_hex_literal(hex_addr).expect("can't parse address");
        let chain_id = rest_client.get_index().await.map_err(|e| e.to_string())?.inner().chain_id;
    
        let entry_function = EntryFunction::new(ModuleId::new(addr,Identifier::new(name).unwrap()), Identifier::new(func).unwrap(), ty_args, args);
        let payload = TransactionPayload::EntryFunction(entry_function);
        let expiration_timestamp = SystemTime::now().duration_since(UNIX_EPOCH).unwrap().as_secs() + options.timeout_secs;
        let trn_builder = TransactionBuilder::new(payload, expiration_timestamp, ChainId::new(chain_id))
        .sender(sender.address())
        .sequence_number(sender.increment_sequence_number())
        .max_gas_amount(options.max_gas_amount)
        .gas_unit_price(options.gas_unit_price);
    
        Ok(trn_builder.build())
    }



    pub async fn simulate_transaction(rest_client: &Client, raw_trn: RawTransaction, sender: &LocalAccount) -> Result<Vec<UserTransaction>, String> {
        let fake_acc = LocalAccount::generate(&mut rand::rngs::OsRng);
        let signed_trn = raw_trn.sign(fake_acc.private_key(), sender.public_key().clone()).expect("Signing a txn can't fail").into_inner();
        
        let res = rest_client.simulate(&signed_trn).await.map_err(|e| e.to_string())?.into_inner();
        Ok(res)
    }



    pub async fn submit_transaction(rest_client: &Client, raw_trn: RawTransaction, sender: &LocalAccount) -> Result<Transaction, String> {    
        sender.increment_sequence_number();
        let signed_trn = sender.sign_transaction(raw_trn);
        let trn = rest_client.submit(&signed_trn).await.map_err(|e| e.to_string())?.into_inner();
        let res = rest_client.wait_for_transaction(&trn).await.map_err(|e| e.to_string())?.into_inner();
        Ok(res)
    }
    

        
    pub async fn get_balance_token(rest_client: &Client, addr: &AccountAddress, token: &str) -> Result<u64, String> {
        let token_struct = Self::get_token_struct(token);
        let response = rest_client.get_account_resource(addr.clone(), &token_struct).await.map_err(|e| e.to_string())?;
        let inner = response.into_inner();
        let resource = inner.ok_or("No data returned".to_string())?;
        let balance = serde_json::from_value::<Balance>(resource.data).map_err(|e| e.to_string())?;
        Ok(balance.get())
    }



    pub async fn get_balance(rest_client: &Client, addr: &AccountAddress) -> Result<u64, String> {
        Self::get_balance_token(rest_client, addr, &Self::get_default_token()).await.map_err(|e| e.to_string())
    }


    
    pub fn get_token_struct(s: &str) -> String {
        format!("0x1::coin::CoinStore<{}>", s)
    }



    pub fn get_default_token() -> String {
        "0x1::aptos_coin::AptosCoin".to_string()
    }
}
