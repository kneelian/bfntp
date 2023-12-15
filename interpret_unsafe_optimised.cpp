#include "interpret_unsafe_optimised.hpp"

void interpret_opt_unsafe(
    const std::vector<insn_t>& opt_program, 
    std::vector<uint16_t>& mem_tape,
    std::stack<size_t>& bracket_insn_pointers
    )
{
    size_t data_pointer = 0;
    size_t insn_pointer = 0;
    size_t brackets_to_skip = 0;

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
            /*
                fmt::print("\x1b\7");
                mem_tape[data_pointer] = getchar();
                fflush(stdin);
                fmt::print("\x1b\10\x1b[1C\x1b[1A");
            */
                mem_tape[data_pointer] = getchar();
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
    return;
}