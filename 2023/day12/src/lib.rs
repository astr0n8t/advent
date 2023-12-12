use std::fs;
use std::str::FromStr;

pub fn part1(input_file: &str) -> usize {
    parse_input(input_file).iter()
        .map(|x| x.solve())
        .sum()
}

pub fn part2(input_file: &str) -> usize {
    parse_input(input_file).iter_mut()
        .map(|x| {
            x.unfold(); 
            x.solve()
        })
        .sum::<usize>()
}

#[derive(Debug)]
struct Row {
    springs: String,
    counts: Vec<usize>,
}
#[derive(Debug)]
struct RowError;

impl Row {
    fn unfold(&mut self) {
        let mut unfolded: String = self.springs.clone() + "?";
        for _ in 0..3 {
            unfolded += &(self.springs.clone() + "?");
        }
        unfolded += &self.springs;
        self.springs = unfolded;

        let copies = self.counts.clone();
        for _ in 0..4 {
            self.counts.append(&mut copies.clone());
        }
    }
    // Referenced this solution heavily https://github.com/mfornet/advent-of-code-2023/blob/main/src/bin/12.rs
    fn solve(&self) -> usize {
        let spring = format!(".{}", self.springs.trim_end_matches('.'));
        let spring = spring.chars().collect::<Vec<char>>();

        let mut dynamic_programming = vec![0; spring.len() + 1];
        dynamic_programming[0] = 1;

        for (i, _) in spring.iter().take_while(|&&c| c != '#').enumerate() {
            dynamic_programming[i + 1] = 1;
        }

        for &count in self.counts.iter() {
            let mut new_dynamic_programming = vec![0; spring.len() + 1];
            let mut chunk = 0;

            for (i, &c) in spring.iter().enumerate() {
                if c != '.' {
                    chunk += 1;
                } else {
                    chunk = 0;
                }

                if c != '#' {
                    new_dynamic_programming[i + 1] += new_dynamic_programming[i];
                }

                if chunk >= count && spring[i - count] != '#' {
                    new_dynamic_programming[i + 1] += dynamic_programming[i - count];
                }
            }

            dynamic_programming = new_dynamic_programming;
        }

        *dynamic_programming.last().unwrap()
    }
}

impl FromStr for Row {
    type Err = RowError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let s = String::from_str(s).unwrap();
        let (springs, counts) = s.split_once(' ').unwrap();
        let mut out = Self{
            springs: springs.to_string(),
            counts: vec![],
        };

        for num in counts.split(",") {
            out.counts.push(num.parse::<usize>().unwrap());
        }
        Ok(out)
    }
}


fn parse_input(input_file: &str) -> Vec<Row> {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    let mut out: Vec<Row> = vec![];

    for line in input.trim().split("\n") {
        out.push(Row::from_str(line).unwrap());
    }
    out
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 21);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 525152);
    }
}
