#pragma once

#include <string>
#include <vector>
#include <stack>
#include "types.hpp"

#include "fmt/core.h"
#include "fmt/printf.h"
#include "fmt/color.h"

void interpret_opt_safe(
    const std::vector<insn_t>&, 
    std::vector<uint16_t>&,
    std::stack<size_t>&
    );