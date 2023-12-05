use std::str::FromStr;
use std::fs;


pub fn part1(input_file: &str) -> u64 {
    let (seeds, map) = parse_input(input_file);
    seeds.iter().map(|x| {
        let mut v = *x;
        for m in map.iter() {
            v = m.translate(v);
        }
        v
        }).min().unwrap().clone()
}

pub fn part2(input_file: &str) -> u32 {
    0
}


#[derive(Debug)]
struct OffsetMap {
    pub offsets: Vec<Offset>, 
}
#[derive(Debug)]
struct OffsetMapError;

impl OffsetMap {
    pub fn new() -> Self {
        Self { offsets: vec![] }
    }
    pub fn translate(self: &OffsetMap, src: u64) -> u64 {
        let mut offset = self.offsets.iter()
            .filter(|x| x.contains(src))
            .peekable();
        if offset.peek().is_some() { 
            return offset.next()
                .unwrap()
                .translate(src);
        }
        src
    }
}
impl FromStr for OffsetMap {
    type Err = OffsetMapError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut map = OffsetMap::new();
        let s = String::from_str(s)
            .unwrap();
        for line in s.split("\n") {
            map.offsets.push(
                Offset::from_str(line)
                .unwrap()
                );
        }
        Ok(map)
    }
}

#[derive(Debug)]
struct Offset {
    pub destination: u64,
    pub source: u64,
    pub range: u64,
}
#[derive(Debug)]
struct OffsetError;

impl Offset {
    pub fn new() -> Self {
        Self { destination: 0, source: 0, range: 0 }
    }
    pub fn contains(self: &Offset, src: u64) -> bool {
        src >= self.source && src < self.source + self.range
    }
    pub fn translate(self: &Offset, src: u64) -> u64 {
        (src - self.source) + self.destination
    }
}

impl FromStr for Offset {
    type Err = OffsetError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let s = String::from_str(s)
            .unwrap();
        let mut s = s.trim()
            .split_whitespace();
        let mut offset = Offset::new();
        offset.destination = s.next()
            .unwrap().parse::<u64>().unwrap();
        offset.source = s.next()
            .unwrap().parse::<u64>().unwrap();
        offset.range = s.next()
            .unwrap().parse::<u64>().unwrap();
        Ok(offset)
    }
}

fn parse_input(input_file: &str) -> (Vec<u64>, Vec<OffsetMap>) {
    let mut map: Vec<OffsetMap> = vec![];
    let mut seeds: Vec<u64> = vec![];

    let input = fs::read_to_string(input_file)
        .expect("Something went wrong reading the file");
    let (top, rest) = input.split_once("\n").unwrap();
    let top = top.strip_prefix("seeds: ").unwrap();

    for seed in top.split_whitespace() {
        seeds.push(
            seed.parse::<u64>().unwrap()
            );
    }

    for block in rest.trim().split("\n\n") {
        let (_, block) = block.split_once("\n").unwrap();
        map.push(
            OffsetMap::from_str(block)
                .unwrap()
            );
    }
    (seeds, map)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 35);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 0);
    }
}
