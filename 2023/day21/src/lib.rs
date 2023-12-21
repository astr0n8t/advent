use std::fs;
use std::str::FromStr;
use std::collections::VecDeque;
use std::collections::HashSet;

pub fn part1(input_file: &str) -> usize {
    parse_input(input_file).get_num_plots(64)
}

pub fn part2(input_file: &str) -> usize {
    0
}

#[derive(Copy,Clone,Debug,PartialEq)]
enum Pieces {
    Start,
    Rock,
    Plot,
}

#[derive(Copy,Clone,Debug,Eq,Hash,PartialEq)]
struct Position {
    x: isize,
    y: isize,
}

impl Position {
    fn new() -> Self {
        Self{ x: 0, y: 0 }
    }
    fn north(&self) -> Self {
        Self{ x: self.x, y: self.y - 1 }
    }
    fn south(&self) -> Self {
        Self{ x: self.x, y: self.y + 1 }
    }
    fn east(&self) -> Self {
        Self{ x: self.x + 1, y: self.y }
    }
    fn west(&self) -> Self {
        Self{ x: self.x - 1, y: self.y }
    }
}

#[derive(Debug)]
struct Maze {
    maze: Vec<Vec<Pieces>>,
    start: Position,
}
#[derive(Debug)]
struct MazeError;

impl Maze {
    fn get_num_plots(&self, max: usize) -> usize {
        let mut reachable: HashSet<Position> = HashSet::new();
        let mut history: HashSet<(usize,Position)> = HashSet::new();
        let mut queue: VecDeque<(usize, Position)> = VecDeque::new();
        queue.push_front((0,self.start));

        while let Some((step,pos)) = queue.pop_front() {
            if step == max && !reachable.contains(&pos) {
                reachable.insert(pos);
            } else if step < max {
                for curr in [pos.north(), pos.south(), pos.west(), pos.east()].iter() {
                    if self.check_valid_from_pos(&curr).is_some() && !history.contains(&(step+1,*curr)) {
                        queue.push_back((step+1, *curr));
                        history.insert((step+1,*curr));
                    }
                }
            }
        }
        reachable.len()
    }
    fn check_valid_from_pos(&self, pos: &Position) -> Option<Pieces> {
        match self.get_from_pos(pos) {
            Some(x) => match x {
                Pieces::Rock => None,
                _ => Some(x),
            },
            _ => None,
        }
    }
    fn get_from_pos(&self, pos: &Position) -> Option<Pieces> {
        if pos.x < 0 || pos.y < 0 || 
            pos.x as usize >= self.maze[0].len() || 
            pos.y as usize >= self.maze.len() {
                return None;
        }

        Some(self.maze[pos.y as usize][pos.x as usize])
    }
}

impl FromStr for Maze {
    type Err = MazeError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut maze: Vec<Vec<Pieces>> = vec![];
        let mut start = Position::new();
        let mut pos = Position::new();
        for line in s.split("\n") {
            let mut curr: Vec<Pieces> = vec![];
            for c in line.chars() {
                curr.push(match c {
                    '.' => Pieces::Plot,
                    '#' => Pieces::Rock,
                    'S' => {
                        start = pos;
                        Pieces::Start
                    },
                    _ => continue,
                });
                pos = pos.east();
            }
            pos.x = 0;
            pos = pos.south();
            maze.push(curr);
        }
        Ok(Self{maze,start})
    }
}

fn parse_input(input_file: &str) -> Maze {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    Maze::from_str(&input.trim()).unwrap()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(parse_input("data/test.txt").get_num_plots(6), 16);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 0);
    }
}
