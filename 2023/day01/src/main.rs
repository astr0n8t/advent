use std::env;
use std::{
    fs::File,
    io::{prelude::*, BufReader},
};

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() < 2 {
        panic!("Missing puzzle input argument!");
    }

    let filename = args[1].clone();

    let file = File::open(filename).expect("Unable to open puzzle input!");
    let buf = BufReader::new(file);
    let input: Vec<String> = buf.lines()
        .map(|l| l.expect("Could not parse line"))
        .collect();

    println!("Number of lines {}", input.len().to_string());

    println!("Answer for part1 is {}", part1(input.clone()).to_string())
}

fn part1(input: Vec<String>) -> i64 {
    let mut answer: i64 = 0;
    for line in input {
        let mut sum: String = String::from("");
        sum.push(first_digit(&line));
        sum.push(first_digit(&line.chars().rev().collect::<String>()));
        answer += sum.parse::<i64>().unwrap();
    }
    answer
}

fn first_digit(input: &String) -> char {
    for c in input.chars() {
        if c.is_digit(10) {
            return c;
        }
    }
    '0'
}
