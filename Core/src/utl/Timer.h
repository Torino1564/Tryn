#pragma once
#include <chrono>
#include <iostream>

namespace ch = std::chrono;

#define ZT_TIME(arg) { auto start = ch::high_resolution_clock::now(); arg auto end = ch::high_resolution_clock::now(); auto duration = end - start; std::cout << duration << '\n';}

