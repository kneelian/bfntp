#include "optimiser.hpp"

void optim(std::string& program, std::vector<insn_t>& opt_program)
{

    program = 
        std::regex_replace
        (program, std::regex("[ A-Za-z0-9\n]{1,}"), std::string());

    replace_all(program, "[-]", "@");
    replace_all(program, "[+]", "@");

    if(!opt_program.empty()) { opt_program.clear(); }

    insn_t temp = { 0, COMMENT };

    for(int i = 0; i < program.size(); i++)
    {
        int count = 1;
        while(program[i] == program[i+1])
        {
            if
            (
                (program[i] == '[') ||
                (program[i] == ']') ||
                (program[i] == ',') ||
                (program[i] == '\0')
            )
            { break; }
            count++;
            i++;
        }
        temp.count = count;
        switch(program[i])
        {
            case '[':
                temp.type = LBRACKET;
                break;
            case ']':
                temp.type = RBRACKET;
                break;
            case '>':
                temp.type = RIGHT;
                break;
            case '<':
                temp.type = LEFT;
                break;
            case '+':
                temp.type = PLUS;
                break;
            case '-':
                temp.type = MINUS;
                break;
            case '.':
                temp.type = DOT;
                break;
            case ',':
                temp.type = COMMA;
                break;
            case '@':
                temp.type = CLEAR;
                break;
            default:
                temp.type = COMMENT;
                break;
        }
        if(temp.type == COMMENT) { continue; }

        opt_program.push_back(temp);
    }
    return;
}