#include "interpret_safe_unoptimised.hpp"

void interpret_raw_safe(
    const std::string& program, 
    std::vector<uint16_t>& mem_tape,
    std::stack<size_t>& bracket_insn_pointers,
    bool& overflow,
    const bool counter,
    int64_t& insn_num
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
    return;
}