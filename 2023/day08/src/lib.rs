use std::collections::HashMap;
use std::fs;

pub fn part1(input_file: &str) -> usize {
    parse_input(input_file).navigate()
}

pub fn part2(input_file: &str) -> usize {
    0
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
    current_node: String,
}

impl Maze {
    fn navigate(&mut self) -> usize {
        let mut num_steps = 0;
        for dir in self.instructions.iter().cycle() {
            self.current_node = match dir {
                Direction::Left => self.node_lookup[&self.current_node].left.clone(),
                Direction::Right => self.node_lookup[&self.current_node].right.clone(),
            };
            num_steps += 1;
            if self.current_node == "ZZZ" {
                break;
            }
        }
        num_steps
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
        current_node: String::from("AAA"),
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
        assert_eq!(part2("data/test.txt"), 0);
    }
}
