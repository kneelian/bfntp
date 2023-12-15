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

void optim(std::string&, std::vector<insn_t>&);
void transpiler(std::string&, bool, bool);

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

    size_t insn_pointer = 0;
    size_t data_pointer = 0;

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
        {
            for(insn_pointer = 0; insn_pointer < program.size(); insn_pointer++)
            {
                char insn = program[insn_pointer];
                switch(insn)
                {
                    case '+':
                        mem_tape[data_pointer]++;
                        break;
                    case '-':
                        mem_tape[data_pointer]--;
                        break;
                    case '>':
                        data_pointer++;
                        break;
                    case '<':
                        data_pointer--;
                        break;
                    case '.':
                        fmt::printf("%lc", mem_tape[data_pointer]);
                        break;
                    case ',':
                        fmt::print("\x1b\7");
                        mem_tape[data_pointer] = getchar();
                        fflush(stdin);
                        fmt::print("\x1b\10\x1b[1C\x1b[1A");
                        break;
                    case '[':
                        if(mem_tape[data_pointer] == 0)
                        {
                            brackets_to_skip = 1;
                            size_t j = insn_pointer;
                            while(brackets_to_skip)
                            {
                                j++;
                                if(program[j] == '[') { brackets_to_skip++; }
                                if(program[j] == ']') { brackets_to_skip--; }
                                if(brackets_to_skip == 0) { insn_pointer = j; break; }
                            }
                        } else
                        {
                            bracket_insn_pointers.push(insn_pointer-1);
                        }
                        break;
                    case ']':
                        insn_pointer = bracket_insn_pointers.top();
                        bracket_insn_pointers.pop();
                        break;
                    case '@':
                        if(optimise) { mem_tape[data_pointer] = 0; break; }
                        else { break; }
                    default:
                        break;
                }
            }
        } else
        {
            for(insn_pointer = 0; insn_pointer < program.size(); insn_pointer++)
            {
                char insn = program[insn_pointer];
                switch(insn)
                {
                    case '+':
                        if(counter) { insn_num++; if(insn_num < 0) { overflow = true; } }
                        mem_tape[data_pointer]++;
                        break;
                    case '-':
                        if(counter) { insn_num++; if(insn_num < 0) { overflow = true; } }
                        mem_tape[data_pointer]--;
                        break;
                    case '>':
                        if(counter) { insn_num++; if(insn_num < 0) { overflow = true; } }
                        if(data_pointer - mem_tape.size() <= 2)
                            mem_tape.resize(mem_tape.size() + (mem_tape.size() / 2), 0);
                        data_pointer++;
                        break;
                    case '<':
                        if(counter) { insn_num++; if(insn_num < 0) { overflow = true; } }
                        data_pointer = data_pointer>0? data_pointer-1: data_pointer;
                        break;
                    case '.':
                        if(counter) { insn_num++; if(insn_num < 0) { overflow = true; } }
                        fmt::printf("%lc", mem_tape[data_pointer]);
                        break;
                    case ',':
                        if(counter) { insn_num++; if(insn_num < 0) { overflow = true; } }
                        fmt::print("\x1b\7");
                        mem_tape[data_pointer] = getchar();
                        fflush(stdin);
                        fmt::print("\x1b\10\x1b[1C\x1b[1A");
                        break;
                    case '[':
                        if(mem_tape[data_pointer] == 0)
                        {
                            brackets_to_skip = 1;
                            size_t j = insn_pointer;
                            while(brackets_to_skip)
                            {
                                if(counter) { insn_num++; if(insn_num < 0) { overflow = true; } }
                                j++;
                                if(program[j] == '[') { brackets_to_skip++; }
                                if(program[j] == ']') { brackets_to_skip--; }
                                if(brackets_to_skip == 0) { insn_pointer = j; break; }
                            }
                        } else
                        {
                            if(counter) { insn_num++; if(insn_num < 0) { overflow = true; } }
                            bracket_insn_pointers.push(insn_pointer-1);
                        }
                        break;
                    case ']':
                        if(counter) { insn_num++; if(insn_num < 0) { overflow = true; } }
                        insn_pointer = bracket_insn_pointers.top();
                        bracket_insn_pointers.pop();
                        break;
                    default:
                        break;
                }
            }
        }
    }
    else // optimised, using a different base datatype
    {
        std::vector<insn_t> opt_program;
        optim(program, opt_program);
        
        if(!safe)
        {
            for(size_t ipt = 0; ipt < opt_program.size(); ipt++)
            {
                switch(opt_program[ipt].type)
                {
                    case CLEAR:
                        mem_tape[data_pointer] = 0; 
                        break;
                    case PLUS:
                        mem_tape[data_pointer] += opt_program[ipt].count;
                        break;
                    case MINUS:
                        mem_tape[data_pointer] -= opt_program[ipt].count;
                        break;
                    case LEFT:
                        data_pointer -= opt_program[ipt].count;
                        break;
                    case RIGHT:
                        data_pointer += opt_program[ipt].count;
                        break;
                    case DOT:
                        for(int j = opt_program[ipt].count; j > 0; j--)
                            fmt::printf("%lc", mem_tape[data_pointer]);
                        break;
                    case COMMA:
                        fmt::print("\x1b\7");
                        mem_tape[data_pointer] = getchar();
                        fflush(stdin);
                        fmt::print("\x1b\10\x1b[1C\x1b[1A");
                        break;
                    case LBRACKET:
                        if(mem_tape[data_pointer] == 0)
                        {
                            brackets_to_skip = 1;
                            size_t j = ipt;
                            while(brackets_to_skip)
                            {
                                j++;
                                if(opt_program[j].type == LBRACKET) { brackets_to_skip++; }
                                if(opt_program[j].type == RBRACKET) { brackets_to_skip--; }
                                if(brackets_to_skip == 0) { ipt = j; break; }
                            }
                        } else
                        {
                            bracket_insn_pointers.push(ipt-1);
                        }
                        break;
                    case RBRACKET:
                        ipt = bracket_insn_pointers.top();
                        bracket_insn_pointers.pop();
                        break;
                    default:
                        break;
                }
            }
        }
        else // safe mode
        {
            for(size_t ipt = 0; ipt < opt_program.size(); ipt++)
            {
                switch(opt_program[ipt].type)
                {
                    case CLEAR:
                        mem_tape[data_pointer] = 0; 
                        break;
                    case PLUS:
                        mem_tape[data_pointer] += opt_program[ipt].count;
                        break;
                    case MINUS:
                        mem_tape[data_pointer] -= opt_program[ipt].count;
                        break;
                    case LEFT:
                        data_pointer -= opt_program[ipt].count;
                        if(data_pointer >= mem_tape.size()) { data_pointer = 0; }
                        break;
                    case RIGHT:
                        data_pointer += opt_program[ipt].count;
                        if(data_pointer >= mem_tape.size()) { mem_tape.resize(mem_tape.size() + (mem_tape.size() / 2), 0); }
                        break;
                    case DOT:
                        for(int j = opt_program[ipt].count; j > 0; j--)
                            fmt::printf("%lc", mem_tape[data_pointer]);
                        break;
                    case COMMA:
                        fmt::print("\x1b\7");
                        mem_tape[data_pointer] = getchar();
                        fflush(stdin);
                        fmt::print("\x1b\10\x1b[1C\x1b[1A");
                        break;
                    case LBRACKET:
                        if(mem_tape[data_pointer] == 0)
                        {
                            brackets_to_skip = 1;
                            size_t j = insn_pointer;
                            while(brackets_to_skip)
                            {
                                j++;
                                if(program[j] == '[') { brackets_to_skip++; }
                                if(program[j] == ']') { brackets_to_skip--; }
                                if(brackets_to_skip == 0) { insn_pointer = j; break; }
                            }
                        } else
                        {
                            bracket_insn_pointers.push(insn_pointer-1);
                        }
                        break;
                    case RBRACKET:
                        insn_pointer = bracket_insn_pointers.top();
                        bracket_insn_pointers.pop();
                        break;
                    default:
                        break;
                }
            }
        }
    }   

    if(counter)
    {
        fmt::print("the program finished in {0} instructions and {1}\n", insn_num,
            overflow?"overflowed":"didn't overflow");
    }

    fmt::print("\n");
    return 0;
}

/*
enum INSN_TYPE
{
    // the default
    COMMENT = 0,
    // the 8 brainfuck builtins
    RIGHT, LEFT,
    PLUS, MINUS,
    LBRACKET, RBRACKET,
    DOT, COMMA,
    // and the specials
    CLEAR
};

struct insn_t
{
    uint16_t count = 0;
    INSN_TYPE type = COMMENT;
};
*/