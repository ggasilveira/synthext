alias b := build
alias r := run


build:
    make -C build

run: build
    ./build/apps/app
