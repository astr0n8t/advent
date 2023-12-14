use std::fs;
use std::collections::HashMap;

pub fn part1(input_file: &str) -> usize {
    let input = parse_input(input_file);

    let mut anchors: HashMap<(usize,usize),usize> = HashMap::new(); 
    for x in 0..input[0].len() {
        let mut curr = (x,0);
        for y in 0..input.len() {
            match input[y][x] {
                '#' => {
                    curr = (x,y+1);
                },
                'O' => {
                    anchors.entry(curr).and_modify(|z| *z += 1).or_insert(1);
                },
                _ => (),
            }
        }
    }

    let mut sum = 0;
    for ((_,y),num) in &anchors {
        for i in 0..*num {
            sum += input.len() - y - i;
        }

    }
    sum
}

pub fn part2(input_file: &str) -> usize {
    0
}


fn parse_input(input_file: &str) -> Vec<Vec<char>> {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    input.trim().split("\n")
        .map(|x| x.chars().collect() )
        .collect::<Vec<Vec<char>>>()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 136);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 0);
    }
}
