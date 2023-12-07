use std::fs;
use std::collections::HashMap;
use std::cmp::Ordering;

pub fn part1(input_file: &str) -> usize {
    let mut input = parse_input(input_file, false);
    input.sort();
    input.iter().rev().enumerate().map(|(x,y)| {
        y.bid*(x+1)
    }).sum()
}

pub fn part2(input_file: &str) -> usize {
    let mut input = parse_input(input_file, true);
    input.sort();
    input.iter().rev().enumerate().map(|(x,y)| {
        y.bid*(x+1)
    }).sum()
}


#[derive(Debug, Eq, PartialEq)]
struct Hand {
    pub kind: u8,
    pub cards: String,
    pub bid: usize,
    pub joker_enable: bool,
}

impl Hand {
    pub fn new() -> Self {
        Self { cards: String::from(""), kind: 0, bid: 0, joker_enable: false }
    }

    fn from_str(s: &str, joker_enable: bool) -> Self {
        let mut hand = Hand::new();
        hand.joker_enable = joker_enable;
        let s = String::from(s);
        let mut s = s.split_whitespace();

        hand.cards = s.next().unwrap().to_string();
        hand.bid = s.next().unwrap().parse::<usize>().unwrap();
        
        let mut hist: HashMap<char,u8> = HashMap::with_capacity(3);
        for c in hand.cards.chars() {
           hist.entry(c).and_modify(|y| *y += 1).or_insert(1);
        }

        // Funky truth table
        hand.kind = match hist.len() {
            1 => 1,
            2 => match hist.values().next().unwrap() {
                1 => 2,
                4 => 2,
                2 => 3,
                3 => 3,
                _ => unreachable!(),
            },
            3 => match hist.values().next().unwrap() {
                3 => 4,
                2 => 5,
                1 => match hist.values().nth(1).unwrap() {
                    3 => 4,
                    2 => 5,
                    1 => 4,
                    _ => unreachable!(),
                },
                _ => unreachable!(),
            },
            4 => 6,
            5 => 7,
            _ => unreachable!(),
        };

        if joker_enable && hist.contains_key(&'J') {
            hand.apply_joker(hist[&'J']);
        }

        hand
    }

    fn apply_joker(&mut self, joker: u8) {
        self.kind = match self.kind {
            1 => 1,
            2 => 1,
            3 => 1,
            4 => 2,
            5 => match joker {
                2 => 2,
                1 => 3,
                _ => unreachable!(),
            },
            6 => 4,
            7 => 6,
            _ => unreachable!(),
        };
    }

}

impl Ord for Hand {
    fn cmp(&self, other: &Self) -> Ordering {
        if self.kind == other.kind {
            let ord = if self.joker_enable {
                String::from("AKQT98765432J")
            } else {
                String::from("AKQJT98765432")
            };
            let mut n_cmp = 0;
            while self.cards.chars().nth(n_cmp).unwrap()
                == other.cards.chars().nth(n_cmp).unwrap()
                && n_cmp < 5 { 
                n_cmp += 1 
            }
            return ord.find(
                self.cards.chars().nth(n_cmp).unwrap()
                ).unwrap().cmp(
                    &ord.find(other.cards.chars().nth(n_cmp).unwrap()).unwrap()
                );
        }
        self.kind.cmp(&other.kind)
    }
}

impl PartialOrd for Hand {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

fn parse_input(input_file: &str, joker_enable: bool) -> Vec<Hand> {
    let input = fs::read_to_string(input_file)
        .expect("Error opening input");
    let mut hands: Vec<Hand> = Vec::with_capacity(input.matches("\n").count());
    for s in input.trim().split("\n") {
        hands.push(
            Hand::from_str(s, joker_enable)
            );
    }
    hands
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 6440);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 5905);
    }
}
