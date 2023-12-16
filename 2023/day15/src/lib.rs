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
    let input = parse_input(input_file);

    let mut box_map: IndexMap<u8,Box> = IndexMap::new();

    for action in input.iter() {
        if action.find("=").is_some() {
            let (label,value) = action.split_once("=").unwrap();
            let value = value.parse::<u8>().unwrap();
            box_map.entry(hash(label)).and_modify(|b| b.insert(label,value)).or_insert(Box::new(label,value));
        } else {
            let (label,_) = action.split_once("-").unwrap();
            box_map.entry(hash(label)).and_modify(|b| b.remove(label));
        }
    }

    box_map.iter()
        .map(|(i,b)|{
            b.sum(*i as usize)
        }).sum()
}

struct Box {
    lenses: IndexMap<String,u8>,
}

impl Box {
    fn new(label: &str, value: u8) -> Self {
        let mut out = Self {
            lenses: IndexMap::new(),
        };
        out.lenses.insert(String::from(label),value);
        out
    }
    fn insert(&mut self, label: &str, value: u8) {
        self.lenses.insert(String::from(label),value);
    }
    fn remove(&mut self, label: &str) {
        self.lenses.shift_remove(&String::from(label));
    }
    fn sum(&self, id: usize) -> usize {
        let mut sum = 0;
        let _: usize = self.lenses.iter().enumerate()
            .map(|(i,(_,v))|{
                sum += (id+1)*(i+1)*(*v as usize);
                0
            }).sum();
        sum
    }
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
        assert_eq!(part2("data/test.txt"), 145);
    }
}
