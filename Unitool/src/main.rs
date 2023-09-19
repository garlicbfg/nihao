use std::io::{self, Write};

mod instruction_parser;






fn main() 
{
    let mut parser = instruction_parser::InstructionParser::new();

    parser.add("exit");
    parser.add("add i s r -ba");
    parser.add("add i s -ba");
    parser.add("run s -x");
    parser.add("run -x");
    println!("Printing possible instructions:");
    println!("{:?}", parser);

    println!("Enter instruction:");
    loop
    {
        let mut input_str = String::new();

        print!(":");
        io::stdout().flush();
        io::stdin()
            .read_line(&mut input_str)
            .expect("Failed to read line");
    
            //let guess: u32 = match input_str.trim().parse()
            //{
                //Ok(num) => num,
                //Err(_) => continue,
            //};
        
            let v: Vec<&str> = input_str.split(' ').collect();

            for s in v 
            {
                println!("{}", s)
            }

            //println!("You entered: {}", input_str);
    }
}
