#include <filesystem>
#include <iostream>

#include "definitions.hh"
#include "Quacker.hh"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Incorrect Usage: Expected quacker <filename>" << std::endl;
    return ERROR_USAGE;
  } else if (!std::filesystem::exists(argv[1])) {
    std::cerr << "File Not Found: Cannot find database " << argv[1] << std::endl;
    return ERROR_FILE;
  }
  
  Quacker quacker(argv[1]);
  quacker.run();
}