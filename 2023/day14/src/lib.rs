#![feature(test)]
extern crate test;
use test::Bencher;

use std::fs;
use std::collections::HashMap;
use std::collections::hash_map::DefaultHasher;
use std::hash::{Hash, Hasher};

pub fn part1(input_file: &str) -> usize {
    let mut input = parse_input(input_file);
    north(&mut input);
    load(&input)
}

pub fn part2(input_file: &str) -> usize {
    let mut input = parse_input(input_file);
    let mut history: HashMap<u64,usize> = HashMap::new();
    let mut num: usize = 0;
    let mut curr = calculate_hash(&input);
    while !history.contains_key(&curr) {
        history.insert(curr, num);
        cycle(&mut input);
        curr = calculate_hash(&input);
        num += 1;
    }

    let num_before_repeat = history.get(&calculate_hash(&input)).unwrap();
    let period = num-num_before_repeat;
    let num_before_repeat = num_before_repeat % period;
    let num_to_go = (period - num_before_repeat) + (1000000000 % period);

    for _ in 0..num_to_go {
        cycle(&mut input);
    }

    load(&input)
}

fn calculate_hash<T: Hash>(t: &T) -> u64 {
    let mut s = DefaultHasher::new();
    t.hash(&mut s);
    s.finish()
}


fn load(rocks: &Vec<Vec<char>>) -> usize {
    let mut sum: usize = 0;
    for x in 0..rocks[0].len() {
        for y in 0..rocks.len() {
            match rocks[y][x] {
                'O' => {
                    sum += rocks.len() - y;
                },
                _ => (),
            }
        }
    }
    sum
}

fn cycle(rocks: &mut Vec<Vec<char>>) {
    north(rocks);
    west(rocks);
    south(rocks);
    east(rocks);
}
fn north(rocks: &mut Vec<Vec<char>>) {
    for x in 0..rocks[0].len() {
        for mut y in 0..rocks.len() {
            if rocks[y][x] == 'O' {
                while y != 0 && rocks[y-1][x] == '.' {
                    rocks[y-1][x] = rocks[y][x];
                    rocks[y][x] = '.';
                    y -= 1;

                }
            }
        }
    }
}
fn west(rocks: &mut Vec<Vec<char>>) {
    for y in 0..rocks.len() {
        for mut x in 0..rocks[0].len() {
            if rocks[y][x] == 'O' {
                while x != 0 && rocks[y][x-1] == '.' {
                    rocks[y][x-1] = rocks[y][x];
                    rocks[y][x] = '.';
                    x -= 1;
                }
            }

        }
    }
}
fn south(rocks: &mut Vec<Vec<char>>) {
    for x in 0..rocks[0].len() {
        for mut y in (0..rocks.len()).rev() {
            if rocks[y][x] == 'O' {
                while y != rocks.len()-1 && rocks[y+1][x] == '.' {
                    rocks[y+1][x] = rocks[y][x];
                    rocks[y][x] = '.';
                    y += 1;
                }
            }

        }
    }
}
fn east(rocks: &mut Vec<Vec<char>>) {
    for y in 0..rocks.len() {
        for mut x in (0..rocks[0].len()).rev() {
            if rocks[y][x] == 'O' {
                while x != rocks[0].len()-1 && rocks[y][x+1] == '.' {
                    rocks[y][x+1] = rocks[y][x];
                    rocks[y][x] = '.';
                    x += 1;
                }
            }

        }
    }
}


fn parse_input(input_file: &str) -> Vec<Vec<char>> {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    input.trim().split("\n")
        .map(|x| x.chars().collect() )
        .collect::<Vec<Vec<char>>>()
}

#[bench]
fn bench_part1(b: &mut Bencher) {
    b.iter(|| {
        part1("data/input.txt");
    });
}
#[bench]
fn bench_part2(b: &mut Bencher) {
    b.iter(|| {
        part2("data/input.txt");
    });
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 136);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 64);
    }
}
