use std::fs;
use std::rc::Rc;
use pathfinding::directed::astar;

pub fn part1(input_file: &str) -> usize {
    let mut answer = 0;
    let map: Rc<Vec<Vec<u8>>> = Rc::new(parse_input(input_file));
    let start = Pos::new(0,0,Directions::East as i32,0,Rc::clone(&map));
    let end = Pos::new((map[0].len() - 1) as i32,(map.len() - 1) as i32,Directions::South as i32,0,Rc::clone(&map));
    let result = astar::astar(&start, |p| p.successors(), |p| p.distance(&end), |p| *p == end);
    match result {
        Some(x) => {
            let (_, c) = x;
            answer = c;
        },
        _ => (),
    }

    answer
}

pub fn part2(input_file: &str) -> usize {
    0
}

#[derive(Clone, Copy, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
enum Directions {
    North = 3,
    South = 5,
    East = 7,
    West = 11,
}

impl Directions {
    fn from(x: i32) -> Self {
        for dir in [Directions::North, Directions::South, Directions::East, Directions::West].iter() {
            if x % (*dir as i32) == 0 {
                return *dir;
            }
        }
        Directions::North
    }
}

#[derive(Clone, Debug, Hash, Ord, PartialOrd)]
struct Pos {
    x: i32,
    y: i32,
    d: i32,
    c: u8,
    m: Rc<Vec<Vec<u8>>>,
}

impl Pos {
    fn new(x: i32, y: i32, d: i32, c: u8, m: Rc<Vec<Vec<u8>>>) -> Self {
        Self {x, y, d, c, m}
    }
    fn distance(&self, other: &Pos) -> usize {
        (self.x.abs_diff(other.x) + self.y.abs_diff(other.y)) as usize
    }
    fn successors(&self) -> Vec<(Pos, usize)> {
        let mut out: Vec<(Pos, usize)> = vec![];
        match Directions::from(self.d) {
            Directions::North => {
                match self.c {
                    3 => (),
                    _ => if self.y - 1 > -1 {out.push(self.north())},
                };
                if self.x - 1 > -1 {
                    out.push(self.west());
                }
                if self.x + 1 < self.m[0].len() as i32 {
                    out.push(self.east());
                }
            },
            Directions::South => {
                match self.c {
                    3 => (),
                    _ => if self.y + 1 < self.m.len() as i32 {out.push(self.south())},
                };
                if self.x - 1 > -1 {
                    out.push(self.west());
                }
                if self.x + 1 < self.m[0].len() as i32 {
                    out.push(self.east());
                }
            },
            Directions::East => {
                match self.c {
                    3 => (),
                    _ => if self.x + 1 < self.m[0].len() as i32 {out.push(self.east())},
                };
                if self.y + 1 < self.m.len() as i32 {
                    out.push(self.south());
                }
                if self.y - 1 > -1 {
                    out.push(self.north());
                }
            },
            Directions::West => {
                match self.c {
                    3 => (),
                    _ => if self.x - 1 > -1 {out.push(self.west())},
                };
                if self.y + 1 < self.m.len() as i32 {
                    out.push(self.south());
                }
                if self.y - 1 > -1 {
                    out.push(self.north());
                }
            },
        }
        out
    }
    fn north(&self) -> (Self, usize) {
        (Self {
            x: self.x, 
            y: self.y - 1, 
            d: Directions::North as i32,
            c: if self.d == Directions::North as i32 {self.c + 1} else {1},
            m: Rc::clone(&self.m),
        }, self.m[(self.y-1) as usize][self.x as usize] as usize)
    }
    fn south(&self) -> (Self, usize) {
        (Self {
            x: self.x, 
            y: self.y + 1, 
            d: Directions::South as i32,
            c: if self.d == Directions::South as i32 {self.c + 1} else {1},
            m: Rc::clone(&self.m),
        }, self.m[(self.y+1) as usize][self.x as usize] as usize)
    }
    fn east(&self) -> (Self, usize) {
        (Self {
            x: self.x + 1, 
            y: self.y, 
            d: Directions::East as i32,
            c: if self.d == Directions::East as i32 {self.c + 1} else {1},
            m: Rc::clone(&self.m),
        }, self.m[self.y as usize][(self.x+1) as usize] as usize)
    }
    fn west(&self) -> (Self, usize) {
        (Self {
            x: self.x - 1, 
            y: self.y, 
            d: Directions::West as i32,
            c: if self.d == Directions::West as i32 {self.c + 1} else {1},
            m: Rc::clone(&self.m),
        }, self.m[self.y as usize][(self.x-1) as usize] as usize)
    }
}

impl PartialEq for Pos {
    fn eq(&self, other: &Self) -> bool {
        self.x == other.x && self.y == other.y
    }
}
impl Eq for Pos {}


fn parse_input(input_file: &str) -> Vec<Vec<u8>> {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    input.trim().split("\n")
        .map(|l|{
            l.chars()
                .map(|x| x.to_digit(10).unwrap() as u8 )
                .collect::<Vec<u8>>()
        }).collect::<Vec<Vec<u8>>>()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 102);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 0);
    }
}
