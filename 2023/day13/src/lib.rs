use std::fs;
use std::str::FromStr;

pub fn part1(input_file: &str) -> usize {
    parse_input(input_file).iter().map(|x|{
        x.find_line_of_reflection()
    }).sum()
}

pub fn part2(input_file: &str) -> usize {
    0
}

#[derive(Debug)]
struct Pattern {
    horizontal: Vec<String>,
    vertical: Vec<String>,
}
#[derive(Debug)]
struct PatternErr;

impl Pattern {
    fn new() -> Self {
        Self {
            horizontal: vec![],
            vertical: vec![],
        }
    }

    fn find_line_of_reflection(&self) -> usize {
        for i in 0..(self.vertical.len()-1) {
            if self.vertical[i] == self.vertical[i+1] {
                let mut left: i32 = i as i32;
                let mut right: i32 = (i + 1) as i32;
                while self.vertical[left as usize] == self.vertical[right as usize]{
                    left -= 1;
                    right += 1;
                    if left < 0 || right as usize > self.vertical.len()-1 {
                        return i+1;
                    }
                }
            }
        }
        for i in 0..(self.horizontal.len()-1) {
            if self.horizontal[i] == self.horizontal[i+1] {
                let mut left: i32 = i as i32;
                let mut right: i32 = (i + 1) as i32;
                while self.horizontal[left as usize] == self.horizontal[right as usize]{
                    left -= 1;
                    right += 1;
                    if left < 0 || right as usize > self.horizontal.len()-1 {
                        return (i+1)*100;
                    }
                }
            }
        }
        unreachable!();
    }
}

impl FromStr for Pattern {
    type Err = PatternErr;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut out = Self::new();

        out.horizontal = String::from_str(s).unwrap()
            .split("\n")
            .map(|x| String::from_str(x).unwrap())
            .collect::<Vec<String>>();

        for _ in 0..out.horizontal[0].len() {
            out.vertical.push(String::from_str("").unwrap());
        }

        for x in 0..out.horizontal[0].len() {
            for y in 0..out.horizontal.len() {
                out.vertical[x].push(out.horizontal[y].chars().nth(x).unwrap());
            }
        }
        Ok(out)
    }
}

fn parse_input(input_file: &str) -> Vec<Pattern> {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    let mut out: Vec<Pattern> = vec![];

    for pattern in input.trim().split("\n\n") {
        out.push(
            Pattern::from_str(pattern).unwrap()
            );
    }
    out
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 405);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 0);
    }
}
