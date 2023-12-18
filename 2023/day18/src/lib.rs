use std::fs;

pub fn part1(input_file: &str) -> usize {
    let input = parse_input(input_file, false);
    shoelace(&input) + (perimeter(&input)/2) + 1
}

pub fn part2(input_file: &str) -> usize {
    let input = parse_input(input_file, true);
    shoelace(&input) + (perimeter(&input)/2) + 1
}

fn perimeter(array: &Vec<(isize,isize)>) -> usize {
    let mut sum: usize = 0;

    for i in 1..array.len() {
        sum += (isize::abs(array[i-1].0-array[i].0)
                +
                isize::abs(array[i-1].1-array[i].1)
                ) as usize;
    }
    sum
}

fn shoelace(array: &Vec<(isize,isize)>) -> usize {
    let mut sum: isize = 0;

    for i in 0..array.len() {
        sum += (array[i].0*array[(i+1) % array.len()].1) - (array[i].1*array[(i+1) % array.len()].0);
    }

    (isize::abs(sum) / 2) as usize
}

fn parse_input(input_file: &str, flip: bool) -> Vec<(isize,isize)> {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");

    let mut out: Vec<(isize,isize)> = vec![];

    let mut x: isize = 0;
    let mut y: isize = 0;

    out.push((x,y));

    for item in input.trim().split("\n") {
        let dir: u8;
        let length: isize;
        if flip {
            let rest = item.split(" ").last().unwrap();
            let rest = rest.trim_start_matches("(#")
                .trim_end_matches(")");
            let d = rest.chars().last().unwrap();
            let rest = rest.strip_suffix(d).unwrap();
            length = isize::from_str_radix(rest,16).unwrap();
            dir = d as u8 - '0' as u8;

        } else {
            let (d, r) = item.split_once(" ").unwrap();
            let (l, _) = r.split_once(" ").unwrap();
            dir = match d {
                "U" => 3,
                "L" => 2,
                "D" => 1,
                "R" => 0,
                _ => unreachable!(),
            };
            length = l.parse().unwrap();

        }

        match dir {
            0 => x += length,
            1 => y += length,
            2 => x -= length,
            3 => y -= length,
            _ => unreachable!(),
        }

        out.push((x,y));
    }

    out
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"), 62);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 952408144115);
    }
}
