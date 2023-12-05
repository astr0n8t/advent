use std::fs;
use std::collections::HashMap;

pub fn part1(input_file: &str) -> u32 {
    let input = parse_input(input_file);
    get_num_winning_total(&input).iter()
        .map(|x|
             if *x > 0 {u32::pow(2, x-1)} else {0}
             )
        .sum()
}

pub fn part2(input_file: &str) -> u32 {
    let mut sum = 0;
    let input = parse_input(input_file);
    let totals = get_num_winning_total(&input);
    let mut cards: HashMap<usize,u32> = HashMap::new();
    for x in 0..totals.len() {
        cards.entry(x).or_insert(1);
        let curr = cards[&x];
        for y in 1..=totals[x] {
            cards.entry(x+y as usize).and_modify(|z| *z += curr).or_insert(1+curr);
        }
        sum += curr;
    }
    sum
}

fn get_num_winning_total(input: &[String]) -> Vec<u32> {
    let mut out: Vec<u32> = vec![];
    for line in input.iter() {
        let line = line.split(":").last().unwrap();
        let mut line = line.split("|");
        let history = build_hashmap(
            line.next()
                .unwrap()
            );
        out.push(get_num_winning_card(history, line.next().unwrap()));
    }
    out
}

fn get_num_winning_card(history: HashMap<u32,bool>, input: &str) -> u32 {
    let mut total = 0;
    for num in input.split_whitespace() {
        if history.contains_key(&num.parse::<u32>().unwrap()) {
            total += 1;
        }
    }
    total
}

fn build_hashmap(input: &str) -> HashMap<u32, bool> {
    let mut history: HashMap<u32,bool> = HashMap::new();
    let input = input.trim()
        .split_whitespace();

    for entry in input {
        history.insert(
            entry.trim()
                .parse::<u32>()
                .unwrap(),
            true
            );
    }
    history
}

fn parse_input(input_file: &str) -> Vec<String> {
    let input = fs::read_to_string(input_file)
        .expect("Something went wrong reading the file");

    input.trim_end()
        .split("\n")
        .map(str::to_string)
        .collect()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 13);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 30);
    }
}
