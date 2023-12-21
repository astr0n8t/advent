use std::fs;
use std::str::FromStr;
use std::collections::VecDeque;
use std::collections::HashSet;

pub fn part1(input_file: &str) -> usize {
    parse_input(input_file).get_num_plots(64, false)
}

pub fn part2(input_file: &str) -> usize {
    parse_input(input_file).estimate_num_plots(26501365)
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
    fn modulo(&self, x_mod: usize, y_mod: usize) -> Self {
        let mut x = self.x % x_mod as isize;
        let mut y = self.y % y_mod as isize;
        if x < 0 {
            x += x_mod as isize;
        }
        if y < 0 {
            y += y_mod as isize;
        }
        Self { x: x, y: y }
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
    fn estimate_num_plots(&self, num: usize) -> usize {
        let modulo = self.maze[0].len();
        let rem = num % modulo;
        let y1 = self.get_num_plots(rem, true) as isize;
        let y2 = self.get_num_plots(rem+modulo, true) as isize;
        let y3 = self.get_num_plots(rem+modulo*2, true) as isize;

        let x_2 = (y1 - 2*y2 + y3) as f64 / 2.0;
        let x_1 = (4*y2 - y3 - 3*y1) as f64 / 2.0;
        let c = y1 as f64;
        let mul = (num / modulo) as f64;

        f64::ceil(x_2*mul*mul + x_1*mul + c) as usize
    }
    fn get_num_plots(&self, max: usize, modulo: bool) -> usize {
        let mut reachable: HashSet<Position> = HashSet::new();
        let mut history: HashSet<(usize,Position)> = HashSet::new();
        let mut queue: VecDeque<(usize, Position)> = VecDeque::new();
        queue.push_front((0,self.start));

        while let Some((step,pos)) = queue.pop_front() {
            if step == max && !reachable.contains(&pos) {
                reachable.insert(pos);
            } else if step < max {
                for curr in [pos.north(), pos.south(), pos.west(), pos.east()].iter() {
                    let next_piece = if modulo {
                        self.check_valid_from_pos_modulo(&curr)
                    } else {
                        self.check_valid_from_pos(&curr)
                    };
                    if next_piece.is_some() && !history.contains(&(step+1,*curr)) {
                        queue.push_back((step+1, *curr));
                        history.insert((step+1,*curr));
                    }
                }
            }
        }
        reachable.len()
    }
    fn check_valid_from_pos_modulo(&self, pos: &Position) -> Option<Pieces> {
        match self.get_from_pos(&pos.modulo(self.maze[0].len(), self.maze.len())) {
            Some(x) => match x {
                Pieces::Rock => None,
                _ => Some(x),
            },
            _ => None,
        }
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
        assert_eq!(parse_input("data/test.txt").get_num_plots(6, false), 16);
    }

    #[test]
    fn test2() {
        assert_eq!(parse_input("data/test.txt").get_num_plots(6, true), 16);
        assert_eq!(parse_input("data/test.txt").get_num_plots(10, true), 50);
        assert_eq!(parse_input("data/test.txt").get_num_plots(10, true), 50);
        assert_eq!(parse_input("data/test.txt").get_num_plots(50, true), 1594);
    }
}
