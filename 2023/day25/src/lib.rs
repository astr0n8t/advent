use std::fs;
use std::collections::VecDeque;
use rustc_hash::{FxHashMap, FxHashSet};

// Adapted this solution https://github.com/maneatingape/advent-of-code-rust/blob/main/src/year2023/day25.rs
pub fn part1(input_file: &str) -> usize {
    let input = parse_input(input_file);

    let mut freq = FxHashMap::default();

    for curr in input.keys() {

        let mut queue = VecDeque::new();
        let mut hist = FxHashSet::default();

        queue.push_back(curr);
        hist.insert(curr);

        while let Some(curr) = queue.pop_front() {
            for neighbor in input.get(curr).unwrap() {
                if hist.insert(neighbor) {
                    let key = if curr < neighbor { [curr, neighbor] } else { [neighbor, curr] };

                    let entry = freq.entry(key).or_insert(0);
                    *entry += 1;

                    queue.push_back(neighbor);
                }
            }
        }
    }

    // Grabs three top edges
    let mut cut = freq.iter().collect::<Vec<_>>();
    cut.sort_by(|(_,a),(_,b)| b.cmp(&a));
    let cut: Vec<_> = cut.iter().take(3)
        .map(|(k,_)| *k)
        .collect();

    let mut hist = FxHashSet::default();
    let mut queue = VecDeque::new();

    let curr = input.keys().next().unwrap();
    let mut size = 1;
    queue.push_back(curr);
    hist.insert(curr);

    while let Some(curr) = queue.pop_front() {
        for neighbor in input.get(curr).unwrap() {
            let key = if curr < neighbor { [curr, neighbor] } else { [neighbor, curr] };

            if cut.contains(&&key) {
                continue;
            }

            if hist.insert(neighbor) {
                size += 1;
                queue.push_back(neighbor);
            }
        }
    }
    size * (input.len() - size)
}

fn parse_input(input_file: &str) -> FxHashMap<String,FxHashSet<String>> {
    let input = fs::read_to_string(input_file)
        .expect("Unable to open input data");
    let mut nodes = FxHashMap::default();

    for line in input.trim().split("\n") {
        let (node,edge) = line.split_once(": ").unwrap();
        if !nodes.contains_key(&node.to_string()) {
            nodes.insert(node.to_string(), FxHashSet::default());
        }
        for edge in edge.trim().split(" ") {
            if !nodes.contains_key(&edge.to_string()) {
                nodes.insert(edge.to_string(), FxHashSet::default());
            }
            nodes.entry(node.to_string()).and_modify(|n| {n.insert(edge.to_string());});
            nodes.entry(edge.to_string()).and_modify(|n| {n.insert(node.to_string());});
        }
    }
    nodes
}
