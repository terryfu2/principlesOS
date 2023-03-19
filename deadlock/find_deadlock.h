
#pragma once
#include <string>
#include <vector>

struct Result {
    int index;
    std::vector<std::string> procs;
};

Result find_deadlock(const std::vector<std::string> & edges);
