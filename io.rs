use std::io::{BufWriter, stdin, stdout, Write};

#[derive(Default)]
struct Scanner {
    buffer: Vec<String>
}

impl Scanner {
    fn next<T: std::str::FromStr>(&mut self) -> T {
        loop {
            if let Some(token) = self.buffer.pop() {
                return token.parse().ok().expect("Failed parse");
            }
            let mut input = String::new();
            stdin().read_line(&mut input).expect("Failed read");
            self.buffer = input.split_whitespace().rev().map(String::from).collect();
        }
    }
}

fn solve(n: usize, m: u64, w: Vec<u64>) -> Vec<usize>
{
    let mut result = Vec::new();
    let mut sum: u64 = 0;
    for i in 0..n {
        if w[i] <= m {
            if 2 * w[i] >= m {
                return vec![i];
            } else {
                result.push(i);
                sum += w[i];
                if 2 * sum >= m {
                    return result;
                }
            }
        }
    }
    return vec![];
}

fn main() {
    let mut scan = Scanner::default();
    let out = &mut BufWriter::new(stdout());
}
