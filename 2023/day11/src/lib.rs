use std::fs;

pub fn part1(input_file: &str) -> usize {
    let mut sum = 0;
    let galaxies = parse_input(input_file);

    for i in 0..galaxies.len() {
        for j in i..galaxies.len() {
            sum += (i64::abs(
                    (galaxies[i].x+galaxies[i].x_offset)
                    -
                    (galaxies[j].x+galaxies[j].x_offset)
                    ) + i64::abs(
                    (galaxies[i].y+galaxies[i].y_offset)
                    -
                    (galaxies[j].y+galaxies[j].y_offset)
                    )) as usize;
        }
    }
    sum
}

pub fn part2(input_file: &str) -> usize {
    let mut sum = 0;
    let galaxies = parse_input(input_file);

    for i in 0..galaxies.len() {
        for j in i..galaxies.len() {
            sum += (i64::abs(
                    (galaxies[i].x + (galaxies[i].x_offset*1000000) - galaxies[i].x_offset)
                    -
                    (galaxies[j].x + (galaxies[j].x_offset*1000000) - galaxies[j].x_offset)
                    ) + i64::abs(
                    (galaxies[i].y + (galaxies[i].y_offset*1000000) - galaxies[i].y_offset)
                    -
                    (galaxies[j].y + (galaxies[j].y_offset*1000000) - galaxies[j].y_offset)
                    )) as usize;
        }
    }
    sum
}

#[derive(Debug)]
struct Galaxy {
    x: i64,
    y: i64,
    x_offset: i64,
    y_offset: i64,
}

fn parse_input(input_file: &str) -> Vec<Galaxy> {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    let mut grid: Vec<Vec<char>> = vec![];

    for line in input.trim().split("\n") {
        let mut row: Vec<char> = vec![];
        for c in line.trim().chars() {
            row.push(c);
        }
        grid.push(row);
    }

    let mut offset_y: Vec<usize> = vec![];
    for y in 0..grid.len() {
        let mut num_galaxies = 0;
        for x in 0..grid[y].len() {
            if grid[y][x] == '#' {
                num_galaxies += 1;
            }

        }
        let mut offset = if num_galaxies == 0 {1} else {0};
        if y > 0 {
            offset += offset_y[y-1];
        }
        offset_y.push(offset);
    }

    let mut offset_x: Vec<usize> = vec![];
    for x in 0..grid[0].len() {
        let mut num_galaxies = 0;
        for y in 0..grid.len() {
            if grid[y][x] == '#' {
                num_galaxies += 1;
            }

        }
        let mut offset = if num_galaxies == 0 {1} else {0};
        if x > 0 {
            offset += offset_x[x-1];
        }
        offset_x.push(offset);
    }


    let mut galaxies: Vec<Galaxy> = vec![];

    for y in 0..grid.len() {
        for x in 0..grid[y].len() {
            if grid[y][x] == '#' {
                galaxies.push(
                    Galaxy {
                        x: x as i64,
                        y: y as i64,
                        x_offset: offset_x[x] as i64,
                        y_offset: offset_y[y] as i64,
                    });
            }

        }
    }

    galaxies
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(part1("data/test.txt"),374);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 82000210);
    }
}
