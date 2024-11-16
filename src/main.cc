#include <filesystem>
#include <iostream>

#include "definitions.hh"
#include "Quacker.hh"

/**
 * @brief Main function for the Quacker application.
 * 
 * This function initializes the Quacker application with a database file
 * specified via command-line arguments. It checks for proper usage and
 * the existence of the provided file before proceeding.
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return int Exit status code. Returns ERROR_USAGE for incorrect usage,
 *         ERROR_FILE if the file is not found, or 0 for success.
 */
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