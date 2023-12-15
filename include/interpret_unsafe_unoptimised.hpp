#pragma once

#include <string>
#include <vector>
#include <stack>
#include "types.hpp"

#include "fmt/core.h"
#include "fmt/printf.h"
#include "fmt/color.h"

void interpret_raw_unsafe(
    const std::string&, 
    std::vector<uint16_t>&,
    std::stack<size_t>&
        //size_t data_pointer, 
        //size_t insn_pointer,
    );