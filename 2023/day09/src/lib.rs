use std::str::FromStr;
use std::fs;

pub fn part1(input_file: &str) -> i64 {
    parse_input(input_file).iter_mut().map(|x| x.next()).sum()
}

pub fn part2(input_file: &str) -> i64 {
    parse_input(input_file).iter_mut().map(|x| x.prev()).sum()
}

#[derive(Debug)]
struct Sequence {
    seq: Vec<i64>,
    next_seq: Option<Box<Sequence>>,
}
#[derive(Debug)]
struct SequenceError;

impl Sequence {
    fn new(size: usize) -> Self {
        Self {
            seq: Vec::with_capacity(size),
            next_seq: None,
        }
    }
    fn next(&mut self) -> i64 {
        if self.next_seq.is_none() && !self.pyramid_end() {
            self.calculate_pyramid();
        }

        let mut x = 0;

        if !self.pyramid_end() {
            if let Some(ref mut seq) = self.next_seq {
                x = seq.next();
            }
        }
        self.seq.iter().last().unwrap() + x
    }
    fn prev(&mut self) -> i64 {
        if self.next_seq.is_none() && !self.pyramid_end() {
            self.calculate_pyramid();
        }

        let mut x = 0;

        if !self.pyramid_end() {
            if let Some(ref mut seq) = self.next_seq {
                x = seq.prev();
            }
        }
        self.seq.iter().next().unwrap() - x
    }
    fn calculate_pyramid(&mut self) {
        let mut next_seq = Box::new(Sequence::new(self.seq.len()-1));

        for i in 1..self.seq.len() {
            next_seq.seq.push(self.seq[i]-self.seq[i-1]);
        }
        if !next_seq.pyramid_end() {
            next_seq.calculate_pyramid();
        }
        self.next_seq = Some(next_seq);
    }
    fn pyramid_end(&self) -> bool {
        for i in self.seq.iter() {
            if *i != 0 {
                return false;
            }
        }
        true
    }
}

impl FromStr for Sequence {
    type Err = SequenceError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let s = String::from(s);
        let mut out = Sequence::new(s.matches(" ").count()+1);
        for i in s.trim().split(" ") {
            out.seq.push(i.parse::<i64>().unwrap());
        }
        Ok(out)
    }

}

fn parse_input(input_file: &str) -> Vec<Sequence> {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");

    let mut out: Vec<Sequence> = Vec::with_capacity(input.matches("\n").count());

    for line in input.trim().split("\n") {
        out.push(Sequence::from_str(line).unwrap());
    }
    out
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 114);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 2);
    }
}
