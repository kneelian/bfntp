#include "interpret_unsafe_unoptimised.hpp"

void interpret_raw_unsafe(
    const std::string& program, 
    std::vector<uint16_t>& mem_tape,
    std::stack<size_t>& bracket_insn_pointers
        //size_t data_pointer, 
        //size_t insn_pointer,
    )
{
	size_t data_pointer = 0;
	size_t insn_pointer = 0;
	size_t brackets_to_skip = 0;
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
            /*
                fmt::print("\x1b\7");
                mem_tape[data_pointer] = getchar();
                fflush(stdin);
                fmt::print("\x1b\10\x1b[1C\x1b[1A");
            */
                mem_tape[data_pointer] = getchar();
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
            default:
          		break;
        }
    }
    return;
}