use std::fs;
use std::str::FromStr;
use std::cmp;

pub fn part1(input_file: &str) -> usize {
    let mut input = parse_input(input_file);
    input.sort_by(|a, b| a.bottom().partial_cmp(&b.bottom()).unwrap() );

    for x in 0..input.len() {
        input[x] = input[x].shift(&input);
    }

    let mut count = 0;
    for x in 0..input.len() {
        let mut new_input = input.clone();
        new_input.remove(x);
        if new_input.iter().all(|b| *b == b.shift(&new_input)) {
            count += 1;
        }
    }

    count
}

pub fn part2(input_file: &str) -> usize {
    let mut input = parse_input(input_file);
    input.sort_by(|a, b| a.bottom().partial_cmp(&b.bottom()).unwrap() );

    for x in 0..input.len() {
        input[x] = input[x].shift(&input);
    }

    let mut count = 0;
    for x in 0..input.len() {
        let mut new_input = input.clone();
        new_input.remove(x);
        for y in 0..new_input.len() {
            let tmp = new_input[y].shift(&new_input);
            if tmp != new_input[y] {
                count += 1;
                new_input[y] = tmp;
            }

        }
    }

    count
}

#[derive(Copy,Clone,Hash,Eq,PartialEq,Debug)]
struct Brick {
    x1: usize,
    y1: usize,
    z1: usize,
    x2: usize,
    y2: usize,
    z2: usize,
}
#[derive(Debug)]
struct BrickError;

impl Brick {
    fn new() -> Self {
        Self {x1: 0, y1: 0, z1: 0, x2: 0, y2: 0, z2: 0}
    }
    fn shift(&self, env: &[Brick]) -> Self {
        let mut shifted = *self;

        while !shifted.on_floor() && env.iter().all(|b| !b.resting(&shifted)) {
            shifted.z1 -= 1;
            shifted.z2 -= 1;

        }
        shifted
    }
    fn on_floor(&self) -> bool {
        self.bottom() == 1
    }
    fn resting(&self, other: &Brick) -> bool {
        if self.top() + 1 == other.bottom() && self.overlapping(other) {
            return true;
        }
        false
    }
    fn overlapping(&self, other: &Brick) -> bool {
        self.overlapping_x(other) && self.overlapping_y(other)
    }
    fn overlapping_x(&self, other: &Brick) -> bool {
        if self.east() >= other.west() && self.west() <= other.east() {
            return true;
        } else if other.east() >= self.west() && other.west() <= self.east() {
            return true;
        }
        false
    }
    fn overlapping_y(&self, other: &Brick) -> bool {
        if self.north() >= other.south() && self.south() <= other.north() {
            return true;
        } else if other.north() >= self.south() && other.south() <= self.north() {
            return true;
        }
        false
    }
    fn north(&self) -> usize {
        cmp::max(self.y1,self.y2)
    }
    fn south(&self) -> usize {
        cmp::min(self.y1,self.y2)
    }
    fn east(&self) -> usize {
        cmp::max(self.x1,self.x2)
    }
    fn west(&self) -> usize {
        cmp::min(self.x1,self.x2)
    }
    fn top(&self) -> usize {
        cmp::max(self.z1,self.z2)
    }
    fn bottom(&self) -> usize {
        cmp::min(self.z1,self.z2)
    }
}

impl FromStr for Brick {
    type Err = BrickError;
    fn from_str(s: &str) -> Result<Self,Self::Err> {
        let mut out = Self::new();

        let (first,second) = s.split_once("~").unwrap();
        let mut first = first.split(",");
        let mut second = second.split(",");

        out.x1 = first.next().unwrap().parse().unwrap();
        out.y1 = first.next().unwrap().parse().unwrap();
        out.z1 = first.next().unwrap().parse().unwrap();

        out.x2 = second.next().unwrap().parse().unwrap();
        out.y2 = second.next().unwrap().parse().unwrap();
        out.z2 = second.next().unwrap().parse().unwrap();

        Ok(out)
    }
}


fn parse_input(input_file: &str) -> Vec<Brick> {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    let mut out = vec![];
    for line in input.trim().split("\n") {
        out.push(
            Brick::from_str(&line).unwrap()
            );
    }
    out
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 5);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 7);
    }
}
