use std::fs;
use std::cmp;
use std::str::FromStr;

#[derive(Clone, Default, Debug)]
struct Game {
    pub id: u32,
    pub rounds: Vec<Round>,
}
#[derive(Debug)]
struct GameError;

impl Game {
    pub fn new() -> Self {
        Self { id: 0, rounds: vec![] }
    }
    pub fn possible(self: &Game, source: &Round) -> bool {
        self.rounds.iter()
            .all(|x| x.possible(source))
    }
    pub fn power(self: &Game) -> u32 {
        let mut max = Round::default();
        for round in &self.rounds {
            max.red = cmp::max(max.red, round.red);
            max.green = cmp::max(max.green, round.green);
            max.blue = cmp::max(max.blue, round.blue);
        }
        max.red*max.green*max.blue
    }
}

// Parses the format "Game 0: 0 color, 0 color; 0 color, 0 color"
impl FromStr for Game {
    type Err = GameError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut game = Game::new();
        let line = String::from_str(s).unwrap();

        let mut line = line.split(":");
        // Capture the game id from line format "Game 1: <content>"
        game.id = line.next()
            .unwrap()
            .split(" ")
            .last()
            .unwrap()
            .parse::<u32>()
            .unwrap();
        // Redefine line to be everything after "Game 1:"
        let line = line.next()
            .unwrap();

        // Create our rounds
        for round in line.split(";") {
            match Round::from_str(round) {
                Ok(round) => game.rounds.push(round),
                Err(error) => panic!("Problem creating round from line: {:?}, {:?}", round, error),
            }
        }

        Ok(game)
    }
}

#[derive(Clone, Default, Debug)]
struct Round {
    pub red: u32,
    pub green: u32,
    pub blue: u32,
}
#[derive(Debug)]
struct RoundError;

impl Round {
    pub fn new() -> Self {
        Self { red: 0, green: 0, blue: 0 }
    }
    pub fn possible(self: &Round, source: &Round) -> bool {
        self.red <= source.red &&
        self.green <= source.green &&
        self.blue <= source.blue
    }
}

// Parses the format "0 color, 0 color, 0 color"
impl FromStr for Round {
    type Err = RoundError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut round = Round::new();
        let line = String::from_str(s).unwrap();
        for color in line.split(",") {
            // Redefine color to be an iterator of the color and quantity
            let mut color = color.trim().split(" ");
            // Set the quantity from format "1 red"
            let quantity = color.next()
                .unwrap()
                .parse::<u32>()
                .unwrap();
            // Set the color to actually be the color
            let color = color.last()
                .unwrap();
            // Match appropriately to set the right quantity
            match color {
                "blue" => {
                    round.blue = quantity;
                },
                "red" => {
                    round.red = quantity;
                },
                "green" => {
                    round.green = quantity;
                },
                _ => ()
            }
        }

        Ok(round)
    }
}

pub fn part1(input_file: &str) -> u32 {
    let minimums = Round {
        red: 12,
        green: 13,
        blue: 14,
    };
    parse_input(input_file)
        .iter()
        .map(|x| if x.possible(&minimums) { x.id } else { 0 })
        .sum()
}

pub fn part2(input_file: &str) -> u32 {
    parse_input(input_file)
        .iter()
        .map(|x| x.power())
        .sum()
}

fn parse_input(input_file: &str) -> Vec<Game> {
    let mut games: Vec<Game> = vec![];
    let input = fs::read_to_string(input_file)
        .expect("Something went wrong reading the file");

    for line in input.split("\n") {
        // Make sure this line contains a game
        if !line.contains("Game") {
            continue;
        }
        match Game::from_str(line) {
            Ok(game) => games.push(game),
            Err(error) => panic!("Problem creating game from line: {:?} {:?}", line, error),
        }
    }

    games
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
        assert_eq!(part2("data/test.txt"), 2286);
    }
}
