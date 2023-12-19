use std::fs;
use std::collections::HashMap;
use std::str::FromStr;

pub fn part1(input_file: &str) -> usize {
    let (rules,parts) = parse_input(input_file);
    dbg!(rules);
    dbg!(parts);
    0
}

pub fn part2(input_file: &str) -> usize {
    0
}

#[derive(Debug)]
struct Part {
    x: usize,
    m: usize,
    a: usize,
    s: usize,
}
#[derive(Debug)]
struct PartError;

impl Part {
    fn new() -> Self {
        Self {x: 0, m: 0, a: 0, s: 0}
    }
}

impl FromStr for Part {
    type Err = PartError;

    fn from_str(s: &str) -> Result<Self,Self::Err> {
        let mut out = Self::new();

        for item in s.split(",") {
            let (label, value) = item.split_once("=").unwrap();
            let value = value.parse::<usize>().unwrap();
            match label {
                "x" => out.x = value,
                "m" => out.m = value,
                "a" => out.a = value,
                "s" => out.s = value,
                _ => unreachable!(),
            }
        }
        Ok(out)
    }
    
}

#[derive(Debug)]
enum Operations {
    Greater,
    Less,
    Jump,
    Accept,
    Reject,
}

#[derive(Debug)]
struct Rule {
    part: char,
    value: usize,
    op: Operations,
    dest: String,
}
#[derive(Debug)]
struct RuleError;

impl Rule {
    fn new() -> Self {
        Self {part: '\x00', value: 0, op: Operations::Jump, dest: String::from("")}
    }
}

impl FromStr for Rule {
    type Err = RuleError;
    fn from_str(s: &str) -> Result<Self,Self::Err> {
        let mut out = Self::new();
        let mut s = s;
        let dest: &str;
        if s.find(":").is_some() {
            (s, dest) = s.split_once(":").unwrap();
            out.dest = dest.to_string();
        } 
        if s.find("<").is_some() {
            let (part, value) = s.split_once("<").unwrap();
            out.value = value.parse::<usize>().unwrap();
            out.part = part.chars().nth(0).unwrap();
            out.op = Operations::Less;
        } else if s.find(">").is_some() {
            let (part, value) = s.split_once(">").unwrap();
            out.value = value.parse::<usize>().unwrap();
            out.part = part.chars().nth(0).unwrap();
            out.op = Operations::Greater;
        } else {
            match s {
                "A" => out.op = Operations::Accept,
                "R" => out.op = Operations::Reject,
                _ => out.dest = String::from(s),

            }
        }
        Ok(out)
    }
}

#[derive(Debug)]
struct RuleSet {
    rules: Vec<Rule>,
}
#[derive(Debug)]
struct RuleSetError;

impl RuleSet {
    fn new() -> Self {
        Self {rules: vec![]}
    }
}

impl FromStr for RuleSet {
    type Err = RuleSetError;
    fn from_str(s: &str) -> Result<Self,Self::Err> {
        let mut out = RuleSet::new();
        for rule in s.split(",") {
            out.rules.push(Rule::from_str(rule).unwrap());
        }
        Ok(out)
    }
}

fn parse_input(input_file: &str) -> (HashMap<String,RuleSet>, Vec<Part>) {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");

    let mut rule_map: HashMap<String,RuleSet> = HashMap::new();
    let (rules,parts) = input.trim().split_once("\n\n").unwrap();
    
    for rule in rules.split("\n") {
        let (key,rule_list) = rule.split_once("{").unwrap();
        let rule_list = rule_list.strip_suffix("}").unwrap();
        rule_map.insert(key.to_string(), RuleSet::from_str(rule_list).unwrap());
    }

    let mut part_array: Vec<Part> = vec![];
    for part in parts.split("\n") {
        let part = part.strip_prefix("{").unwrap()
            .strip_suffix("}").unwrap();
        part_array.push(Part::from_str(part).unwrap());
    }

    (rule_map,part_array)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 19114);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 0);
    }
}
