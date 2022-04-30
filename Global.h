#pragma once
#include <iostream>
#include <cstdio>
#include <string>
#include <string_view>
#include "Headers/Parallel.h"
#include "Headers/List.h"
using std::string;
using std::cerr;
using std::clog;
using std::cout;
using std::endl;
INLINE_VAR static constexpr short DrawMap[] = {
    0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24, 26, 28, 29,
    31, 33, 35, 36, 38, 40, 41, 43, 45, 47, 48, 50, 52, 53, 55, 57,
    59, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83, 84,
    86, 88, 89, 91, 93, 95, 96, 98, 100, 101, 103, 105, 107, 108,
    110, 112, 113, 115, 117, 119, 120, 122, 124, 125, 127, 1, 3,
    6, 8, 10, 13, 15, 18, 20, 22, 25, 27, 30, 32, 34, 37, 39, 42, 44,
    46, 49, 51, 54, 56, 58, 61, 63, 66, 68, 70, 73, 75, 78, 80, 82,
    85, 87, 90, 92, 94, 97, 99, 102, 104, 106, 109, 111, 114, 116,
    118, 121, 123, 126
};
INLINE_VAR static constexpr short GenKeyX[] = {
    0, 12, 18, 33, 36, 54, 66, 72, 85, 90, 105, 108
};
INLINE_VAR static constexpr unsigned int Color[]= {
  0xFFFF33CC, 0xFF4EFF33, 0xFF9933FF, 0xFFE7FF33, 0xFF3366FF, 0xFFFF7E33, 0xFF33FF66, 0xFFFF3381, 0xFF33E1E1, 0xFFE433E1, 0xFF99E133, 0xFF4B33E1, 0xFFFFCC33, 0xFF33B4FF, 0xFFFF3333, 0xFF33FFB1, 
};