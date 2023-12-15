#include <fstream>
#include <sstream>
#include <stack>
#include <vector>
#include <string>

#include <regex>

#include "help-usage.hpp"

#include "fmt/core.h"
#include "fmt/printf.h"
#include "fmt/color.h"

#include "types.hpp"
#include "transpiler.hpp"

#include "interpret_unsafe_unoptimised.hpp"
#include "interpret_unsafe_optimised.hpp"
#include "interpret_safe_unoptimised.hpp"
#include "interpret_safe_optimised.hpp"

int main(int argc, char** argv)
{
    std::string program;
    bool safe = false;
    bool counter = false;
    bool optimise = false;
    bool transpile = false;
    
    if(argc > 1)
    {
        if(argv[1] == std::string("-h"))
        {
            print_help();
            return 0;
        }
        std::ifstream f;
        f.open(argv[1]);

        if(!f) 
        {
            fmt::print("{0}: invalid file path! Try the option `-h`\n",
            fmt::styled("bfntp" , fmt::fg(fmt::color::red) | fmt::emphasis::bold)
            ); 
            return -1; 
        }

        for(std::string opt : std::vector<std::string>(argv + 1, argv + argc))
        {
            if(opt == "-s") { safe = true; }
            if(opt == "-c") { counter = true; }
            if(opt == "-o") { optimise = true; }
            if(opt == "-t") { transpile = true; }
        }

        if((not safe) and counter) { counter = false; }
        if(optimise and counter) { counter = false; }

        std::stringstream ss;
        ss << f.rdbuf();

        program = ss.str();
    } else {
        fmt::print(std::string("Please supply the path of the brainfuck source as the first commandline argument\n") + 
            "If you don't know how to use this program, the commandline argument `-h` will give you" + 
            " usage notes.\n");
        return 1;
    }

    std::vector<uint16_t>   mem_tape; mem_tape.resize(32 * 1024); // standard brainfuck 30k cells          
    std::stack<size_t>      bracket_insn_pointers;

    bracket_insn_pointers.push(0);

    size_t brackets_to_skip = 0;

    int64_t insn_num = 0;
    bool overflow = false;

    uint32_t temp = 0;

    if(transpile)
    {
        transpiler(program, optimise, safe);
        return 0;
    }

    if(!optimise)
    {
        if(!safe)
            interpret_raw_unsafe(program, mem_tape, bracket_insn_pointers);
        else
            interpret_raw_safe(program, mem_tape, bracket_insn_pointers, overflow, counter, insn_num);
    }
    else
    {
        std::vector<insn_t> opt_program;
        optim(program, opt_program);
        
        if(!safe)
            interpret_opt_unsafe(opt_program, mem_tape, bracket_insn_pointers);
        else
            interpret_opt_safe(opt_program, mem_tape, bracket_insn_pointers);
    }   

    if(counter)
    {
        fmt::print("the program finished in {0} instructions and {1}\n", insn_num,
            overflow?"overflowed":"didn't overflow");
    }

    fmt::print("\n");
    return 0;
}