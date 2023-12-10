use std::fs;
use std::str::FromStr;
use std::boxed::Box;

pub fn part1(input_file: &str) -> usize {
    parse_input(input_file).navigate()
}

pub fn part2(input_file: &str) -> usize {
    0
}

#[derive(Copy,Clone,Debug,PartialEq)]
enum PipeKinds {
    Vertical,
    Horizontal,
    NorthEast,
    NorthWest,
    SouthWest,
    SouthEast,
    Ground,
    Start,
}

#[derive(Copy,Clone,Debug)]
struct Pipe {
    kind: PipeKinds,
    distance: usize,
}

impl Pipe {
    fn new() -> Self {
        Pipe {
            kind: PipeKinds::Ground,
            distance: 0,
        }
    }
    fn from_char(c: char) -> Self {
        let mut out = Pipe::new();
        out.kind = match c {
            '|' => PipeKinds::Vertical,
            '-' => PipeKinds::Horizontal,
            'L' => PipeKinds::NorthEast,
            'J' => PipeKinds::NorthWest,
            '7' => PipeKinds::SouthWest,
            'F' => PipeKinds::SouthEast,
            'S' => PipeKinds::Start,
            _ => PipeKinds::Ground,
        };
        out
    }
    fn is_valid(&self, dir: Directions) -> bool {
        match (dir, self.kind) {
            (Directions::Up, PipeKinds::Vertical) => true,
            (Directions::Up, PipeKinds::SouthEast) => true,
            (Directions::Up, PipeKinds::SouthWest) => true,
            (Directions::Down, PipeKinds::Vertical) => true,
            (Directions::Down, PipeKinds::NorthEast) => true,
            (Directions::Down, PipeKinds::NorthWest) => true,
            (Directions::Left, PipeKinds::Horizontal) => true,
            (Directions::Left, PipeKinds::NorthEast) => true,
            (Directions::Left, PipeKinds::SouthEast) => true,
            (Directions::Right, PipeKinds::Horizontal) => true,
            (Directions::Right, PipeKinds::NorthWest) => true,
            (Directions::Right, PipeKinds::SouthWest) => true,
            (_, PipeKinds::Start) => true,
            _ => false,
        }
    }
}

#[derive(Copy,Clone,Debug,PartialEq)]
enum Directions {
    Up,
    Down,
    Left,
    Right,
}

#[derive(Copy,Clone,Debug)]
struct Position {
    x: usize,
    y: usize,
    dir: Directions,
}


#[derive(Debug)]
struct Maze {
    maze: Vec<Vec<Pipe>>,
    height: usize,
    width: usize,
    start_pos: Position,
    current_pos_f: Position,
    current_pos_b: Position,
}
#[derive(Debug)]
struct MazeError;

impl Maze {
    fn new() -> Self {
        let blank = Position { x: 0, y: 0, dir: Directions::Right };
        Self {
            maze: vec![],
            height: 0,
            width: 0,
            start_pos: blank,
            current_pos_f: blank,
            current_pos_b: blank,
        }
    }

    fn get(&self, pos: Position) -> Result<Pipe, Box<dyn std::error::Error>> {
        if pos.x >= self.width && pos.y >= self.height {
            Err("Array indexes out of bounds")?;
        }
        if self.maze[pos.y][pos.x].kind == PipeKinds::Ground {
            Err("Cannot enter ground")?;
        }
        if !self.maze[pos.y][pos.x].is_valid(pos.dir) {
            Err("Cannot enter pipe from this direction")?;
        }
        Ok(self.maze[pos.y][pos.x])
    }

    fn get_next(&self, pos: Position) -> Result<(Pipe, Position), Box<dyn std::error::Error>> {
        let curr = self.get(pos)?;
        let mut new_pos = pos;
        new_pos.dir = match (pos.dir, curr.kind) {
            (Directions::Down, PipeKinds::NorthEast) => Directions::Right,
            (Directions::Left, PipeKinds::NorthEast) => Directions::Up,
            (Directions::Down, PipeKinds::NorthWest) => Directions::Left,
            (Directions::Right, PipeKinds::NorthWest) => Directions::Up,
            (Directions::Up,   PipeKinds::SouthEast) => Directions::Right,
            (Directions::Left, PipeKinds::SouthEast) => Directions::Down,
            (Directions::Up,   PipeKinds::SouthWest) => Directions::Left,
            (Directions::Right, PipeKinds::SouthWest) => Directions::Down,
            _ => new_pos.dir,
        };

        new_pos.x = match new_pos.dir {
            Directions::Right => pos.x.checked_add(1).ok_or_else(|| "Cannot add with overflow")?,
            Directions::Left => pos.x.checked_sub(1).ok_or_else(|| "Cannot subtract with underflow")?,
            _ => pos.x,
        };

        new_pos.y = match new_pos.dir {
            Directions::Up => pos.y.checked_sub(1).ok_or_else(|| "Cannot subtract with underflow")?,
            Directions::Down => pos.y.checked_add(1).ok_or_else(|| "Cannot add with overflow")?,
            _ => pos.y,
        };

        let curr = self.get(new_pos)?;

        Ok((curr, new_pos))
    }

    fn update_distance(&mut self, pos: Position, d: usize) {
        self.maze[pos.y][pos.x].distance = d;
    }

    fn navigate(&mut self) -> usize {
        loop {
            if let Ok((next, next_pos)) = self.get_next(self.current_pos_f) {
                if next.distance == 0 {
                    self.update_distance(next_pos, self.get(self.current_pos_f).unwrap().distance+1);
                    self.current_pos_f = next_pos;
                } else {
                    return next.distance;
                }
            }
            if let Ok((next, next_pos)) = self.get_next(self.current_pos_b) {
                if next.distance == 0 {
                    self.update_distance(next_pos, self.get(self.current_pos_b).unwrap().distance+1);
                    self.current_pos_b = next_pos;
                } else {
                    return next.distance;
                }
            }
        }

    }
}

impl FromStr for Maze {
    type Err = MazeError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let s = String::from_str(&s).unwrap();
        let mut out = Maze::new();
        out.height = s.matches("\n").count();
        out.width = s.split_once("\n").unwrap().0.len();
        out.maze = Vec::with_capacity(out.height);

        let mut s = s.trim().split("\n");
        for y in 0..out.height {
            out.maze.push(Vec::with_capacity(out.width));

            let line = s.next().unwrap().trim();

            let mut x = 0;
            for c in line.chars() {
                out.maze[y].push(Pipe::from_char(c));

                if out.maze[y][x].kind == PipeKinds::Start {
                    out.start_pos = Position {
                        x,
                        y,
                        dir: Directions::Down,
                    };
                }
                x += 1;
            }
        }

        out.current_pos_f = out.start_pos;
        for dir in [Directions::Up, Directions::Down, Directions::Left, Directions::Right].iter() {
            out.current_pos_f.dir = *dir;
            if let Ok((next, _)) = out.get_next(out.current_pos_f) {
                if next.kind != PipeKinds::Ground {
                    break;
                }
            }
        }
        out.current_pos_b = out.start_pos;
        for dir in [Directions::Up, Directions::Down, Directions::Left, Directions::Right].iter()
            .filter(|x| *x != &out.current_pos_f.dir) {
            out.current_pos_b.dir = *dir;
            if let Ok((next, _)) = out.get_next(out.current_pos_b) {
                if next.kind != PipeKinds::Ground {
                    break;
                }
            }
        }
         
        Ok(out)
    }
}


fn parse_input(input_file: &str) -> Maze {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    Maze::from_str(&input).unwrap()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 8);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 0);
    }
}
