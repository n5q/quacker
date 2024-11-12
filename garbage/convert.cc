#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

std::string escapeString(const std::string& input) {
    std::ostringstream escaped;
    for (char c : input) {
        switch (c) {
        case '\n': escaped << "\\n\" \n\""; break; // newline escape
        case '\t': escaped << "\\t"; break;        // tab escape
        case '\\': escaped << "\\\\"; break;       // backslash escape
        case '\"': escaped << "\\\""; break;       // double quote escape
        default: escaped << c;
        }
    }
    return escaped.str();
}

int main() {
    std::ifstream file("quack.txt");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file." << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf(); // Read the entire file
    std::string banner = buffer.str();
    file.close();

    // Escape the banner string
    std::string escapedBanner = escapeString(banner);

    // Write the escaped string to a new text file
    std::ofstream output("banner_output.txt");
    if (!output.is_open()) {
        std::cerr << "Error: Could not create output file." << std::endl;
        return 1;
    }

    // Format the string as a C++ literal
    output << "static const std::string QUACKER_BANNER  = \"";
    output << escapedBanner;
    output << "\";\n";
    output.close();

    std::cout << "Banner saved to 'banner_output.txt' as a C++ string literal." << std::endl;

    return 0;
}
