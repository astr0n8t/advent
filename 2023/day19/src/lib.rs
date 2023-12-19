use std::fs;
use std::collections::HashMap;
use std::str::FromStr;

pub fn part1(input_file: &str) -> usize {
    let (rules,parts) = parse_input(input_file);
    parts.iter()
        .filter(|p| p.accepted(&rules)) 
        .map(|p| p.sum())
        .sum()
}

// I adapted this solution https://github.com/klimesf/advent-of-code/blob/master/src/y2023/day19.rs
// Just a bit tired today
pub fn part2(input_file: &str) -> usize {
    let (rules,_) = parse_input(input_file);

    let mut stack: Vec<((usize, usize), (usize, usize), (usize, usize), (usize, usize), &str, usize)> =
        vec! {((1, 4000), (1, 4000), (1, 4000), (1, 4000), "in", 0)};
    let mut accepted: Vec<((usize, usize), (usize, usize), (usize, usize), (usize, usize))> = vec!();
    while let Some(range) = stack.pop() {
        let (x, m, a, s, wf_key, rule_key) = range;
        if wf_key == "A" {
            accepted.push((x, m, a, s));
            continue;
        } else if wf_key == "R" {
            continue;
        }

        // Invalid bounds check
        if x.0 > x.1 || m.0 > m.1 || a.0 > a.1 || s.0 > s.1 { continue }

        let rules = rules.get(wf_key).unwrap();
        let rule = &rules.rules[rule_key];
        match rule.op {
            Operations::Jump => {
                match &rule.dest[..] {
                    "A" => {
                        accepted.push((x, m, a, s));
                        continue;
                    },
                    "R" => {
                        continue;
                    }
                    _ => {
                        stack.push((x, m, a, s, &rule.dest, 0));
                        continue;
                    }
                }
            }
            Operations::Greater => {
                match rule.part {
                    'x' => {
                        stack.push(((rule.value + 1, x.1), m, a, s, &rule.dest, 0));
                        stack.push(((x.0, rule.value), m, a, s, wf_key, rule_key + 1));
                    }
                    'm' => {
                        stack.push((x, (rule.value + 1, m.1), a, s, &rule.dest, 0));
                        stack.push((x, (m.0, rule.value), a, s, wf_key, rule_key + 1));
                    }
                    'a' => {
                        stack.push((x, m, (rule.value + 1, a.1), s, &rule.dest, 0));
                        stack.push((x, m, (a.0, rule.value), s, wf_key, rule_key + 1));
                    }
                    's' => {
                        stack.push((x, m, a, (rule.value + 1, s.1), &rule.dest, 0));
                        stack.push((x, m, a, (s.0, rule.value), wf_key, rule_key + 1));
                    }
                    _ => unreachable!(),
                }
            }
            Operations::Less => {
                match rule.part {
                    'x' => {
                        stack.push(((x.0, rule.value - 1), m, a, s, &rule.dest, 0));
                        stack.push(((rule.value, x.1), m, a, s, wf_key, rule_key + 1));
                    }
                    'm' => {
                        stack.push((x, (m.0, rule.value - 1), a, s, &rule.dest, 0));
                        stack.push((x, (rule.value, m.1), a, s, wf_key, rule_key + 1));
                    }
                    'a' => {
                        stack.push((x, m, (a.0, rule.value - 1), s, &rule.dest, 0));
                        stack.push((x, m, (rule.value, a.1), s, wf_key, rule_key + 1));
                    }
                    's' => {
                        stack.push((x, m, a, (s.0, rule.value - 1), &rule.dest, 0));
                        stack.push((x, m, a, (rule.value, s.1), wf_key, rule_key + 1));
                    }
                    _ => unreachable!(),
                }
            }
        }
    }

    accepted.iter().map(|(x, m, a, s)| {
        (x.1 - x.0 + 1) * (m.1 - m.0 + 1) * (a.1 - a.0 + 1) * (s.1 - s.0 + 1)
    }).sum()
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
    fn sum(&self) -> usize {
        self.x + self.m + self.a + self.s
    }
    fn accepted(&self, rules: &HashMap<String,RuleSet>) -> bool {
        let mut curr = String::from("in");
        let a = String::from("A");
        let r = String::from("R");
        while curr != a && curr != r {
            curr = rules.get(&curr).unwrap().process(self);
        }
        curr == "A"
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

#[derive(Copy,Clone,Debug)]
enum Operations {
    Greater,
    Less,
    Jump,
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
    fn matches(&self, p: &Part) -> bool {
        match self.op {
            Operations::Greater => {
                match self.part  {
                    'x' => p.x > self.value,
                    'm' => p.m > self.value,
                    'a' => p.a > self.value,
                    's' => p.s > self.value,
                    _ => unreachable!(),
                }
            },
            Operations::Less => {
                match self.part  {
                    'x' => p.x < self.value,
                    'm' => p.m < self.value,
                    'a' => p.a < self.value,
                    's' => p.s < self.value,
                    _ => unreachable!(),
                }
            },
            _ => true
        }
    }
    fn jump(&self) -> String {
        self.dest.clone()
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
            out.dest = String::from(s);
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
    fn process(&self, p: &Part) -> String {
        for r in self.rules.iter() {
            if r.matches(p) {
                return r.jump();
            }
        }
        unreachable!()
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
        assert_eq!(part2("data/test.txt"), 167409079868000);
    }
}
