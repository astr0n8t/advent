use std::fs;
use std::collections::VecDeque;
use std::collections::HashMap;

pub fn part1(input_file: &str) -> usize {
    let mut input = parse_input(input_file);
    let mut sum = PulseCount::new();
    for _ in 0..1000 {
        sum.add(&cycle(&mut input));
    }
    sum.product()
}

pub fn part2(input_file: &str) -> usize {
    let mut input = parse_input(input_file);
    let mut total_cycles = 1;
    let mut num_cycles = 0;

    while !input.get("xn").unwrap().get_sent_high() {
        cycle(&mut input);
        num_cycles += 1;
    }
    total_cycles *= num_cycles;
    input = parse_input(input_file);
    num_cycles = 0;

    while !input.get("qn").unwrap().get_sent_high() {
        cycle(&mut input);
        num_cycles += 1;
    }
    total_cycles *= num_cycles;
    input = parse_input(input_file);
    num_cycles = 0;

    while !input.get("xf").unwrap().get_sent_high() {
        cycle(&mut input);
        num_cycles += 1;
    }
    total_cycles *= num_cycles;
    input = parse_input(input_file);
    num_cycles = 0;

    while !input.get("zl").unwrap().get_sent_high() {
        cycle(&mut input);
        num_cycles += 1;
    }
    total_cycles * num_cycles
}

fn cycle(h: &mut HashMap<String,Gate>) -> PulseCount {
    let mut curr = h.get(&"broadcaster".to_string()).unwrap();
    let mut pulses = PulseCount::new();
    let output = Gate::new(GateKind::Output);
    pulses.inc_low();

    let mut queue: VecDeque<(String,String)> = VecDeque::new();
    for output in curr.output.iter() {
        queue.push_back(("broadcaster".to_string(),output.clone()));
    }

    while queue.len() > 0 {
        // Only immutable references to the hashmap here
        let (sender,recipient) = queue.pop_front().unwrap();
        curr = h.get(&recipient).unwrap_or_else(|| &output);
        let (state,count) = curr.get_updated_state(&*h, &sender);
        // Check if either not a state or pulse was low
        if curr.get_type() == GateKind::Output {
            pulses.add(&count);
            continue;
        } else if curr.get_type() != GateKind::State || state != curr.get_state() {
            for s in curr.output.iter() {
                queue.push_back((recipient.clone(),s.clone()));
            }
        }
        if curr.get_type() == GateKind::Nand && state {
            if !curr.get_sent_high() {
                h.entry(recipient.clone()).and_modify(|s| s.set_sent_high());
            }
        }
        // Actually update hashmap here
        h.entry(recipient).and_modify(|s| s.set_state(state));

        pulses.add(&count);
    }
    pulses
}

#[derive(Debug)]
struct PulseCount {
    low: usize,
    high: usize,
}

impl PulseCount {
    fn new() -> Self {
        Self{ low: 0, high: 0 }
    }
    fn inc_high(&mut self) {
        self.high += 1;
    }
    fn inc_low(&mut self) {
        self.low += 1;
    }
    fn add(&mut self, o: &PulseCount) {
        self.low += o.low;
        self.high += o.high;
    }
    fn product(&self) -> usize {
        self.low*self.high
    }
}

#[derive(Copy,Clone,Debug,PartialEq)]
enum GateKind {
    Nand,
    State,
    Broadcast,
    Output,
}

#[derive(Debug)]
struct Gate {
    kind: GateKind,
    output: Vec<String>,
    input: Vec<String>,
    state: bool,
    sent_high: bool,
}

impl Gate {
    fn new(kind: GateKind) -> Self {
        Self {kind, output: vec![], input: vec![], state: false, sent_high: false}
    }
    fn set_state(&mut self, state: bool) {
        self.state = state;
    }
    fn set_sent_high(&mut self) {
        self.sent_high = true;
    }
    fn get_sent_high(&self) -> bool {
        self.sent_high
    }
    fn get_state(&self) -> bool {
        self.state
    }
    fn get_type(&self) -> GateKind {
        self.kind
    }
    fn get_updated_state(&self, h: &HashMap<String,Gate>, sender: &String) -> (bool, PulseCount) {
        let mut pulses = PulseCount::new();
        let mut state: bool = h.get(sender).unwrap().get_state();

        if state {
            pulses.inc_high();
        } else {
            pulses.inc_low();
        }

        match self.kind {
            GateKind::Broadcast => state = true,
            GateKind::Output => (),
            GateKind::State => {
                if state {
                    // If the pulse is high do nothing
                    state = self.state;
                } else {
                    // If the pulse is low flip 
                    state = !self.state;
                }
            },
            GateKind::Nand => {
                let high = self.input.iter().filter(|s|{
                    h.get(*s).unwrap().get_state()
                }).count();

                if high == self.input.len() {
                    state = false;
                } else {
                    state = true;
                }
            },
        }
        (state, pulses)
    }
}

fn parse_input(input_file: &str) -> HashMap<String,Gate> {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");

    let mut out: HashMap<String,Gate> = HashMap::new();

    // First pass create our gates
    for item in input.trim().split("\n") {
        let gate;
        let (mut item, _) = item.split_once(" -> ").unwrap();
        match item.chars().nth(0).unwrap() {
            '%' => {
                gate = Gate::new(GateKind::State);
                item = item.strip_prefix("%").unwrap();
            },
            '&' => {
                gate = Gate::new(GateKind::Nand);
                item = item.strip_prefix("&").unwrap();
            },
            'b' => {
                gate = Gate::new(GateKind::Broadcast);
            }
            _ => unreachable!(),
        }

        out.insert(String::from(item),gate);
    }

    // Second pass link our input and outputs
    for item in input.trim().split("\n") {
        let (mut item, list) = item.split_once(" -> ").unwrap();
        match item.chars().nth(0).unwrap() {
            '%' => {
                item = item.strip_prefix("%").unwrap();
            },
            '&' => {
                item = item.strip_prefix("&").unwrap();
            },
            'b' => (),
            _ => unreachable!(),
        }
        let list: Vec<&str> = list.split(", ").collect();

        let item = String::from(item);

        for entry in list {
            let entry = String::from(entry);
            out.entry(entry.clone()).and_modify(|e| e.input.push(item.clone()));
            out.entry(item.clone()).and_modify(|e| e.output.push(entry));
        }
    }
    out
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 32000000);
        assert_eq!(part1("data/test2.txt"), 11687500);
    }

}
