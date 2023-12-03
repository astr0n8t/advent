use std::fs;

pub fn part1(input_file: &str) -> u32 {
    let map = parse_input(input_file);
    let map = remove_non_adjacent(&map, &'\0');
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
    let map = parse_input(input_file);
    let map = remove_non_adjacent(&map, &'*');
    find_ratios(&map).iter().map(|x| x.0*x.1).sum()
}

fn is_special(c: &char, f: &char) -> bool {
    !c.is_digit(10) && ((*f == '\0' && *c != '.') || *c == *f)
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

fn find_ratios(map: &Vec<Vec<char>>) -> Vec<(u32,u32)> {
    let mut out: Vec<(u32,u32)> = vec![];

    for (pos_y, line) in map.iter().enumerate() {
        if line.len() == 0 {
            continue;
        }
        for (pos_x, c) in line.iter().enumerate() {
            if *c == '*' {
                let mut snap: Vec<Vec<char>> = vec![];
                snap.push(map[pos_y-1][pos_x-3..=pos_x+3].to_vec());
                snap.push(map[pos_y][pos_x-3..=pos_x+3].to_vec());
                snap.push(map[pos_y+1][pos_x-3..=pos_x+3].to_vec());
                out.push(get_pairs(&snap));
            }
        }
    }

    out
}

fn get_pairs(map: &Vec<Vec<char>>) -> (u32,u32) {
    let mut out = (0,0);
    let mut map_working = map.clone();

    for (pos_y, line) in map.iter().enumerate() {
        for (pos_x, c) in line.iter().enumerate() {
            if pos_y == 1 && pos_x == 3 {
                continue;
            }
            if !c.is_digit(10) {
                map_working[pos_y][pos_x] = '.';
            }
        }
    }
    let map = remove_non_adjacent(&map_working, &'*');

    let map: String = map.into_iter()
        .map(|x| 
             x.into_iter()
             .collect::<String>()
             )
        .collect::<Vec<String>>()
        .join(".");

    let map = clean_map(&map);
    let mut map = map.split(".");

    out.0 = map.next().unwrap().parse::<u32>().unwrap();

    out.1 = match map.next() {
        Some(n) => {
            if n.len() > 0 {n.parse::<u32>().unwrap()} else {0}
        },
        _ => 0
    };

    out
}

fn remove_non_adjacent(map: &Vec<Vec<char>>, f: &char) -> Vec<Vec<char>> {
    let mut map_working = map.clone();
    for (pos_y, line) in map.iter().enumerate() {
        if line.len() == 0 {
            continue;
        }
        let mut num_adjacent = 0;
        for (pos_x, c) in line.iter().enumerate() {
            if c.is_digit(10) {
                if pos_x != 0 {
                    num_adjacent += if is_special(&map[pos_y][pos_x - 1], &f) {1} else {0};
                } 
                if pos_x + 1 < line.len() {

                    num_adjacent += if is_special(&map[pos_y][pos_x + 1], &f) {1} else {0};
                }
                if pos_y != 0 {

                    num_adjacent += if is_special(&map[pos_y - 1][pos_x], &f) {1} else {0};
                }
                if pos_y + 1 < map.len() {

                    num_adjacent += if is_special(&map[pos_y + 1][pos_x], &f) {1} else {0};
                }
                if pos_x != 0 && pos_y != 0 {
                    num_adjacent += if is_special(&map[pos_y - 1][pos_x - 1], &f) {1} else {0};
                } 
                if pos_x + 1 < line.len() && pos_y + 1 < map.len() {

                    num_adjacent += if is_special(&map[pos_y + 1][pos_x + 1], &f) {1} else {0};
                }
                if pos_y != 0 && pos_x + 1 < line.len() {

                    num_adjacent += if is_special(&map[pos_y - 1][pos_x + 1], &f) {1} else {0};
                }
                if pos_y + 1 < map.len() && pos_x != 0 {

                    num_adjacent += if is_special(&map[pos_y + 1][pos_x - 1], &f) {1} else {0};
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
        assert_eq!(part2("data/test.txt"), 467835);
    }
}
