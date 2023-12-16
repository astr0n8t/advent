use std::fs;
use std::str::FromStr;
use std::collections::HashMap;
use std::collections::VecDeque;
use std::cmp;

pub fn part1(input_file: &str) -> usize {
    parse_input(input_file)
        .navigate(Beam::new(0,0,Directions::East))
}

pub fn part2(input_file: &str) -> usize {
    parse_input(input_file)
        .optimize()
}

#[derive(Debug)]
enum Pieces {
    Vertical,
    Horizontal,
    Forward,
    Backward,
    Space,
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
    North,
    South,
    East,
    West,
}

impl Directions {
    fn val(&self) -> usize {
        match self {
            Directions::North => 3,
            Directions::South => 5,
            Directions::East => 7,
            Directions::West => 11,
        }
    }
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
    fn new(x: isize, y: isize, d: Directions) -> Self {
        Self {
            x,
            y,
            d,
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
    fn navigate(&self, start: Beam) -> usize {
        let mut energized: HashMap<(isize,isize),usize> = HashMap::new();
        let mut beams: VecDeque<Beam> = VecDeque::new();

        beams.push_back(start);

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
                // Break if we've already hit this spot from this direction before
                match energized.get(&(curr.x,curr.y)) {
                    Some(x) => if *x % curr.d.val() == 0 {break},
                    None => (),
                };
                // Otherwise mark this spot as visited from this direction
                energized.entry((curr.x,curr.y)).and_modify(|x| *x *= curr.d.val()).or_insert(curr.d.val());
                // Continue
                curr.move_one();
            }
        }
        energized.len()
    }
    fn optimize(&mut self) -> usize {
        let mut max = 0;
        for x in 0..self.grid[0].len() {
            max = cmp::max(self.navigate(Beam::new(x as isize,0,Directions::South)), max);
        }
        for x in 0..self.grid[0].len() {
            max = cmp::max(self.navigate(Beam::new(x as isize,self.grid.len() as isize - 1,Directions::North)), max);
        }
        for y in 0..self.grid.len() {
            max = cmp::max(self.navigate(Beam::new(0,y as isize,Directions::East)), max);
        }
        for y in 0..self.grid.len() {
            max = cmp::max(self.navigate(Beam::new(self.grid[0].len() as isize - 1,y as isize,Directions::West)), max);
        }
        max
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
        assert_eq!(part2("data/test.txt"), 51);
    }
}
