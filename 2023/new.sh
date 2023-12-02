#!/bin/bash
cargo install cargo-generate

cargo generate --path template --name $1
