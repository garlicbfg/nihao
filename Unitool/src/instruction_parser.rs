use std::convert::TryInto;
use std::vec::Vec;
use std::collections::HashMap;




#[derive(Debug)]
enum ArgType
{
    Int,
    Float,
    String,
    ExistingFile,
    NewFile,
    Repeat
}



#[derive(Debug)]
struct ArgumentsDescription
{
    arguments: Vec<ArgType>,
    flags: Vec<char>
}


#[derive(Debug)]
struct InstructionDescription
{
    min_args: u32,
    max_args: u32,
    signatures: Vec<ArgumentsDescription>
}


impl InstructionDescription
{
    fn new(args: &str, flags: &str) -> InstructionDescription
    {
        let mut ret = InstructionDescription
        {
            min_args: u32::MAX,
            max_args: 0,
            signatures: Vec::new()
        };
        ret.add(args, flags);
        ret
    }


    fn add(&mut self, arguments: &str, flags: &str)
    {
        let mut v: Vec<&str> = Vec::new();
        if arguments.len() != 0
        {
            v = arguments.split(' ').collect();
        };

        let mut repeat: bool = false;

        let mut signature: Vec<ArgType> = Vec::new();

        for s in v
        {
            match s
            {
                "i" => signature.push(ArgType::Int),
                "f" => signature.push(ArgType::Float),
                "s" => signature.push(ArgType::String),
                "e" => signature.push(ArgType::ExistingFile),
                "n" => signature.push(ArgType::NewFile),
                "r" => repeat = true,
                _ =>
                {
                    println!("Wrong signature: {}", arguments);
                    return;
                }
            };
        }

        let flags_vec: Vec<char> = flags.chars().collect();

        let min_args: u32 = signature.len().try_into().unwrap();

        let max_args: u32 = if repeat { signature.push(ArgType::Repeat); u32::MAX } else { min_args };

        let ad = ArgumentsDescription
        {
            arguments: signature,
            flags: flags_vec
        };

        self.signatures.push(ad);
        if max_args > self.max_args { self.max_args = max_args };
        if min_args < self.min_args { self.min_args = min_args };
    }
}



#[derive(Debug)]
pub struct InstructionParser
{
    instructions : HashMap<String, InstructionDescription>,
}

impl InstructionParser
{
    pub fn new() -> InstructionParser
    {
        InstructionParser
        {
            instructions: HashMap::new()
        }
    }


    pub fn add(&mut self, input_str: &str)
    {
        let index = match input_str.find(' ')
        {
            Some(num) => num,
            None => input_str.len()
        };

        let name: &str = &input_str[..index].trim();
        let residue: &str = &input_str[index..].trim();

        let (index2, index3) = match residue.find('-')
        { 
            Some(num) => (num, num + 1),
            None => (residue.len(), residue.len())
        };

        let args: &str = &residue[..index2].trim();
        let flags: &str = &residue[index3..].trim();

        println!("Adding name \'{}\' args \'{}\' flags \'{}\'", name, args, flags);

        match self.instructions.get_mut(name)
        {
            Some(v) =>
            { 
                v.add(args, flags); 
            },
            None => 
            {
                self.instructions.insert(name.to_string(), InstructionDescription::new(args, flags));
            }
        };
    }

    pub fn to_string(&self) -> String
    {
        let mut ret = String::new();
        for (key, value) in self.instructions.iter() 
        {
            let tstr: String = format!("{}: [{}, {}]\n", key, value.min_args, value.max_args);
            ret.push_str(&tstr);
        }
        ret
    }
}
