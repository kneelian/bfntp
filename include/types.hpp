#pragma once

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