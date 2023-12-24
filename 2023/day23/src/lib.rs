use std::fs;
use std::rc::Rc;
use rustc_hash::FxHashSet;
use pathfinding::matrix::Matrix;

pub fn part1(input_file: &str) -> usize {
    let maze = Rc::new(parse_input(input_file));
    let start = Pos::new(1,0,Rc::clone(&maze));
    let end = Pos::new((maze.columns as isize) - 2, (maze.rows as isize) - 1, Rc::clone(&maze));
    find_longest_path(start, end, true)
}

// Brute force works eventually so why not...
//⠀⠀⠀⠀⢀⣤⡀⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
//⠀⠀⠀⠀⣿⠉⢻⠟⢹⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
//⠀⠀⠀⢀⣿⡄⠀⠀⣼⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣤⣄⣠⣤⣄⠀⠀⠀⠀
//⠀⠀⣰⡿⠋⠀⣀⣀⠈⣿⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢻⣇⠘⠋⠀⣿⠇⠀⠀⠀
//⠀⣠⡟⠀⢀⣾⠟⠻⠿⠿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣀⣀⣀⡀⠀⠀⣾⠋⢀⣀⠈⠻⢶⣄⠀⠀
//⢠⣿⠁⣰⡿⠁⠀⣀⣤⣶⣶⡶⢶⣤⣄⡀⢀⣠⠴⠚⠉⠉⠉⠉⠉⠙⢶⡄⠛⠒⠛⠙⢳⣦⡀⠹⣆⠀
//⢸⡇⢠⣿⣠⣴⣿⡟⢉⣠⠤⠶⠶⠾⠯⣿⣿⣧⣀⣤⣶⣾⣿⡿⠿⠛⠋⢙⣛⡛⠳⣄⡀⠙⣷⡀⢹⡆
//⢸⠀⢸⣿⣿⣿⣿⠞⠉⠀⠀⠀⠀⣀⣤⣤⠬⠉⠛⠻⠿⠟⠉⢀⣠⢞⣭⣤⣤⣍⠙⠺⢷⡀⢸⡇⠀⣿
//⢸⠀⢸⣿⣿⡟⠀⠀⠀⢀⣠⠞⣫⢗⣫⢽⣶⣤⣀⠉⠛⣶⠖⠛⠀⣾⡷⣾⠋⣻⡆⠀⠀⡇⣼⠇⠀⣿
//⢸⠀⠀⣿⣿⡇⢠⡤⠔⣋⡤⠞⠁⢸⣷⣾⣯⣹⣿⡆⢀⣏⠀⠈⠈⣿⣷⣼⣿⠿⠷⣴⡞⠀⣿⠀⠀⣿
//⢸⠀⠀⢿⣿⡇⠀⠀⠘⠻⠤⣀⡀⠸⣿⣯⣿⣿⡿⠷⠚⠉⠛⠛⠛⠛⠉⠉⠀⣠⡾⠛⣦⢸⡏⠀⠀⣿
//⢸⠀⠀⢸⣿⡇⠀⣠⠶⠶⠶⠶⠿⣿⣭⣭⣁⣀⣠⣤⣤⣤⣤⣤⣤⡶⠶⠛⠋⢁⣀⣴⠟⣽⠇⠀⠀⣿
//⢸⠀⠀⢸⣿⡇⢾⣅⠀⠀⠶⠶⢦⣤⣤⣀⣉⣉⣉⣉⣁⣡⣤⣤⣴⡶⠶⠶⠚⠉⢉⡿⣠⠟⠀⠀⣰⡟
//⢸⡀⠀⠀⢿⣇⠀⠈⠛⠳⠶⠤⠤⢤⣀⣉⣉⣉⣉⣉⣉⣁⣀⣠⣤⡤⠤⠤⠶⠞⢻⡟⠃⠀⠀⣰⠟⠀
//⢸⣧⠀⠀⠘⣿⣦⣄⡀⠀⠀⠀⠀⠀⠀⠉⠉⠉⠉⠉⠉⠁⠀⠀⠀⠀⠀⣠⣤⣶⣿⣧⣀⣴⠟⠃⠀⠀
// ⢻⣆⠀⠀⠈⢻⣿⣿⣷⣶⣤⣄⣀⣀⣀⣠⣤⣶⣶⣶⣶⣶⣶⣶⣿⣿⣿⣿⣿⣿⣟⡉⠀⠀⠀⠀⠀
// ⠀⢻⣦⡄⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⠀⠀⠀⠀
// ⠀⢀⣿⣿⣿⣶⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡧⠀
pub fn part2(input_file: &str) -> usize {
    let maze = Rc::new(parse_input(input_file));
    let start = Pos::new(1,0,Rc::clone(&maze));
    let end = Pos::new((maze.columns as isize) - 2, (maze.rows as isize) - 1, Rc::clone(&maze));
    find_longest_path(start, end, false)
}

fn dfs(curr: &Pos, hist: &mut FxHashSet<(isize,isize)>, cost: usize, end: &Pos, costs: &mut Vec<usize>, slopes: bool) {
    if curr == end {
        costs.push(cost);
        return;
    } else if hist.contains(&curr.as_coords()) {
        return;
    }

    hist.insert(curr.as_coords());

    for next in curr.successors(slopes) {
        if !hist.contains(&next.as_coords()) {
            dfs(&next, hist, cost+1, end, costs, slopes);
        }
    }

    hist.remove(&curr.as_coords());
}

fn find_longest_path(start: Pos, end: Pos, slopes: bool) -> usize {
    let mut costs = Vec::new();
    let mut hist = FxHashSet::default();
    dfs(&start, &mut hist, 0, &end, &mut costs, slopes);
    *costs.iter().max().unwrap()
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
    fn as_coords(&self) -> (isize,isize) {
        (self.x,self.y)
    }
    fn successors(&self, slopes: bool) -> Vec<Pos> {
        let mut next: Vec<Pos> = vec![];
        match self.north(slopes) {
            Some(x) => next.push(x),
            _ => (),
        }
        match self.south(slopes) {
            Some(x) => next.push(x),
            _ => (),
        }
        match self.east(slopes) {
            Some(x) => next.push(x),
            _ => (),
        }
        match self.west(slopes) {
            Some(x) => next.push(x),
            _ => (),
        }

        next
    }
    fn north(&self, slopes: bool) -> Option<Self> {
        match self.m.get(((self.y - 1) as usize, self.x as usize)) {
            Some(Piece::Path) => (),
            Some(Piece::North) => (),
            Some(x)  => {
                match x {
                    Piece::Rock => return None,
                    _ => if slopes{ return None; },
                }
            }
            _ => return None,
        }
        Some(Self::new(self.x,self.y-1,Rc::clone(&self.m)))
    }
    fn south(&self, slopes: bool) -> Option<Self> {
        match self.m.get(((self.y + 1) as usize, self.x as usize)) {
            Some(Piece::Path) => (),
            Some(Piece::South) => (),
            Some(x)  => {
                match x {
                    Piece::Rock => return None,
                    _ => if slopes{ return None; },
                }
            }
            _ => return None,
        }
        Some(Self::new(self.x,self.y+1,Rc::clone(&self.m)))

    }
    fn east(&self, slopes: bool) -> Option<Self> {
        match self.m.get((self.y as usize, (self.x + 1) as usize)) {
            Some(Piece::Path) => (),
            Some(Piece::East) => (),
            Some(x)  => {
                match x {
                    Piece::Rock => return None,
                    _ => if slopes{ return None; },
                }
            }
            _ => return None,
        }
        Some(Self::new(self.x+1,self.y,Rc::clone(&self.m)))

    }
    fn west(&self, slopes: bool) -> Option<Self> {
        match self.m.get((self.y as usize, (self.x - 1) as usize)) {
            Some(Piece::Path) => (),
            Some(Piece::West) => (),
            Some(x)  => {
                match x {
                    Piece::Rock => return None,
                    _ => if slopes{ return None; },
                }
            }
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
        assert_eq!(part2("data/test.txt"), 154);
    }
}

