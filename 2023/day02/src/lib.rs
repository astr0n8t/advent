use std::fs;

#[derive(Clone, Default, Debug)]
struct Game {
    pub id: u32,
    pub rounds: Vec<Round>,
}

impl Game {
    pub fn possible(self: &Game, source: &Round) -> bool {
        for round in self.rounds.clone() {
            if !round.possible(source) {
                return false;
            }
        }
        true
    }
}

#[derive(Clone, Default, Debug)]
struct Round {
    pub red: u32,
    pub green: u32,
    pub blue: u32,
}

impl Round {
    pub fn possible(self: &Round, source: &Round) -> bool {
        if self.red <= source.red &&
            self.green <= source.green &&
            self.blue <= source.blue {
                return true;
        }
        false
    }
}

pub fn part1(input_file: &str) -> u64 {
    let minimums = Round {
        red: 12,
        green: 13,
        blue: 14,
    };
    let mut sum: u64 = 0;
    for item in parse_input(input_file) {
        if item.possible(&minimums) {
            sum += u64::from(item.id);
        }
    }
    sum
}

pub fn part2(input_file: &str) -> u64 {
    println!("Opening {}", input_file);
    0
}


fn parse_input(input_file: &str) -> Vec<Game> {
    let mut games: Vec<Game> = vec![];
    let input = fs::read_to_string(input_file)
        .expect("Something went wrong reading the file");
    let input = input.split("\n");

    for line in input {
        if line.len() <= 1 {
            continue
        }
        let mut game: Game = Game::default();

        let mut split_line = line.split(":");
        game.id = split_line.next()
            .expect("Error processing line")
            .split(" ")
            .last()
            .expect("Error processing line")
            .parse::<u32>()
            .unwrap();
        let split_game = split_line.next()
            .expect("Error processing line")
            .split(";");

        for item in split_game {
            let mut round: Round = Round::default();
            for color in item.split(",") {
                let mut split_color = color.trim().split(" ");
                let quantity = split_color.next()
                    .expect("Error processing line")
                    .parse::<u32>()
                    .unwrap();
                match split_color.last().expect("Color should exist") {
                    "blue" => {
                        round.blue = quantity;
                    },
                    "red" => {
                        round.red = quantity;
                    },
                    "green" => {
                        round.green = quantity;
                    },
                    &_ => ()

                }
            }
            game.rounds.push(round);
        }
        games.push(game.clone());
    }
    games
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test1.txt"), 8);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test2.txt"), 0);
    }
}
