#include <fstream>
#include <sstream>
#include <stack>
#include <vector>
#include <string>

#include <regex>

#include "fmt/core.h"
#include "fmt/printf.h"
#include "fmt/color.h"

using namespace std;

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
            fmt::print("{0} v0.1a usage notes\n", 
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
            fmt::print("                   {0}: negates the -c flag because some optimisations",
                fmt::styled("note", fmt::emphasis::bold)
            );
            fmt::print("                   invalidate op counts.\n");
            fmt::print("    -t: transpile: instead of executing the brainfuck code directly, it\n");
            fmt::print("                   will print a transpiled CPP equivalent of the code and\n");
            fmt::print("                   exit. You can then compile the output using a normally\n");
            fmt::print("                   conforming CPP compiler");
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

void replace_all(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

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

void transpiler(std::string& program, bool opt, bool safe)
{
    fmt::print("#include <cstdio>");
    fmt::print("\n#include <string>");
    fmt::print("\n#include <vector>");
    fmt::print("\n// transpiled using bftpl © kneelian (kneelian.github.com)");
    fmt::print("\nint main() {{q");// fmt doesnt like { }
    fmt::print("\n\tstd::vector<uint16_t> d;");
    fmt::print("\n\td.resize({0});", 8 * 1024);

    fmt::print("\n\tsize_t dp = 0;");

    if(opt)
    {
        std::vector<insn_t> opt_program;
        optim(program, opt_program);

        for(insn_t insn : opt_program)
        {
            switch(insn.type)
            {
                case PLUS:
                    fmt::print("\n\td[dp] += {0};", insn.count);
                    break;
                case MINUS:
                    fmt::print("\n\td[dp] -= {0};", insn.count);
                    break;
                case RIGHT:
                    fmt::print("\n\tdp += {0};", insn.count);
                    if(safe)
                        fmt::printf("\n\tif(dp + 1 == d.size()) { d.resize(d.size() + (d.size() / 2)); }");
                    break;
                case LEFT:
                    fmt::print("\n\tdp -= {0};", insn.count);
                    if(safe)
                        fmt::print("\n\tdp = dp? (dp-1): 0;");
                    break;
                case DOT:
                    fmt::print("\n\tfor(int j = {0}; j > 0; j--)", insn.count);
                    fmt::print("\n\t\tprintf(\"%lc\", wchar_t(d[dp]));");
                    break;
                case COMMA:
                    fmt::print("\n\td[dp] = getchar();");
                    break;
                case LBRACKET:
                    fmt::printf("\n\twhile(d[dp]){");
                    break;
                case RBRACKET:
                    fmt::printf("\n\t}");
                    break;
                case CLEAR:
                    fmt::print("\n\td[dp] = 0;");
                    break;
                default:
                    break;
            }
        }
    }
    else
    {
        for(char insn : program)
        {
            switch(insn)
            {
                case '+':
                    fmt::print("\n\td[dp]++;");
                    break;
                case '-':
                    fmt::print("\n\td[dp]--;");
                    break;
                case '>':
                        fmt::print("\n\tdp++;");
                    if(safe)
                        fmt::printf("\n\tif(dp + 1 == d.size()) { d.resize(d.size() + (d.size() / 2)); }");
                    break;
                case '<':
                    fmt::print("\n\tdp--;");
                    if(safe)
                        fmt::print("\n\tdp = dp? (dp-1): 0;");
                    break;
                case '.':
                    fmt::print("\n\tprintf(\"%lc\", wchar_t(d[dp]));");
                    break;
                case ',':
                    fmt::print("\n\td[dp] = getchar();");
                    break;
                case '[':
                    fmt::printf("\n\twhile(d[dp]){"); //
                    break;
                case ']':
                    fmt::printf("\n\t}");
                    break;
                default:
                    break;
            }
        }       
    }

    fmt::printf("\n\treturn 0;\n}");
    fmt::print("\n");
    return;
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