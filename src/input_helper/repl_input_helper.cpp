//
// Created by CGrakeski on 2026/1/24.
//

#include <iosfwd>
#include <string>
#include "repl_input_helper.hpp"
#include <windows.h>
#include <chrono>
#include <istream>

#include "kiz.hpp"

std::string debug_char(char c);

#include <chrono>
#include <windows.h>

std::string helper::get_whole_input(std::istream *is, std::ostream *os) {
    if (is == nullptr)
        throw std::runtime_error("istream pointer is null");

    std::string input;
    char ch;

    while (true) {
        ch = is -> get();
        // 检查Ctrl+Enter或Shift+Enter组合键来结束输入
        if (((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 || (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0) && ch == '\n') {
            std::string result = input + ch;
            DEBUG_OUTPUT("final returns input: " << result);
            return result;
        } else if (ch == '\n') {
            os -> put('.');
            os -> put('.');
            os -> put('.');
            os -> put(' ');
            input += ch;
        } else {
            input += ch;
        }
    }
}