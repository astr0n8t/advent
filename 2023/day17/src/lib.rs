use std::fs;
use std::rc::Rc;
use pathfinding::directed::astar;

pub fn part1(input_file: &str) -> usize {
    let mut answer = 0;
    let map: Rc<Vec<Vec<u8>>> = Rc::new(parse_input(input_file));
    let start = Pos::new(0,0,Directions::East,0,Rc::clone(&map),0,3);
    let end = Pos::new((map[0].len() - 1) as i32,(map.len() - 1) as i32,Directions::South,0,Rc::clone(&map),0,3);
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
    let mut answer = 0;
    let map: Rc<Vec<Vec<u8>>> = Rc::new(parse_input(input_file));
    let start = Pos::new(0,0,Directions::East,4,Rc::clone(&map),4,10);
    let end = Pos::new((map[0].len() - 1) as i32,(map.len() - 1) as i32,Directions::South,0,Rc::clone(&map),4,10);
    let result = astar::astar(&start, |p| p.successors(), |p| p.distance(&end), |p| *p == end && p.c >= p.min);
    match result {
        Some(x) => {
            let (_, c) = x;
            answer = c;
        },
        _ => (),
    }

    answer
}

#[derive(Clone, Copy, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
enum Directions {
    North,
    South,
    East,
    West,
}

#[derive(Clone, Debug, Hash, Ord, PartialOrd)]
struct Pos {
    x: i32,
    y: i32,
    d: Directions,
    c: u8,
    m: Rc<Vec<Vec<u8>>>,
    min: u8,
    max: u8,
}

impl Pos {
    fn new(x: i32, y: i32, d: Directions, c: u8, m: Rc<Vec<Vec<u8>>>, min: u8, max: u8) -> Self {
        Self {x, y, d, c, m, min, max}
    }
    fn distance(&self, other: &Pos) -> usize {
        (self.x.abs_diff(other.x) + self.y.abs_diff(other.y)) as usize
    }
    fn successors(&self) -> Vec<(Pos, usize)> {
        let mut out: Vec<(Pos, usize)> = vec![];
        match self.d {
            Directions::North => {
                match (self.c >= self.min, self.c < self.max) {
                    (_,false) => (),
                    (true,true) => if self.y - 1 > -1 {out.push(self.north())},
                    (false,true) => {
                        if self.y - 1 > -1 {
                            out.push(self.north()); 
                            return out;
                        } else {
                            return out;
                        }
                    },
                };
                if self.x - 1 > -1 {
                    out.push(self.west());
                }
                if self.x + 1 < self.m[0].len() as i32 {
                    out.push(self.east());
                }
            },
            Directions::South => {
                match (self.c >= self.min, self.c < self.max) {
                    (_,false) => (),
                    (true,true) => if self.y + 1 < self.m.len() as i32 {out.push(self.south())},
                    (false,true) => {
                        if self.y + 1 < self.m.len() as i32 {
                            out.push(self.south()); 
                            return out;
                        } else {
                            return out;
                        }
                    },
                };
                if self.x - 1 > -1 {
                    out.push(self.west());
                }
                if self.x + 1 < self.m[0].len() as i32 {
                    out.push(self.east());
                }
            },
            Directions::East => {
                match (self.c >= self.min, self.c < self.max) {
                    (_,false) => (),
                    (true,true) => if self.x + 1 < self.m[0].len() as i32 {out.push(self.east())},
                    (false,true) => {
                        if self.x + 1 < self.m[0].len() as i32 {
                            out.push(self.east()); 
                            return out;
                        } else {
                            return out;
                        }
                    },
                };
                if self.y + 1 < self.m.len() as i32 {
                    out.push(self.south());
                }
                if self.y - 1 > -1 {
                    out.push(self.north());
                }
            },
            Directions::West => {
                match (self.c >= self.min, self.c < self.max) {
                    (_,false) => (),
                    (true,true) => if self.x - 1 > -1 {out.push(self.west())},
                    (false,true) => {
                        if self.x - 1 > -1 {
                            out.push(self.west()); 
                            return out;
                        } else {
                            return out;
                        }
                    },
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
            d: Directions::North,
            c: if self.d == Directions::North {self.c + 1} else {1},
            m: Rc::clone(&self.m),
            min: self.min,
            max: self.max,
        }, self.m[(self.y-1) as usize][self.x as usize] as usize)
    }
    fn south(&self) -> (Self, usize) {
        (Self {
            x: self.x, 
            y: self.y + 1, 
            d: Directions::South,
            c: if self.d == Directions::South {self.c + 1} else {1},
            m: Rc::clone(&self.m),
            min: self.min,
            max: self.max,
        }, self.m[(self.y+1) as usize][self.x as usize] as usize)
    }
    fn east(&self) -> (Self, usize) {
        (Self {
            x: self.x + 1, 
            y: self.y, 
            d: Directions::East,
            c: if self.d == Directions::East {self.c + 1} else {1},
            m: Rc::clone(&self.m),
            min: self.min,
            max: self.max,
        }, self.m[self.y as usize][(self.x+1) as usize] as usize)
    }
    fn west(&self) -> (Self, usize) {
        (Self {
            x: self.x - 1, 
            y: self.y, 
            d: Directions::West,
            c: if self.d == Directions::West {self.c + 1} else {1},
            m: Rc::clone(&self.m),
            min: self.min,
            max: self.max,
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
        assert_eq!(part2("data/test.txt"), 94);
        assert_eq!(part2("data/test2.txt"), 71);
    }
}
