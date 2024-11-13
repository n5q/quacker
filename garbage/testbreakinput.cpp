#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>

std::string trim(const std::string& str) {
    // Find the first non-whitespace character from the start
    auto start = std::find_if_not(str.begin(), str.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });

    // Find the last non-whitespace character from the end
    auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base();

    // Return the trimmed string
    return (start < end) ? std::string(start, end) : std::string();
}

int main() {
    std::string s = "   Hello, World!   ";
    std::string trimmed = trim(s);
    std::cout << "[" << trimmed << "]" << std::endl;  // Output: [Hello, World!]
    return 0;
}