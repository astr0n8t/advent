use std::collections::HashMap;
use std::fs;

pub fn part1(input_file: &str) -> usize {
    parse_input(input_file).navigate(String::from("AAA"), true).0
}

pub fn part2(input_file: &str) -> usize {
    let map = parse_input(input_file);
    let cycle_length = map.instructions.len();
    let mut nodes: Vec<String> = vec![];

    for (node, _) in &map.node_lookup {
        if node.chars().last() == Some('A') {
            nodes.push(node.clone());
        }
    }

    let mut cycles = 1;
    nodes.iter().for_each(|x| {
        let (_, y) = map.navigate(x.clone(), false);
        cycles *= y;
    });

    cycle_length*cycles
}

#[derive(Debug)]
enum Direction {
    Left,
    Right,
}

#[derive(Debug)]
struct Node {
    left: String,
    right: String,
}

#[derive(Debug)]
struct Maze {
    node_lookup: HashMap<String,Node>,
    instructions: Vec<Direction>,
}

impl Maze {
    fn navigate(&self, start: String, strict: bool) -> (usize, usize) {
        let mut current_node = &start;
        
        let mut num_cycles = 0;
        let mut num_steps = 0;
        while !self.at_end(current_node, strict) {
            for dir in self.instructions.iter() {
                current_node = match dir {
                    Direction::Left => &self.node_lookup[current_node].left,
                    Direction::Right => &self.node_lookup[current_node].right,
                };
                num_steps += 1;
                if strict && self.at_end(current_node, strict) {
                    break;
                }
            }
            num_cycles += 1
        }
        (num_steps, num_cycles)
    }
    fn at_end(&self, current_node: &String, strict: bool) -> bool {
        if strict {
            return current_node == "ZZZ";
        }
        current_node.chars().last() == Some('Z')
    }
}

fn parse_input(input_file: &str) -> Maze {
    let input = fs::read_to_string(input_file)
        .expect("Could not open input file");
    let (directions, nodes) = input.split_once("\n").unwrap();
    let (_, nodes) = nodes.split_once("\n").unwrap();
    let nodes = nodes.trim();
    let mut maze: Maze = Maze {
        node_lookup: HashMap::with_capacity(nodes.matches("\n").count()),
        instructions: Vec::with_capacity(directions.chars().count()),
    };
    for d in directions.chars() {
        match d {
            'L' => maze.instructions.push(Direction::Left),
            'R' => maze.instructions.push(Direction::Right),
            _ => (),
        }
    }

    for line in nodes.split("\n") {
        let node = String::from(&line[0..3]);
        let left = String::from(&line[7..10]);
        let right = String::from(&line[12..15]);
        maze.node_lookup.insert(
            node,
            Node {
                left,
                right,
            },
        );
    }
    maze
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 2);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test2.txt"), 6);
    }
}
