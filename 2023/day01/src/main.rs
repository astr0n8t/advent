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

    println!("Answer for part1 is {}", part1(&input).to_string());
    println!("Answer for part2 is {}", part2(&input).to_string());
}

fn part1(input: &Vec<String>) -> i64 {
    let mut answer: i64 = 0;
    for line in input {
        let mut sum: String = String::from("");
        sum.push(first_digit(&line).0);
        sum.push(first_digit(&line.chars().rev().collect::<String>()).0);
        answer += sum.parse::<i64>().unwrap();
    }
    answer
}

fn part2(input: &Vec<String>) -> i64 {
    let written: [(&str, char); 10] = [
        ("zero", '0'), 
        ("one", '1'), 
        ("two", '2'),
        ("three", '3'),
        ("four", '4'),
        ("five", '5'),
        ("six", '6'),
        ("seven", '7'),
        ("eight", '8'),
        ("nine", '9')
    ];
    let written_back: [(&str, char); 10] = [
        ("orez", '0'), 
        ("eno", '1'), 
        ("owt", '2'),
        ("eerht", '3'),
        ("ruof", '4'),
        ("evif", '5'),
        ("xis", '6'),
        ("neves", '7'),
        ("thgie", '8'),
        ("enin", '9')
    ];
    let mut answer: i64 = 0;
    for line in input {
        let mut sum: String = String::from("");

        let (digit, digit_index) = first_digit(&line);
        let (written_digit, written_digit_index) = first_written_digit(&line, &written);
        if written_digit_index < digit_index {
            sum.push(written_digit);
        } else {
            sum.push(digit);
        }

        let (digit, digit_index) = first_digit(&line.chars().rev().collect::<String>());
        let (written_digit, written_digit_index) = first_written_digit(
            &line.chars().rev().collect::<String>(), 
            &written_back,
        );
        if written_digit_index < digit_index {
            sum.push(written_digit);
        } else {
            sum.push(digit);
        }

        answer += sum.parse::<i64>().unwrap();
    }
    answer
}

fn first_digit(input: &String) -> (char, u32) {
    let mut index: u32 = 0;
    for c in input.chars() {
        if c.is_digit(10) {
            return (c, index);
        }
        index += 1;
    }
    ('0',10000)
}

fn first_written_digit(input: &String, written: &[(&str,char)]) -> (char, u32) {
    let mut index: u32 = 10000;
    let mut first_digit: char = '0';
    for digit in written {
        match input.find(digit.0) {
            None => {},
            Some(digit_index) => {
                if (digit_index as u32) < index {
                    index = digit_index as u32;
                    first_digit = digit.1;
                }
            }

        }
    }
    (first_digit, index)
}
