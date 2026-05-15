alias b := build
alias r := run
alias t := test


build:
    cmake --build build

run: build
    ./build/apps/app

test: build
    ./build/src/test_synthlib