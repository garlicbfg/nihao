use rand::Rng;
use std::cmp::Ordering;
use std::io;



fn main() 
{
    let mut maxres = 0;
    for i in 1..1000
    {
        let res = testNum(i);
        if res > maxres
        {
            maxres = res;
            println!("{}, result {}",i , res);
        }
    }
}

fn testNum(x :u128) -> i32
{
    let mut x : u128 = x;
    let mut counter = 0;

    let result = loop 
    {
        if x == 1 
        {
            break counter;
        }
        counter += 1;
        
        if x%2 == 0
           {x /= 2}
        else
           {x = x*3 +1}
    };
    result
}

/*
fn main() 
{
    println!("Guess the number!");

    let secret_number = rand::thread_rng().gen_range(1..101);

    println!("The secret number is: {}", secret_number);

    loop
    {

        println!("Please input your guess.");

        let mut guess = String::new();
    
        io::stdin()
            .read_line(&mut guess)
            .expect("Failed to read line");
    
        println!("You guessed: {}", guess);
    
        //let guess: u32 = guess.trim().parse().expect("Please type a number!");

        let guess: u32 = match guess.trim().parse() {
            Ok(num) => num,
            Err(_) => continue,
        };
    
        match guess.cmp(&secret_number)
        {
            Ordering::Less => println!("Too small!"),
            Ordering::Greater => println!("Too big!"),
            Ordering::Equal => 
            {
                println!("You win!");
                break;
            }
        }
    }
}*/