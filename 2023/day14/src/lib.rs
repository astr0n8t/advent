use std::fs;
use std::collections::HashMap;
use std::collections::hash_map::DefaultHasher;
use std::hash::{Hash, Hasher};

pub fn part1(input_file: &str) -> usize {
    let mut input = parse_input(input_file);
    cycle(&mut input, 0);
    load(&input)
}

pub fn part2(input_file: &str) -> usize {
    let mut input = parse_input(input_file);
    let mut history: HashMap<u64,usize> = HashMap::new();
    let mut num: usize = 0;
    while !history.contains_key(&calculate_hash(&input)) {
        history.insert(calculate_hash(&input), num);
        cycle(&mut input,0);
        cycle(&mut input,1);
        cycle(&mut input,2);
        cycle(&mut input,3);
        num += 1;
    }

    let num_before_repeat = history.get(&calculate_hash(&input)).unwrap();
    let period = num-num_before_repeat;
    let num_before_repeat = num_before_repeat % period;
    let num_to_go = (period - num_before_repeat) + (1000000000 % period);

    for _ in 0..num_to_go {
        cycle(&mut input,0);
        cycle(&mut input,1);
        cycle(&mut input,2);
        cycle(&mut input,3);
    }

    load(&input)
}

fn calculate_hash<T: Hash>(t: &T) -> u64 {
    let mut s = DefaultHasher::new();
    t.hash(&mut s);
    s.finish()
}


fn load(rocks: &Vec<Vec<char>>) -> usize {
    let mut sum: usize = 0;
    for x in 0..rocks[0].len() {
        for y in 0..rocks.len() {
            match rocks[y][x] {
                'O' => {
                    sum += rocks.len() - y;
                },
                _ => (),
            }
        }
    }
    sum
}

fn cycle(rocks: &mut Vec<Vec<char>>, dir: u8) {
    match dir {
        0 => {
            for x in 0..rocks[0].len() {
                for mut y in 0..rocks.len() {
                    match rocks[y][x] {
                        'O' => {
                            while y != 0 && rocks[y-1][x] == '.' {
                                rocks[y-1][x] = rocks[y][x];
                                rocks[y][x] = '.';
                                y -= 1;
                            }
                        },
                        _ => (),
                    }

                }
            }
        },
        1 => {
            for y in 0..rocks.len() {
                for mut x in 0..rocks[0].len() {
                    match rocks[y][x] {
                        'O' => {
                            while x != 0 && rocks[y][x-1] == '.' {
                                rocks[y][x-1] = rocks[y][x];
                                rocks[y][x] = '.';
                                x -= 1;
                            }
                        },
                        _ => (),
                    }

                }
            }
        },
        2 => {
            for x in 0..rocks[0].len() {
                for mut y in (0..rocks.len()).rev() {
                    match rocks[y][x] {
                        'O' => {
                            while y != rocks.len()-1 && rocks[y+1][x] == '.' {
                                rocks[y+1][x] = rocks[y][x];
                                rocks[y][x] = '.';
                                y += 1;
                            }
                        },
                        _ => (),
                    }

                }
            }
        },
        3 => {
            for y in 0..rocks.len() {
                for mut x in (0..rocks[0].len()).rev() {
                    match rocks[y][x] {
                        'O' => {
                            while x != rocks[0].len()-1 && rocks[y][x+1] == '.' {
                                rocks[y][x+1] = rocks[y][x];
                                rocks[y][x] = '.';
                                x += 1;
                            }
                        },
                        _ => (),
                    }

                }
            }
        },
        _ => ()
    }
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
        assert_eq!(part2("data/test.txt"), 64);
    }
}
