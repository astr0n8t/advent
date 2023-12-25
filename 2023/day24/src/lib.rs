use std::fs;
use std::str::FromStr;

use z3::ast::{Ast, Int};
use maths_rs::{Vec3d, line_segment_vs_line_segment};

pub fn part1(input_file: &str) -> usize {
    parse_input(input_file)
        .num_intersect_within_range(200000000000000.0,400000000000000.0)
}

pub fn part2(input_file: &str) -> usize {
    parse_input(input_file)
        .compute_intersect_vector() as usize
}

#[derive(Debug)]
struct VectorList {
    list: Vec<Vector3d>,
}
#[derive(Debug)]
struct VectorListError;

impl VectorList {
    fn num_intersect_within_range(&self, r_start: f64, r_end: f64) -> usize {
        let mut total = 0;
        for i in 0..(self.list.len()-1) {
            for j in (i+1)..self.list.len() {
                if self.list[i].intersects_within_range(&self.list[j], r_start, r_end) {
                    total += 1;
                }
            }
        }
        total
    }
    // Adapted this z3 solve: https://github.com/AxlLind/AdventOfCode2023/blob/main/src/bin/24.rs
    fn compute_intersect_vector(&self) -> i64 {
        let ctx = z3::Context::new(&z3::Config::new());
        let s = z3::Solver::new(&ctx);

        let i_x = Int::new_const(&ctx, "i_x");
        let i_y = Int::new_const(&ctx, "i_y");
        let i_z = Int::new_const(&ctx, "i_z");
        let i_x_v = Int::new_const(&ctx, "i_x_v");
        let i_y_v = Int::new_const(&ctx, "i_y_v");
        let i_z_v = Int::new_const(&ctx, "i_z_v");

        let zero = Int::from_i64(&ctx, 0);

        for (i, vec) in self.list.iter().enumerate() {
            let x = Int::from_i64(&ctx, vec.pos.x as _);
            let y = Int::from_i64(&ctx, vec.pos.y as _);
            let z = Int::from_i64(&ctx, vec.pos.z as _);
            let x_v = Int::from_i64(&ctx, vec.vel.x as _);
            let y_v = Int::from_i64(&ctx, vec.vel.y as _);
            let z_v = Int::from_i64(&ctx, vec.vel.z as _);

            let time = Int::new_const(&ctx, format!("t{i}"));

            s.assert(&time.ge(&zero));
            s.assert(&((&x + &x_v * &time)._eq(&(&i_x + &i_x_v * &time))));
            s.assert(&((&y + &y_v * &time)._eq(&(&i_y + &i_y_v * &time))));
            s.assert(&((&z + &z_v * &time)._eq(&(&i_z + &i_z_v * &time))));
        }
        assert_eq!(s.check(), z3::SatResult::Sat);
        let model = s.get_model().unwrap();
        let res = model.eval(&(&i_x + &i_y + &i_z), true).unwrap();
        res.as_i64().unwrap()
    }
}

impl FromStr for VectorList {
    type Err = VectorListError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        Ok(Self {
            list: s.split("\n").map(|l|{
                Vector3d::from_str(l).unwrap()
            }).collect(),
        })
    }
}

#[derive(Debug)]
struct Vector3d {
    pos: Vec3d,
    vel: Vec3d,
}
#[derive(Debug)]
struct Vector3dError;

impl Vector3d {
    fn intersects_within_range(&self, o: &Self, r_start: f64, r_end: f64) -> bool {
        let mut a_start = self.pos;
        a_start.z = 0.0;
        let mut a_end = a_start;
        a_end.x += self.vel.x * r_end;
        a_end.y += self.vel.y * r_end;

        let mut b_start = o.pos;
        b_start.z = 0.0;
        let mut b_end = b_start;
        b_end.x += o.vel.x * r_end;
        b_end.y += o.vel.y * r_end;

        let intersection = line_segment_vs_line_segment(a_start, a_end, b_start, b_end);

        if let Some(i) = intersection {
            if i.x >= r_start && i.x <= r_end && i.y >= r_start && i.y <= r_end {
                return true;
            }
        }
        false
    }
}

impl FromStr for Vector3d {
    type Err = Vector3dError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let (pos,vel) = s.split_once(" @ ").unwrap();

        let mut pos = pos.split(", ");
        let pos: Vec3d = Vec3d::new(
            pos.next().unwrap().trim().parse::<f64>().unwrap(),
            pos.next().unwrap().trim().parse::<f64>().unwrap(),
            pos.next().unwrap().trim().parse::<f64>().unwrap(),
           );

        let mut vel = vel.split(", ");
        let vel: Vec3d = Vec3d::new(
            vel.next().unwrap().trim().parse::<f64>().unwrap(),
            vel.next().unwrap().trim().parse::<f64>().unwrap(),
            vel.next().unwrap().trim().parse::<f64>().unwrap(),
           );

        Ok(Self{ pos, vel })
    }
}


fn parse_input(input_file: &str) -> VectorList {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    VectorList::from_str(input.trim()).unwrap()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test1() {
        assert_eq!(parse_input("data/test.txt").num_intersect_within_range(7.0,27.0), 2);
    }

    #[test]
    fn test2() {
        assert_eq!(part2("data/test.txt"), 47);
    }
}
