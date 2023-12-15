#pragma once

#include "fmt/core.h"
#include "fmt/printf.h"
#include "fmt/color.h"

void print_help()
{
	fmt::print("{0} v0.2a usage notes\n", 
        fmt::styled("bfntp" , fmt::fg(fmt::color::orange_red) | fmt::emphasis::bold)
    );
    fmt::print("  {0} source [options]:\n",
        fmt::styled("bfntp", fmt::emphasis::bold)
    );
    fmt::print("    -h: print this usage note and exit\n");
    fmt::print("    -s: safe mode: keep track of tape size and reads past the end\n");
    fmt::print("                   and resize tape when accessing last element\n");
    fmt::print("                   {0}: this is very slow compared to unsafe mode\n",
        fmt::styled("note", fmt::emphasis::bold)
    );
    fmt::print("    -c:   counter: keeps track of how many BF instructions have been\n");
    fmt::print("                   executed and prints the number at the end\n");
    fmt::print("                   {0}: for especially huge programs, this might overflow\n",
        fmt::styled("note", fmt::emphasis::bold)
    );
    fmt::print("                   the counter, which will trip a flag and stop further\n");
    fmt::print("                   cycle counting\n");
    fmt::print("                   {0}: must be used in safe mode, otherwise does nothing!\n",
        fmt::styled("note", fmt::emphasis::bold)
    );
    fmt::print("    -d:  debugger: [not implemented]\n");
    fmt::print("    -o: optimised: attempts to do some elementary constant simplification\n");
    fmt::print("                   such as zero loop elimination and condensing moves and\n");
    fmt::print("                   arithmetic into one operation\n");
    fmt::print("                   {0}: negates the -c flag because some optimisations\n",
        fmt::styled("note", fmt::emphasis::bold)
    );
    fmt::print("                   invalidate op counts.\n");
    fmt::print("    -t: transpile: instead of executing the brainfuck code directly, it\n");
    fmt::print("                   will print a transpiled CPP equivalent of the code and\n");
    fmt::print("                   exit. You can then compile the output using a normally\n");
    fmt::print("                   conforming CPP compiler\n");
    return;
}