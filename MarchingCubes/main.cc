// Source: https://www.cs.toronto.edu/~jacobson/seminar/lorenson-and-cline-1987.pdf

#include <fstream>
#include <iostream>
#include <cmath> // std::sqrt(), NAN


// At most 4 sets of triangles (3), '-1' is early eof
// Useful interactive visual aid that helped a lot: https://skfb.ly/6SZCy
// Same link: https://sketchfab.com/3d-models/15-cases-marching-cubes-representation-78927bfd96694fccbc064bb5fe820f3d
constexpr char TRIANGLE_TABLE[256][12] = {
// 0 - 15
    {-1,-1,-1,  -1,-1,-1,  -1,-1,-1,  -1,-1,-1}, // 0000 0000
    { 0, 3, 8,                              -1}, // 0000 0001
    { 0, 9, 1,                              -1}, // 0000 0010
    { 1, 3, 8,   1, 8, 9,                   -1}, // 0000 0011
    { 1,10, 2,                              -1}, // 0000 0100
    { 0, 3, 8,   1,10, 2,                   -1}, // 0000 0101
    { 2, 9,10,   0, 9, 2,                   -1}, // 0000 0110
    { 2, 3, 8,   2, 8,10,   8, 9,10,        -1}, // 0000 0111
    { 2,11, 3,                              -1}, // 0000 1000
    { 0,11, 8,   0, 2,11,                   -1}, // 0000 1001
    { 2,11, 3,   0, 9, 1,                   -1}, // 0000 1010
    { 1, 2,11,   1,11, 9,   8, 9,11,        -1}, // 0000 1011
    { 3,10,11,   1,10, 3,                   -1}, // 0000 1100
    { 0, 1,10,   0,10, 8,   8,10,11,        -1}, // 0000 1101
    { 0, 9, 3,   3, 9,11,   9,10,11,        -1}, // 0000 1110
    { 8, 9,11,   9,10,11,                   -1}, // 0000 1111

// 16 - 31
    { 4, 8, 7,                              -1}, // 0001 0000
    { 3, 7, 4,   0, 3, 4,                   -1}, // 0001 0001
    { 4, 8, 7,   0, 9, 1,                   -1}, // 0001 0010
    { 1, 4, 9,   1, 7, 4,   1, 3, 7,        -1}, // 0001 0011
    { 4, 8, 7,   1,10, 2,                   -1}, // 0001 0100
    { 3, 7, 4,   0, 3, 4,   1,10, 2,        -1}, // 0001 0101
    { 2, 9,10,   0, 9, 2,   4, 8, 7,        -1}, // 0001 0110
    { 2, 3,10,   3, 4,10,   3, 7, 4,   4, 9,10}, // 0001 0111
    { 4, 8, 7,   2,11, 3,                   -1}, // 0001 1000
    { 4,11, 7,   2,11, 4,   0, 2, 4,        -1}, // 0001 1001
    { 0, 9, 1,   4, 8, 7,   2,11, 3,        -1}, // 0001 1010
    { 4,11, 7,   2,11, 4,   2, 4, 9,   1, 2, 9}, // 0001 1011
    { 1,10, 3,   3,10,11,   4, 8, 7,        -1}, // 0001 1100
    { 1,10,11,   4,11, 7,   1,11, 4,   0, 1, 4}, // 0001 1101
    { 4, 8, 7,   0, 9, 3,   3, 9,11,   9,10,11}, // 0001 1110
    { 4,11, 7,   4, 9,11,   9,10,11,        -1}, // 0001 1111

// 32 - 47
    { 4, 5, 9,                              -1}, // 0010 0000
    { 4, 5, 9,   0, 3, 8,                   -1}, // 0010 0001
    { 0, 4, 5,   0, 5, 1,                   -1}, // 0010 0010
    { 4, 5, 8,   3, 8, 5,   1, 3, 5,        -1}, // 0010 0011
    { 1,10, 2,   4, 5, 9,                   -1}, // 0010 0100
    { 4, 5, 9,   0, 3, 8,   1,10, 2,        -1}, // 0010 0101
    { 2, 5,10,   2, 4, 5,   0, 4, 2,        -1}, // 0010 0110
    { 2, 5,10,   2, 4, 5,   2, 3, 4,   3, 8, 4}, // 0010 0111
    { 4, 5, 9,   2,11, 3,                   -1}, // 0010 1000
    { 0, 2,11,   0,11, 8,   4, 5, 9,        -1}, // 0010 1001
    { 0, 4, 5,   0, 5, 1,   2,11, 3,        -1}, // 0010 1010
    { 1, 4, 5,   1, 2,11,   1,11, 4,   4,11, 8}, // 0010 1011
    { 3,10,11,   1,10, 3,   4, 5, 9,        -1}, // 0010 1100
    { 4, 5, 9,   0, 1,10,   0,10, 8,   8,10,11}, // 0010 1101
    { 0,11, 3,   0, 5,11,   0, 4, 5,   5,10,11}, // 0010 1110
    { 4, 5, 8,   5,10, 8,   8,10,11,        -1}, // 0010 1111

// 48 - 63
    { 7, 9, 8,   5, 9, 7,                   -1}, // 0011 0000
    { 0, 3, 9,   3, 5, 9,   3, 7, 5,        -1}, // 0011 0001
    { 0, 8, 7,   0, 7, 1,   1, 7, 5,        -1}, // 0011 0010
    { 3, 7, 5,   1, 3, 5,                   -1}, // 0011 0011
    { 7, 9, 8,   5, 9, 7,   1,10, 2,        -1}, // 0011 0100
    { 1,10, 2,   0, 3, 9,   3, 5, 9,   3, 7, 5}, // 0011 0101
    { 5, 8, 7,   2, 5,10,   2, 8, 5,   0, 8, 2}, // 0011 0110
    { 2, 5,10,   2, 3, 5,   3, 7, 5,        -1}, // 0011 0111
    { 5, 9, 7,   7, 9, 8,   2,11, 3,        -1}, // 0011 1000
    { 2,11, 7,   2, 7, 9,   5, 9, 7,   0, 2, 9}, // 0011 1001
    { 2,11, 3,   0, 8, 7,   0, 7, 1,   1, 7, 5}, // 0011 1010
    { 1, 2,11,   1,11, 7,   1, 7, 5,        -1}, // 0011 1011
    { 5, 8, 7,   5, 9, 8,   3,10,11,   1,10, 3}, // 0011 1100
    { 5,11, 7,   5,10,11,   0, 1, 9,        -1}, // 0011 1101
    { 5,10,11,   5,11, 7,   0, 8, 3,        -1}, // 0011 1110
    { 5,11, 7,   5,10,11,                   -1}, // 0011 1111

// 64 - 79
    { 5, 6,10,                              -1}, // 0100 0000
    { 0, 3, 8,   5, 6,10,                   -1}, // 0100 0001
    { 5, 6,10,   0, 9, 1,                   -1}, // 0100 0010
    { 1, 3, 8,   1, 8, 9,   5, 6,10,        -1}, // 0100 0011
    { 1, 5, 6,   1, 6, 2,                   -1}, // 0100 0100
    { 1, 5, 6,   1, 6, 2,   0, 3, 8,        -1}, // 0100 0101
    { 5, 6, 9,   0, 9, 6,   0, 6, 2,        -1}, // 0100 0110
    { 2, 5, 6,   2, 3, 8,   2, 8, 5,   5, 8, 9}, // 0100 0111
    { 2,11, 3,   5, 6,10,                   -1}, // 0100 1000
    { 0,11, 8,   0, 2,11,   5, 6,10,        -1}, // 0100 1001
    { 0, 9, 1,   2,11, 3,   5, 6,10,        -1}, // 0100 1010
    { 5, 6,10,   1, 2,11,   1,11, 9,   8, 9,11}, // 0100 1011
    { 3, 6,11,   3, 5, 6,   1, 5, 3,        -1}, // 0100 1100
    { 0, 1, 8,   1, 6, 8,   1, 5, 6,   6,11, 8}, // 0100 1101
    { 3, 6,11,   3, 5, 6,   0, 5, 3,   0, 9, 5}, // 0100 1110
    { 5, 6, 9,   6,11, 9,   8, 9,11,        -1}, // 0100 1111

// 80 - 95
    { 5, 6,10,   4, 8, 7,                   -1}, // 0101 0000
    { 0, 3, 4,   3, 7, 4,   5, 6,10,        -1}, // 0101 0001
    { 5, 6,10,   4, 8, 7,   0, 9, 1,        -1}, // 0101 0010
    { 5, 6,10,   1, 4, 9,   1, 7, 4,   1, 3, 7}, // 0101 0011
    { 1, 6, 2,   1, 5, 6,   4, 8, 7,        -1}, // 0101 0100
    { 0, 3, 7,   0, 7, 4,   1, 6, 2,   1, 5, 6}, // 0101 0101
    { 4, 8, 7,   5, 6, 9,   0, 9, 6,   0, 6, 2}, // 0101 0110
    { 2, 3, 7,   2, 7, 6,   4, 9, 5,        -1}, // 0101 0111
    { 4, 8, 7,   5, 6,10,   2,11, 3,        -1}, // 0101 1000
    { 5, 6,10,   4,11, 7,   2,11, 4,   0, 2, 4}, // 0101 1001
    { 4, 8, 7,   5, 6,10,   2,11, 3,   0, 9, 1}, // 0101 1010
    { 1, 2,10,   6,11, 7,   4, 9, 5,        -1}, // 0101 1011
    { 4, 8, 7,   3, 6,11,   3, 5, 6,   1, 5, 3}, // 0101 1100
    { 0, 1, 5,   0, 5, 4,   6,11, 7,        -1}, // 0101 1101
    { 4, 9, 5,   6,11, 7,   0, 8, 3,        -1}, // 0101 1110
    { 6,11, 7,   4, 9, 5,                   -1}, // 0101 1111

// 96 - 111
    { 4, 6,10,   4,10, 9,                   -1}, // 0110 0000
    { 4, 6,10,   4,10, 9,   0, 3, 8,        -1}, // 0110 0001
    { 0,10, 1,   0, 6,10,   0, 4, 6,        -1}, // 0110 0010
    { 1, 6,10,   1, 8, 6,   1, 3, 8,   4, 6, 8}, // 0110 0011
    { 1, 9, 4,   1, 4, 2,   2, 4, 6,        -1}, // 0110 0100
    { 0, 3, 8,   1, 9, 4,   1, 4, 2,   2, 4, 6}, // 0110 0101
    { 2, 4, 6,   0, 4, 2,                   -1}, // 0110 0110
    { 2, 3, 8,   2, 8, 4,   2, 4, 6,        -1}, // 0110 0111
    { 4,10, 9,   4, 6,10,   2,11, 3,        -1}, // 0110 1000
    { 2,11, 8,   0, 2, 8,   4, 6,10,   4,10, 9}, // 0110 1001
    { 2,11, 3,   0,10, 1,   0, 6,10,   0, 4, 6}, // 0110 1010
    { 4, 6, 8,   6,11, 8,   1, 2,10,        -1}, // 0110 1011
    { 4, 6, 9,   3, 6,11,   3, 9, 6,   1, 9, 3}, // 0110 1100
    { 6,11, 8,   4, 6, 8,   0, 1, 9,        -1}, // 0110 1101
    { 3, 6,11,   0, 6, 3,   0, 4, 6,        -1}, // 0110 1110
    { 6,11, 8,   4, 6, 8,                   -1}, // 0110 1111

// 112 - 127
    { 6,10, 7,   7,10, 8,   8,10, 9,        -1}, // 0111 0000
    { 0, 3, 7,   6,10, 7,   0, 7,10,   0,10, 9}, // 0111 0001
    { 6,10, 7,   7,10, 8,   1, 8,10,   0, 8, 1}, // 0111 0010
    { 6,10, 7,   1, 7,10,   1, 3, 7,        -1}, // 0111 0011
    { 1, 9, 2,   2, 9, 7,   7, 9, 8,   2, 7, 6}, // 0111 0100
    { 2, 7, 6,   2, 3, 7,   0, 1, 9,        -1}, // 0111 0101
    { 0, 8, 7,   0, 7, 6,   0, 6, 2,        -1}, // 0111 0110
    { 2, 3, 7,   2, 7, 6,                   -1}, // 0111 0111
    { 2,11, 3,   6,10, 7,   7,10, 8,   8,10, 9}, // 0111 1000
    { 0, 2, 9,   2,10, 9,   6,11, 7,        -1}, // 0111 1001
    { 0, 8, 3,   6,11, 7,   1, 2,10,        -1}, // 0111 1010
    { 6,11, 7,   1, 2,10,                   -1}, // 0111 1011
    { 1, 9, 8,   1, 8, 3,   6,11, 7,        -1}, // 0111 1100
    { 6,11, 7,   0, 1, 9,                   -1}, // 0111 1101
    { 6,11, 7,   0, 8, 3,                   -1}, // 0111 1110
    { 6,11, 7,                              -1}, // 0111 1111

// 128 - 143
    { 6, 7,11,                              -1}, // 1000 0000
    { 6, 7,11,   0, 3, 8,                   -1}, // 1000 0001
    { 6, 7,11,   0, 9, 1,                   -1}, // 1000 0010
    { 1, 8, 9,   1, 3, 8,   6, 7,11,        -1}, // 1000 0011
    { 6, 7,11,   1,10, 2,                   -1}, // 1000 0100
    { 0, 3, 8,   6, 7,11,   1,10, 2,        -1}, // 1000 0101
    { 0, 9, 2,   2, 9,10,   6, 7,11,        -1}, // 1000 0110
    { 6, 7,11,   2, 3, 8,   2, 8,10,   8, 9,10}, // 1000 0111
    { 2, 7, 3,   2, 6, 7,                   -1}, // 1000 1000
    { 0, 7, 8,   0, 6, 7,   0, 2, 6,        -1}, // 1000 1001
    { 2, 6, 7,   2, 7, 3,   0, 9, 1,        -1}, // 1000 1010
    { 6, 7, 8,   1, 6, 8,   1, 8, 9,   1, 2, 6}, // 1000 1011
    { 6, 7,10,   1,10, 7,   1, 7, 3,        -1}, // 1000 1100
    { 0, 7, 8,   0, 6, 7,   0, 1, 6,   1,10, 6}, // 1000 1101
    { 3, 6, 7,   0, 9, 3,   3, 9, 6,   6, 9,10}, // 1000 1110
    { 6, 7,10,   7, 8,10,   8, 9,10,        -1}, // 1000 1111

// 144 - 159
    { 6, 8,11,   4, 8, 6,                   -1}, // 1001 0000
    { 3,11, 6,   0, 3, 6,   0, 6, 4,        -1}, // 1001 0001
    { 6, 8,11,   4, 8, 6,   0, 9, 1,        -1}, // 1001 0010
    { 1, 3, 9,   3,11, 6,   3, 6, 9,   4, 9, 6}, // 1001 0011
    { 4, 8, 6,   6, 8,11,   1,10, 2,        -1}, // 1001 0100
    { 1,10, 2,   3,11, 6,   0, 3, 6,   0, 6, 4}, // 1001 0101
    { 4,11, 6,   4, 8,11,   2, 9,10,   0, 9, 2}, // 1001 0110
    { 4, 9,10,   4,10, 6,   2, 3,11,        -1}, // 1001 0111
    { 2, 8, 3,   2, 4, 8,   2, 6, 4,        -1}, // 1001 1000
    { 0, 6, 4,   0, 2, 6,                   -1}, // 1001 1001
    { 0, 9, 1,   2, 8, 3,   2, 4, 8,   2, 6, 4}, // 1001 1010
    { 1, 4, 9,   1, 2, 4,   2, 6, 4,        -1}, // 1001 1011
    { 3, 4, 8,   3,10, 4,   1,10, 3,   4,10, 6}, // 1001 1100
    { 0, 1,10,   0,10, 6,   0, 6, 4,        -1}, // 1001 1101
    { 4,10, 6,   4, 9,10,   0, 8, 3,        -1}, // 1001 1110
    { 4,10, 6,   4, 9,10,                   -1}, // 1001 1111

// 160 - 175
    { 6, 7,11,   4, 5, 9,                   -1}, // 1010 0000
    { 4, 5, 9,   6, 7,11,   0, 3, 8,        -1}, // 1010 0001
    { 0, 5, 1,   0, 4, 5,   6, 7,11,        -1}, // 1010 0010
    { 6, 7,11,   4, 5, 8,   3, 8, 5,   1, 3, 5}, // 1010 0011
    { 1,10, 2,   6, 7,11,   4, 5, 9,        -1}, // 1010 0100
    { 0, 3, 8,   4, 5, 9,   6, 7,11,   1,10, 2}, // 1010 0101
    { 6, 7,11,   2, 5,10,   2, 4, 5,   0, 4, 2}, // 1010 0110
    { 4, 7, 8,   5,10, 6,   2, 3,11,        -1}, // 1010 0111
    { 2, 7, 3,   2, 6, 7,   4, 5, 9,        -1}, // 1010 1000
    { 4, 5, 9,   0, 7, 8,   0, 6, 7,   0, 2, 6}, // 1010 1001
    { 2, 6, 3,   3, 6, 7,   0, 5, 1,   0, 4, 5}, // 1010 1010
    { 1, 2, 6,   1, 6, 5,   4, 7, 8,        -1}, // 1010 1011
    { 4, 5, 9,   6, 7,10,   1,10, 7,   1, 7, 3}, // 1010 1100
    { 5,10, 6,   4, 7, 8,   0, 1, 9,        -1}, // 1010 1101
    { 0, 4, 3,   3, 4, 7,   5,10, 6,        -1}, // 1010 1110
    { 5,10, 6,   4, 7, 8,                   -1}, // 1010 1111

// 176 - 191
    { 5, 9, 6,   6, 9,11,   8,11, 9,        -1}, // 1011 0000
    { 3,11, 6,   0, 3, 6,   0, 6, 5,   0, 5, 9}, // 1011 0001
    { 5,11, 6,   0,11, 5,   0, 5, 1,   0, 8,11}, // 1011 0010
    { 3,11, 6,   3, 6, 5,   1, 3, 5,        -1}, // 1011 0011
    { 1,10, 2,   5, 9, 6,   6, 9,11,   8,11, 9}, // 1011 0100
    { 0, 1, 9,   2, 3,11,   5,10, 6,        -1}, // 1011 0101
    { 0, 8,11,   0,11, 2,   5,10, 6,        -1}, // 1011 0110
    { 2, 3,11,   5,10, 6,                   -1}, // 1011 0111
    { 5, 9, 8,   2, 8, 3,   2, 5, 8,   2, 6, 5}, // 1011 1000
    { 5, 9, 6,   0, 6, 9,   0, 2, 6,        -1}, // 1011 1001
    { 1, 6, 5,   1, 2, 6,   0, 8, 3,        -1}, // 1011 1010
    { 1, 6, 5,   1, 2, 6,                   -1}, // 1011 1011
    { 1, 8, 3,   1, 9, 8,   5,10, 6,        -1}, // 1011 1100
    { 5,10, 6,   0, 1, 9,                   -1}, // 1011 1101
    { 0, 8, 3,   5,10, 6,                   -1}, // 1011 1110
    { 5,10, 6,                              -1}, // 1011 1111

// 192 - 207
    { 5, 7,11,   5,11,10,                   -1}, // 1100 0000
    { 5,11,10,   5, 7,11,   0, 3, 8,        -1}, // 1100 0001
    { 5, 7,11,   5,11,10,   0, 9, 1,        -1}, // 1100 0010
    { 7,11,10,   5, 7,10,   1, 3, 8,   1, 8, 9}, // 1100 0011
    { 1,11, 2,   1, 7,11,   1, 5, 7,        -1}, // 1100 0100
    { 0, 3, 8,   1,11, 2,   1, 7,11,   1, 5, 7}, // 1100 0101
    { 2, 7,11,   2, 9, 7,   0, 9, 2,   5, 7, 9}, // 1100 0110
    { 5, 7, 9,   7, 8, 9,   2, 3,11,        -1}, // 1100 0111
    { 2,10, 5,   2, 5, 3,   3, 5, 7,        -1}, // 1100 1000
    { 5, 7,10,   0, 7, 8,   0,10, 7,   0, 2,10}, // 1100 1001
    { 0, 9, 1,   2,10, 5,   2, 5, 3,   3, 5, 7}, // 1100 1010
    { 7, 8, 9,   5, 7, 9,   1, 2,10,        -1}, // 1100 1011
    { 1, 7, 3,   1, 5, 7,                   -1}, // 1100 1100
    { 0, 7, 8,   0, 1, 7,   1, 5, 7,        -1}, // 1100 1101
    { 0, 9, 3,   3, 9, 5,   3, 5, 7,        -1}, // 1100 1110
    { 7, 8, 9,   5, 7, 9,                   -1}, // 1100 1111

// 208 - 223
    { 4, 8, 5,   5, 8,10,   8,11,10,        -1}, // 1101 0000
    { 0, 3,11,   0,11, 5,   5,11,10,   0, 5, 4}, // 1101 0001
    { 0, 9, 1,   4, 8, 5,   5, 8,10,   8,11,10}, // 1101 0010
    { 3,11,10,   1, 3,10,   4, 9, 5,        -1}, // 1101 0011
    { 4, 8,11,   1,11, 2,   1, 4,11,   1, 5, 4}, // 1101 0100
    { 0, 5, 4,   0, 1, 5,   2, 3,11,        -1}, // 1101 0101
    { 0,11, 2,   0, 8,11,   4, 9, 5,        -1}, // 1101 0110
    { 4, 9, 5,   2, 3,11,                   -1}, // 1101 0111
    { 2, 8, 3,   2, 4, 8,   2,10, 4,   4,10, 5}, // 1101 1000
    { 2,10, 5,   2, 5, 4,   0, 2, 4,        -1}, // 1101 1001
    { 4, 9, 5,   0, 8, 3,   1, 2,10,        -1}, // 1101 1010
    { 1, 2,10,   4, 9, 5,                   -1}, // 1101 1011
    { 4, 8, 5,   3, 5, 8,   1, 5, 3,        -1}, // 1101 1100
    { 0, 5, 4,   0, 1, 5,                   -1}, // 1101 1101
    { 4, 9, 5,   0, 8, 3,                   -1}, // 1101 1110
    { 4, 9, 5,                              -1}, // 1101 1111

// 224 - 239
    { 4, 7,11,   4,11, 9,   9,11,10,        -1}, // 1110 0000
    { 0, 3, 8,   4, 7,11,   4,11, 9,   9,11,10}, // 1110 0001
    { 7,11,10,   0,10, 1,   0, 7,10,   0, 4, 7}, // 1110 0010
    { 1, 3,10,   3,11,10,   4, 7, 8,        -1}, // 1110 0011
    { 4, 7,11,   4,11, 9,   2, 9,11,   1, 9, 2}, // 1110 0100
    { 0, 1, 9,   4, 7, 8,   2, 3,11,        -1}, // 1110 0101
    { 4, 7,11,   2, 4,11,   0, 4, 2,        -1}, // 1110 0110
    { 4, 7, 8,   2, 3,11,                   -1}, // 1110 0111
    { 2,10, 3,   3,10, 4,   4,10, 9,   3, 4, 7}, // 1110 1000
    { 2,10, 9,   0, 2, 9,   4, 7, 8,        -1}, // 1110 1001
    { 3, 4, 7,   0, 4, 3,   1, 2,10,        -1}, // 1110 1010
    { 4, 7, 8,   1, 2,10,                   -1}, // 1110 1011
    { 1, 9, 4,   1, 4, 7,   1, 7, 3,        -1}, // 1110 1100
    { 4, 7, 8,   0, 1, 9,                   -1}, // 1110 1101
    { 3, 4, 7,   0, 4, 3,                   -1}, // 1110 1110
    { 4, 7, 8,                              -1}, // 1110 1111

// 240 - 255
    { 8,11,10,   8,10, 9,                   -1}, // 1111 0000
    { 0, 3, 9,   3,11, 9,   9,11,10,        -1}, // 1111 0001
    { 0,10, 1,   0, 8,10,   8,11,10,        -1}, // 1111 0010
    { 3,11,10,   1, 3,10,                   -1}, // 1111 0011
    { 1,11, 2,   1, 9,11,   8,11, 9,        -1}, // 1111 0100
    { 2, 3,11,   0, 1, 9,                   -1}, // 1111 0101
    { 0, 8,11,   0,11, 2,                   -1}, // 1111 0110
    { 2, 3,11,                              -1}, // 1111 0111
    { 2, 8, 3,   2,10, 8,   8,10, 9,        -1}, // 1111 1000
    { 2,10, 9,   0, 2, 9,                   -1}, // 1111 1001
    { 0, 8, 3,   1, 2,10,                   -1}, // 1111 1010
    { 1, 2,10,                              -1}, // 1111 1011
    { 1, 8, 3,   1, 9, 8,                   -1}, // 1111 1100
    { 0, 1, 9,                              -1}, // 1111 1101
    { 0, 8, 3,                              -1}, // 1111 1110
    {                                       -1}, // 1111 1111
};


/**
 *  @brief  Helper funtion for normals
 *  @param  v[3]    xyz indicies (ideally, the edge's corner)
 *  @param  dir     direction flag for which axis is being approximated (0 for X, 1 for Y, and 2 for Z)
 *  @param  data    3 dimensional table
 *  @param  PLANES  max Z data size
 *  @param  ROWS    max Y data size
 *  @param  COLS    max X data size
 *  @return @c gradient approximated at that data point
 *
 *  This is a function that takes the data indices of a voxel (usually the endpoint corner of an edge)
 *  and returns the approximated gradient at that data point (for surface normals)
 */
float calculate_gradient(const size_t v[3], char dir, const float* data, size_t PLANES, size_t ROWS, size_t COLS) {
    // Get left and right index positions on the correct axis
    size_t left  = ROWS*COLS*(v[2] - (dir==2))
                 +      COLS*(v[1] - (dir==1))
                 +           (v[0] - (dir==0));
    size_t right = ROWS*COLS*(v[2] + (dir==2))
                 +      COLS*(v[1] + (dir==1))
                 +           (v[0] + (dir==0));

    // Handle edge-cases (clamp)
    float length = 2; // Assuming cube length is 1
    const size_t pos = v[int(dir)];
    if (pos == 0 || pos >= ((dir==2) ? (PLANES-1) : (dir==1) ? (ROWS-1) : (COLS-1))) {
        // Clamp out of bounds edge past border to center
        length = 1; // Length is halfed
        const size_t center = ROWS*COLS*v[2] + COLS*v[1] + v[0];
        if (pos == 0) // (also, this should be the same as '<= 0' if v is size_t, right?)
            left = center;
        else
            right = center;
    }

    return (data[left] - data[right]) / length;
}

int main(int argc, const char * argv[]) {
    // Standard Setup
    if (argc <= 2) {
        std::cout << "Usage: " << argv[0] << " [-n] data_file threshold\n"
                     "   -n  Generate with vertex normals\n"
                     "Note: Using '-' as the name of the data_file will take input form the standard input (then signal end of file using ctrl+D)"
                    //  "\nThis program reads in a 3D array of binary floating point values and generate a PolySet as output. "
                    //  "The coordinate system of the PolySet should correspond to the index positions of the data, "
                    //  "with x in the direction of increasing columns, y in the direction of increasing rows, and z in the direction of increasing planes. "
                    //  "This program takes a threshold value from the user which will be used to generate the appropriate isosurface in the data. "
                    //  "Also, show_normals if just a flag to determine if surface normals should be generated for each vertex in the polyset."
        << std::endl;
        return 0;
    }

    int argi = 1;
    bool SHOW_NORMALS = false;
    if (argv[1][0] == '-' && argv[1][1] == 'n' && argv[1][2] == '\0') {
        SHOW_NORMALS = true;
        argi++;
    }

    // Variables
    float THRESHOLD = strtof(argv[argi+1], nullptr);
    if (THRESHOLD == NAN) {
        std::cerr << "Invalid threshold: " << argv[argi+1] << std::endl;
        return 2;
    }

    // Prepare Input (Optional: read from stdin if file name is "-" (until ctrl + D))
    std::ifstream inp_file; // Input file
    std::istream* inp = nullptr; // Input stream
    if (argv[argi][0] == '-' && argv[argi][1] == '\0') {
        inp = &std::cin; // Connect to input stream
    } else {
        inp_file.open(argv[argi], std::ios::in); // File checking
        if (!inp_file.is_open()) {
            std::cerr << "Error opening input file: " << argv[argi] << std::endl;
            return 1;
        }
        inp = &inp_file; // File is valid, connect to file stream
    }

    // Read input
    // First read the 3 sizes (ex. 20 20 20)
    size_t COLS, ROWS, PLANES; // cols, rows, planes (x,y,z)
    *inp >> COLS >> ROWS >> PLANES;
    inp->ignore(); // ignore the exact 1 byte of whitespace

    // Then read all float data (4 bytes)
    const size_t SIZE = COLS*ROWS*PLANES;
    float* data = new float[SIZE];
    inp->read(reinterpret_cast<char*>(data), std::streamsize(SIZE) * std::streamsize(sizeof(float))); // std::streamsize casts to silence compiler

    // std::cin.ignore();
    // if (std::cin.eof()) // Perfect, tho if the file is too big just ignore the rest of the numbers and carry on

    // Clean Up input
    if (inp != &std::cin) // Input
        inp_file.close();


    // Setup
    constexpr size_t SIZEOF_VERTEX = 3; // x,y,z vertex positions
    constexpr size_t SIZEOF_FACE   = 3; // 3 points per triangle

    const size_t VERTICES_SIZE = SIZE * SIZEOF_VERTEX * 6; // Max amount of duplicates is probably 6 (or 12? 24?)
    const size_t FACES_SIZE    = SIZE * SIZEOF_FACE   * 4; // vertex_ids in pairs of 3 (max: 4)
    const size_t ROWS_COLS     = ROWS * COLS;

    int  * faces    = new   int[FACES_SIZE];
    float* vertices = new float[VERTICES_SIZE];
    float* normals  = nullptr; // Only if SHOW_NORMALS is true
    if (SHOW_NORMALS) normals = new float[VERTICES_SIZE];

    // unsigned char needed[256] = {}; // Temporary

    // Main Loop
    int nface = 0;
    int nvertex = 0;
    for         (size_t z = 0; z < PLANES-1; z++) {
        for     (size_t y = 0; y <   ROWS-1; y++) {
            for (size_t x = 0; x <   COLS-1; x++) {
                // Cube Data
                const size_t corners[8][3] = {
                    {x  , y  , z  }, // 0
                    {x+1, y  , z  }, // 1
                    {x+1, y+1, z  }, // 2
                    {x  , y+1, z  }, // 3
                    {x  , y  , z+1}, // 4
                    {x+1, y  , z+1}, // 5
                    {x+1, y+1, z+1}, // 6
                    {x  , y+1, z+1}, // 7
                };

                // Cube corner vertex positions
                const float X_COORD = float(x);
                const float Y_COORD = float(y);
                const float Z_COORD = float(z);
                float coordinates[8][3] = {
                    {X_COORD,   Y_COORD,   Z_COORD  }, // 0: (0, 0, 0)
                    {X_COORD+1, Y_COORD,   Z_COORD  }, // 1: (1, 0, 0)
                    {X_COORD+1, Y_COORD+1, Z_COORD  }, // 2: (1, 1, 0)
                    {X_COORD,   Y_COORD+1, Z_COORD  }, // 3: (0, 1, 0)
                    {X_COORD,   Y_COORD,   Z_COORD+1}, // 4: (0, 0, 1)
                    {X_COORD+1, Y_COORD,   Z_COORD+1}, // 5: (1, 0, 1)
                    {X_COORD+1, Y_COORD+1, Z_COORD+1}, // 6: (1, 1, 1)
                    {X_COORD,   Y_COORD+1, Z_COORD+1}, // 7: (0, 1, 1)
                };

                // Setup for bits
                float values[8];
                for (size_t i = 0; i < 8; i++) {
                    const size_t* c = corners[i];
                    values[i] = data[ROWS_COLS*c[2] + COLS*c[1] + c[0]]; // data[z][y][x]
                }

                // Set bits
                unsigned char index = 0;
                for (size_t i = 0; i < 8; i++)
                    if (values[i] > THRESHOLD)
                        index |= (1 << i);

                // Quick Check
                if (index == 0 || index == 255)
                    continue; // No data, skip


                // Loop through faces from lookup table using index
                size_t edge_count;
                float edge_vertices[12][3];
                float edge_normals[12][3]; // Only used if SHOW_NORMALS is true
                const char* triangles = TRIANGLE_TABLE[index];

                // Temporary
                // if (triangles[0] == -1) // Missing
                //     needed[index] = index;

                for (edge_count = 0; edge_count < 12; edge_count++) {
                    char edge_number = triangles[edge_count];
                    if (edge_number == -1) break; // early eof

                    // Convert edge_number to cube edge vertex indicies
                    int c1, c2;
                    switch (edge_number) {          // c1   edge   c2
                    case 0:  c1 = 0; c2 = 1; break; // 0 ----0----> 1
                    case 1:  c1 = 1; c2 = 2; break; // 1 ----1----> 2
                    case 2:  c1 = 2; c2 = 3; break; // 2 ----2----> 3
                    case 3:  c1 = 3; c2 = 0; break; // 3 ----3----> 0
                    case 4:  c1 = 4; c2 = 5; break; // 4 ----4----> 5
                    case 5:  c1 = 5; c2 = 6; break; // 5 ----5----> 6
                    case 6:  c1 = 6; c2 = 7; break; // 6 ----6----> 7
                    case 7:  c1 = 7; c2 = 4; break; // 7 ----7----> 4
                    case 8:  c1 = 0; c2 = 4; break; // 0 ----8----> 4
                    case 9:  c1 = 1; c2 = 5; break; // 1 ----9----> 5
                    case 10: c1 = 2; c2 = 6; break; // 2 ----10---> 6
                    case 11: c1 = 3; c2 = 7; break; // 3 ----11---> 7
                    default: c1 =-1; c2 =-1; break; // Should never happen
                    }


                    // Vertex Interpolation
                    const float* p1 = coordinates[c1];
                    const float* p2 = coordinates[c2];
                    const float t = (THRESHOLD - values[c1]) / (values[c2] - values[c1]);

                    edge_vertices[edge_count][0] = p1[0] + t*(p2[0] - p1[0]);
                    edge_vertices[edge_count][1] = p1[1] + t*(p2[1] - p1[1]);
                    edge_vertices[edge_count][2] = p1[2] + t*(p2[2] - p1[2]);


                    // Normal Interpolation
                    if (!SHOW_NORMALS) continue;
                    const size_t* v1 = corners[c1];
                    const size_t* v2 = corners[c2];

                    // Calculate the gradients in all directions for both corner edges (while handling edge-cases)
                    float gX1 = calculate_gradient(v1, 0, data,PLANES,ROWS,COLS);
                    float gY1 = calculate_gradient(v1, 1, data,PLANES,ROWS,COLS);
                    float gZ1 = calculate_gradient(v1, 2, data,PLANES,ROWS,COLS);
                    float gX2 = calculate_gradient(v2, 0, data,PLANES,ROWS,COLS);
                    float gY2 = calculate_gradient(v2, 1, data,PLANES,ROWS,COLS);
                    float gZ2 = calculate_gradient(v2, 2, data,PLANES,ROWS,COLS);

                    // Interpolate
                    gX1 = gX1 + t*(gX2 - gX1);
                    gY1 = gY1 + t*(gY2 - gY1);
                    gZ1 = gZ1 + t*(gZ2 - gZ1);

                    // Normalize
                    float m = 1.f / std::sqrt(gX1*gX1 + gY1*gY1 + gZ1*gZ1);
                    edge_normals[edge_count][0] = gX1 * m;
                    edge_normals[edge_count][1] = gY1 * m;
                    edge_normals[edge_count][2] = gZ1 * m;
                }

                // Create vertices and faces (and normals if SHOW_NORMALS is true)
                for (size_t i = 0; i < edge_count; i+=3) {
                    int v = nvertex*3; // SIZEOF_VERTEX
                    vertices[v++] = edge_vertices[i  ][0];
                    vertices[v++] = edge_vertices[i  ][1];
                    vertices[v++] = edge_vertices[i  ][2];
                    vertices[v++] = edge_vertices[i+1][0];
                    vertices[v++] = edge_vertices[i+1][1];
                    vertices[v++] = edge_vertices[i+1][2];
                    vertices[v++] = edge_vertices[i+2][0];
                    vertices[v++] = edge_vertices[i+2][1];
                    vertices[v++] = edge_vertices[i+2][2];
                    nvertex += 3;

                    // So simple, it's done below
                    // size_t f = nface*SIZEOF_FACE;
                    // faces[f++] = f;
                    // faces[f++] = f;
                    // faces[f++] = f;
                    nface++;

                    if (!SHOW_NORMALS) continue;
                    normals[--v] = edge_normals[i+2][2];
                    normals[--v] = edge_normals[i+2][1];
                    normals[--v] = edge_normals[i+2][0];
                    normals[--v] = edge_normals[i+1][2];
                    normals[--v] = edge_normals[i+1][1];
                    normals[--v] = edge_normals[i+1][0];
                    normals[--v] = edge_normals[i  ][2];
                    normals[--v] = edge_normals[i  ][1];
                    normals[--v] = edge_normals[i  ][0];
                }
            }
        }
    }

    // Fill faces (all vertices lined up one-by-one)
    for (int i = 0, I = nface*3; i < I; i++)
        faces[i] = i;

    // Temporary
    // for (size_t i = 0; i < 256; i++)
    //     if (needed[i])
    //         std::cout << unsigned(needed[i]) << ' ';

    // Optional: Simplify vertices sequentially along with the faces
    if (true) {
        constexpr float NULL_VALUE = -1;
        for (int i = 0, i3 = 0, I = nvertex*3; i3 < I; i++, i3 += 3) {
            float v0 = vertices[i3];
            if (v0 == NULL_VALUE) continue;
            float v1 = vertices[i3+1];
            float v2 = vertices[i3+2];

            for (int j = i+1, j3 = i3+3; j3 < I; j++, j3 += 3) {
                float v;
                v = vertices[j3];   if (v != v0) continue; // includes v != -1
                v = vertices[j3+1]; if (v != v1) continue;
                v = vertices[j3+2]; if (v != v2) continue;

                // If here, then this is duplicate
                vertices[j3] = NULL_VALUE; // "delete"
                faces[j] = i; // update (i == faces[i] at this point)

                // Combine vertex data with the original (normalized)
                if (!SHOW_NORMALS) continue;
                float x = normals[i3]   + normals[j3];
                float y = normals[i3+1] + normals[j3+1];
                float z = normals[i3+2] + normals[j3+2];
                const float m = 1.f / std::sqrt(x*x + y*y + z*z);
                normals[i3]   = x * m;
                normals[i3+1] = y * m;
                normals[i3+2] = z * m;
            }
        }
        int n = 0;
        for (int i = 0, i3 = 0, I = nvertex*3; i3 < I; i++, i3 += 3) {
            if (vertices[i3] == NULL_VALUE) {
                // everything > n gets decremented
                for (int j = i; j < nface*3; j++) {
                    if (faces[j] > n) {
                        faces[j]--;
                    }
                }
            } else {
                // new (better) position
                const int n3 = n*3;
                vertices[n3]   = vertices[i3];
                vertices[n3+1] = vertices[i3+1];
                vertices[n3+2] = vertices[i3+2];
                if (SHOW_NORMALS) {
                    normals[n3]   = normals[i3];
                    normals[n3+1] = normals[i3+1];
                    normals[n3+2] = normals[i3+2];
                }
                n++;
            }
        }
        nvertex = n; // update
    }


    // Output to stdout
    std::cout << "PolySet \"" << (SHOW_NORMALS ? "PN" : "P") << "\"\n"
              << nvertex << " # Vertices\n" // optional
              << nface   << " # Faces\n\n"; // comments

    // Vertices
    for (int i = 0, I = nvertex*3; i < I; i+=3) { // 3 is vertex size
        std::cout << vertices[i]
           << ' ' << vertices[i+1]
           << ' ' << vertices[i+2];

        if (SHOW_NORMALS)
            std::cout << ' ' << normals[i]
                      << ' ' << normals[i+1]
                      << ' ' << normals[i+2];

        std::cout << '\n';
    }
    std::cout << '\n';

    // Faces
    for (int i = 0, I = nface*3; i < I; i += 3) { // 3 is the number of vertices per polygon (triangle)
        std::cout << faces[i]
           << ' ' << faces[i+1]
           << ' ' << faces[i+2]
           << " -1\n";
    }

    // Clean up
    delete[] data;
    delete[] faces;
    delete[] vertices;
    if (SHOW_NORMALS)
        delete[] normals;
    return 0;
}