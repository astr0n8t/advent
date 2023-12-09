use std::fs;

pub fn part1(input_file: &str) -> usize {
    0
}

pub fn part2(input_file: &str) -> usize {
    0
}


fn parse_input(input_file: &str) -> bool {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    false
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 0);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 0);
    }
}
