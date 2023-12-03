use std::fs;

pub fn part1(input_file: &str) -> u32 {
    let map = parse_input(input_file);
    let map = remove_non_adjacent(&map);
    let map: String = map.into_iter()
        .map(|x| 
             x.into_iter()
             .collect::<String>()
             )
        .collect::<Vec<String>>()
        .join(".");
    let map = clean_map(&map);
    map.split('.')
        .map(|x| if x.len() > 0 {x.parse::<u32>().unwrap()} else {0})
        .sum()
}

pub fn part2(input_file: &str) -> u32 {
    0
}

fn is_special(c: &char) -> bool {
    !c.is_digit(10) && *c != '.'
}

fn clean_map(s: &str) -> String {
    let mut out = String::from("");
    let mut was = false;
    for c in s.chars() {
        if c.is_numeric() {
            out.push(c);
            was = true;
        } else if was {
            out.push('.');
            was = false;
        }
    }
    out
}

fn remove_non_adjacent(map: &Vec<Vec<char>>) -> Vec<Vec<char>> {
    let mut map_working = map.clone();
    for (pos_y, line) in map.iter().enumerate() {
        if line.len() == 0 {
            continue;
        }
        let mut num_adjacent = 0;
        for (pos_x, c) in line.iter().enumerate() {
            if c.is_digit(10) {
                if pos_x != 0 {
                    num_adjacent += if is_special(&map[pos_y ][pos_x - 1 ]) {1} else {0};
                } 
                if pos_x + 1 < line.len() {

                    num_adjacent += if is_special(&map[pos_y ][pos_x + 1 ]) {1} else {0};
                }
                if pos_y != 0 {

                    num_adjacent += if is_special(&map[pos_y - 1 ][pos_x ]) {1} else {0};
                }
                if pos_y + 1 < map.len() {

                    num_adjacent += if is_special(&map[pos_y + 1 ][pos_x ]) {1} else {0};
                }
                if pos_x != 0 && pos_y != 0 {
                    num_adjacent += if is_special(&map[pos_y - 1 ][pos_x - 1 ]) {1} else {0};
                } 
                if pos_x + 1 < line.len() && pos_y + 1 < map.len() {

                    num_adjacent += if is_special(&map[pos_y + 1 ][pos_x + 1 ]) {1} else {0};
                }
                if pos_y != 0 && pos_x + 1 < line.len() {

                    num_adjacent += if is_special(&map[pos_y - 1 ][pos_x + 1 ]) {1} else {0};
                }
                if pos_y + 1 < map.len() && pos_x != 0 {

                    num_adjacent += if is_special(&map[pos_y + 1 ][pos_x - 1 ]) {1} else {0};
                }

                if pos_x + 1 >= line.len() || !map[pos_y ][pos_x + 1 ].is_digit(10) {
                    if num_adjacent == 0 {
                        let mut tmp_x = pos_x;
                        while map[pos_y ][tmp_x ].is_digit(10) {
                            map_working[pos_y ][tmp_x ] = '.';
                            if tmp_x == 0 {
                                break;
                            }
                            tmp_x -= 1;
                        }
                    }
                }
            } else {
                num_adjacent = 0;
            }
        }
    }
    map_working
}


fn parse_input(input_file: &str) -> Vec<Vec<char>> {
    let input = fs::read_to_string(input_file)
        .expect("Something went wrong reading the file");

    input.trim_end().split("\n").map(|x| x.chars().collect()).collect()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 4361);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 0);
    }
}
