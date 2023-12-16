use std::fs;
use indexmap::IndexMap;

pub fn part1(input_file: &str) -> usize {
    parse_input(input_file)
        .iter()
        .map(|s|{
            hash(s) as usize
        })
        .sum()
}

pub fn part2(input_file: &str) -> usize {
    0
}

fn hash(s: &str) -> u8 {
    let mut sum: usize = 0;
    let _: usize = s.chars()
        .map(|c|{
            sum = ((sum + c as usize) * 17) % 256;
            0
        }).sum();
    sum as u8
}


fn parse_input(input_file: &str) -> Vec<String> {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    input.trim()
        .split(",")
        .map(|s| String::from(s))
        .collect()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 1320);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 0);
    }
}
