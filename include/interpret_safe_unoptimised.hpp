#pragma once

#include <string>
#include <vector>
#include <stack>
#include "types.hpp"

#include "fmt/core.h"
#include "fmt/printf.h"
#include "fmt/color.h"

void interpret_raw_safe(
    const std::string&, 
    std::vector<uint16_t>&,
    std::stack<size_t>&,
    bool&,
    const bool,
    int64_t&
    );