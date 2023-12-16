use std::fs;
use std::str::FromStr;
use std::collections::HashMap;
use std::collections::VecDeque;

pub fn part1(input_file: &str) -> usize {
    parse_input(input_file).navigate()
        .len()
}

pub fn part2(input_file: &str) -> usize {
    0
}

#[derive(Debug)]
enum Pieces {
    Vertical = '|' as isize,
    Horizontal = '-' as isize,
    Forward = '/' as isize,
    Backward = '\\' as isize,
    Space = '.' as isize,
}

impl Pieces {
    fn from(c: char) -> Pieces {
        match c {
            '|' => Pieces::Vertical,
            '-' => Pieces::Horizontal,
            '/' => Pieces::Forward,
            '\\' => Pieces::Backward,
            _ => Pieces::Space,
        }
    }
}

#[derive(Debug)]
enum Directions {
    North = '^' as isize,
    South = 'v' as isize,
    East = '>' as isize,
    West = '<' as isize,
}

#[derive(Debug)]
struct Grid {
    grid: Vec<Vec<char>>,
}
#[derive(Debug)]
struct GridError;


#[derive(Debug)]
struct Beam {
    x: isize,
    y: isize,
    d: Directions,
}

impl Beam {
    fn new() -> Self {
        Self {
            x: 0,
            y: 0,
            d: Directions::East,
        }
    }
    fn in_grid(&self, x_max: usize, y_max: usize) -> bool {
        match self.d {
            Directions::East => self.x < x_max as isize,
            Directions::West => self.x > -1,
            Directions::North => self.y > -1,
            Directions::South => self.y < y_max as isize,
        }
    }
    fn move_one(&mut self) {
        match self.d {
            Directions::East => self.x += 1,
            Directions::West => self.x -= 1,
            Directions::North => self.y -= 1,
            Directions::South => self.y += 1,
        }
    }
    fn forward(&mut self) {
        self.d = match self.d {
            Directions::North => Directions::East,
            Directions::South => Directions::West,
            Directions::West => Directions::South,
            Directions::East => Directions::North,
        };
    }
    fn backward(&mut self) {
        self.d = match self.d {
            Directions::North => Directions::West,
            Directions::South => Directions::East,
            Directions::West => Directions::North,
            Directions::East => Directions::South,
        };
    }
    fn north(&self) -> Self {
        Self {
            x: self.x ,
            y: self.y - 1,
            d: Directions::North,
        }
    }
    fn south(&self) -> Self {
        Self {
            x: self.x ,
            y: self.y + 1,
            d: Directions::South,
        }
    }
    fn east(&self) -> Self {
        Self {
            x: self.x + 1 ,
            y: self.y,
            d: Directions::East,
        }
    }
    fn west(&self) -> Self {
        Self {
            x: self.x - 1 ,
            y: self.y,
            d: Directions::West,
        }
    }
}

impl Grid {
    fn new() -> Self {
        Self {
            grid: vec![],
        }
    }
    fn navigate(&self) -> HashMap<(isize,isize),usize> {
        let mut energized: HashMap<(isize,isize),usize> = HashMap::new();
        let mut beams: VecDeque<Beam> = VecDeque::new();

        beams.push_back(Beam::new());

        while beams.len() > 0 {
            let mut curr = beams.pop_front().unwrap();
            let mut split = false;

            while curr.in_grid(self.grid[0].len(), self.grid.len()) && !split {

                match Pieces::from(self.grid[curr.y as usize][curr.x as usize]) {
                    Pieces::Forward => curr.forward(),
                    Pieces::Backward => curr.backward(),
                    Pieces::Horizontal => match curr.d {
                        Directions::North => {
                            beams.push_back(curr.east());
                            beams.push_back(curr.west());
                            split = true;
                        },
                        Directions::South => {
                            beams.push_back(curr.east());
                            beams.push_back(curr.west());
                            split = true;
                        },
                        _ => (),
                    },
                    Pieces::Vertical => match curr.d {
                        Directions::East => {
                            beams.push_back(curr.north());
                            beams.push_back(curr.south());
                            split = true;
                        },
                        Directions::West => {
                            beams.push_back(curr.north());
                            beams.push_back(curr.south());
                            split = true;
                        },
                        _ => (),

                    },
                    _ => ()
                }
                energized.entry((curr.x,curr.y)).and_modify(|x| *x += 1).or_insert(1);
                match energized.get(&(curr.x,curr.y)) {
                    Some(x) => if *x > 100 {break},
                    None => (),
                };
                curr.move_one();
            }
        }
        energized
    }
}

impl FromStr for Grid {
    type Err = GridError;
    fn from_str(s: &str) -> Result<Self,Self::Err> {
        Ok(Self {
            grid: s.split("\n")
                    .map(|x| x.chars().collect::<Vec<char>>() )
                    .collect(),
        })
    }
}


fn parse_input(input_file: &str) -> Grid {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    Grid::from_str(&input.trim()).unwrap()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 46);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 0);
    }
}
