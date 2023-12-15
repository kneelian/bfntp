# bfntp

A simple Brainfuck/bf interpreter and transpiler

## About Brainfuck

The programming language [Brainfuck](https://en.wikipedia.org/wiki/Brainfuck) is a Turing-complete minimalistic esoteric language (esolang) by Urban Müller. It isn't the smallest possible such language (for a more minimal example, check out the Wiki page on [OISC](https://en.wikipedia.org/wiki/One-instruction_set_computer)s), but it did inspire a lot of modern interest in esolangs. It is also very dear to me: it was, unfortunately, the first programming language I actually used, and the one I learned programming with.

## About bfntp

This repository contains the code to a relatively simple interpreter/compiler for Brainfuck that I call **bfntp** (short for **B**rain**f**uck i**nt**er**p**reter) that I wrote in a few evenings. It's not particularly clean or serious code, and the resulting interpreter is *not* particularly efficient or fast: on my machine, it takes it ~8.75 seconds on my machine to run the sample `mandelbrot.bf` program, whereas [rdebath's tritium](https://github.com/rdebath/Brainfuck/tree/master/tritium) blazes through it in ~0.4 seconds using the option `-b16`. Nevertheless, it *does* perform some optimisations, enabled with the flag `-o`, and without them it takes it ~22.5 seconds to print the Mandelbrot set. It's also got a tiny Brainfuck analysis toolset, currently only counting the number of executed instructions from start to finish (with the `-c` flag).

Apart from just interpreting Brainfuck code directly, `bfntp` has the ability to transpile it to C++ code that uses a `std::vector<uint16_t>` as its memory tape, which (with Brainfuck optimisations and using `-O3` with your C++ compiler of choice) provides a reasonably performant binary that is, for example, only ~2x slower than the `tritium` JIT interpreter for `mandelbrot.bf`.

`bfntp` uses 16-bit cells in a `std::vector<>` tape, though any numeric type that implements `{ ++, --, +=, -= }` comparisons with integers and overflow wraparound can be substituted with minimal changes (for example, if you want to use floating point cells, it's not going to be a simple edit because they don't wrap from highest to lowest). Larger cell types can sometimes increase runtime significantly, due to how Brainfuck programs use wraparound, though this completely depends on the program in question. By default, it provides a tape that is 32768 cells long and accessing beyond the end of tape *on either end* will probably give you a segfault; in safe mode (enabled by the `-s` flag; currently doesn't work with optimisations), the interpreter checks for out-of-bounds accesses and grows the tape every time it runs out, and makes sure you can't read before the beginning of the tape (so a program that starts with `<+` will not segfault in safe mode).

There are many obvious clean-up areas and optimisation tactics that aren't used: `bfntp` was put together in the span of a few evenings and is not an exceptionally powerful tool, but it does fully support all vanilla Brainfuck features. Physical limitations aside, it provides a full Turing-complete environment (though not a particularly fast one).

## Usage

`bfntp` is a bit finnicky to use. It takes the filepath of the brainfuck input file as its first argument, and all other command-line options always follow it. It will complain if the first argument isn't a file path. You can use the option `-h` to see the full set of options provided by `bfntp` and commentary on each of them (obviously, you don't need to provide a file path for the usage notes).

## Building

There is one external dependency, [fmt](https://github.com/fmtlib/fmt), and was built against v10.0.0 of the library; it expects its headers in `./include/fmt`, and its shared library object in `./lib`. There is a `build.sh` script in the root directory, the contents of which are simply `g++ main.cpp -o main.a -O3 -Iinclude -L./lib -lfmt`. The `fmt` library can be relatively easily replaced with `printf()`, though I haven't taken the time to do so as I wanted to experiment a little with it. `bfntp` should build with any compiler compatible with C++11, as the `fmt` header uses type traits, and `bfntp` itself uses fixed-width integer types. Rewriting it to comply with C++03 would probably be an easy job (replace `fmt`, and either add `#include <cstdint>` or just use the regular types), but it's more hassle than I feel is worth (I don't see myself using a compiler that's stuck on C++03 any time soon).
