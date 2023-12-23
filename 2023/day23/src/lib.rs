use std::fs;
use std::rc::Rc;
use std::boxed::Box;
use std::collections::VecDeque;
use std::collections::HashSet;
use pathfinding::matrix::Matrix;

pub fn part1(input_file: &str) -> usize {
    let maze = Rc::new(parse_input(input_file));
    let start = Pos::new(1,0,Rc::clone(&maze));
    let end = Pos::new((maze.columns as isize) - 2, (maze.rows as isize) - 1, Rc::clone(&maze));
    find_longest(start, end)
}

pub fn part2(input_file: &str) -> usize {
    0
}

fn find_longest(s: Pos, e: Pos) -> usize {
    let mut maxes: Vec<usize> = vec![];
    let mut queue: VecDeque<(Pos,usize,Box<HashSet<Pos>>)> = VecDeque::new();
    let history: HashSet<Pos> = HashSet::new();
    queue.push_back((s,0,Box::new(history)));

    while let Some((s,c,mut h)) = queue.pop_front() {
        if s == e {
            maxes.push(c);
            continue;
        }
        h.insert(s.clone());
        for next in s.successors() {
            if !h.contains(&next) {
                queue.push_back((next,c+1,Box::new(*h.clone())));
            }
        }
    }
    *maxes.iter().max().unwrap()
}

#[derive(Copy,Clone,Debug,Eq,Hash,PartialEq)]
enum Piece {
    Path,
    Rock,
    North,
    South,
    East,
    West,
}

#[derive(Clone, Hash)]
struct Pos{
    x: isize,
    y: isize,
    m: Rc<Matrix<Piece>>,
}

impl Pos {
    fn new(x: isize, y: isize, m: Rc<Matrix<Piece>>) -> Self {
        Self { x, y, m }
    }
    fn successors(&self) -> Vec<Pos> {
        let mut next: Vec<Pos> = vec![];
        match self.north() {
            Some(x) => next.push(x),
            _ => (),
        }
        match self.south() {
            Some(x) => next.push(x),
            _ => (),
        }
        match self.east() {
            Some(x) => next.push(x),
            _ => (),
        }
        match self.west() {
            Some(x) => next.push(x),
            _ => (),
        }

        next
    }
    fn north(&self) -> Option<Self> {
        match self.m.get(((self.y - 1) as usize, self.x as usize)) {
            Some(Piece::Path) => (),
            Some(Piece::North) => (),
            _ => return None,
        }
        Some(Self::new(self.x,self.y-1,Rc::clone(&self.m)))
    }
    fn south(&self) -> Option<Self> {
        match self.m.get(((self.y + 1) as usize, self.x as usize)) {
            Some(Piece::Path) => (),
            Some(Piece::South) => (),
            _ => return None,
        }
        Some(Self::new(self.x,self.y+1,Rc::clone(&self.m)))

    }
    fn east(&self) -> Option<Self> {
        match self.m.get((self.y as usize, (self.x + 1) as usize)) {
            Some(Piece::Path) => (),
            Some(Piece::East) => (),
            _ => return None,
        }
        Some(Self::new(self.x+1,self.y,Rc::clone(&self.m)))

    }
    fn west(&self) -> Option<Self> {
        match self.m.get((self.y as usize, (self.x - 1) as usize)) {
            Some(Piece::Path) => (),
            Some(Piece::West) => (),
            _ => return None,
        }
        Some(Self::new(self.x-1,self.y,Rc::clone(&self.m)))
    }
}

impl PartialEq for Pos {
    fn eq(&self, other: &Self) -> bool {
        self.x == other.x && self.y == other.y
    }
}
impl Eq for Pos {}

impl std::fmt::Debug for Pos {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("Pos")
            .field("x", &self.x)
            .field("y", &self.y)
            .finish()
    }
}


fn parse_input(input_file: &str) -> Matrix<Piece> {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    let input = input.trim();
    Matrix::from_rows(
        input.lines()
            .map(|l|{
                l.chars().map(|c|{
                    match c {
                        '.' => Piece::Path,
                        '#' => Piece::Rock,
                        '^' => Piece::North,
                        'v' => Piece::South,
                        '>' => Piece::East,
                        '<' => Piece::West,
                        _ => unreachable!(),
                    }
                })
            })
        ).unwrap()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 94);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 0);
    }
}
