use std::fs;

pub fn part1(input_file: &str) -> usize {
    parse_input(input_file).iter().map(|x| {
        x.get_num_winning()
    }).product()
}

pub fn part2(input_file: &str) -> usize {
    parse_input_single(input_file).get_num_winning()
}


#[derive(Debug)]
struct Race {
    pub t: usize,
    pub d: usize,
}

impl Race {
    fn is_winning_charge_time(self: &Race, c: usize) -> bool {
        (self.t-c)*c > self.d
    }
    pub fn get_num_winning(self: &Race) -> usize {
        self.last_winning() - self.first_winning() + 1
    }
    pub fn first_winning(self: &Race) -> usize {
        let mut c = 1;
        while !self.is_winning_charge_time(c) { c += 1 };
        c
    }
    pub fn last_winning(self: &Race) -> usize {
        let mut c = self.t - 1;
        while !self.is_winning_charge_time(c) { c -= 1 };
        c
    }
}


fn parse_input(input_file: &str) -> Vec<Race> {
    let mut races: Vec<Race> = vec![];
    let input = fs::read_to_string(input_file)
        .expect("Something went wrong reading the file");

    let mut input = input.split("\n");

    let times = input.next().unwrap()
        .strip_prefix("Time: ").unwrap().trim();
    let distances = input.next().unwrap()
        .strip_prefix("Distance: ").unwrap().trim();
    let times = times.split_whitespace();
    let mut distances = distances.split_whitespace();
    for t in times {
        races.push(
            Race { 
                t: t.parse::<usize>().unwrap(),
                d: distances.next().unwrap()
                    .parse::<usize>().unwrap(),
            }
          );
    }
    races
}

fn parse_input_single(input_file: &str) -> Race {
    let input = fs::read_to_string(input_file)
        .expect("Something went wrong reading the file");

    let mut input = input.split("\n");

    let t = input.next().unwrap()
        .strip_prefix("Time: ").unwrap()
        .split_whitespace()
        .collect::<String>();
    let d = input.next().unwrap()
        .strip_prefix("Distance: ").unwrap()
        .split_whitespace()
        .collect::<String>();
    Race { 
        t: t.parse::<usize>().unwrap(),
        d: d.parse::<usize>().unwrap(),
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 288);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 71503);
    }
}
