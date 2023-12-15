#include "transpiler.hpp"

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