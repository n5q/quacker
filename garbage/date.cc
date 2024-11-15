#include <iostream>
#include <chrono>

int main() {
std::time_t rn = std::time(nullptr);
  std::tm* gmt = std::gmtime(&rn);

  char* t = new char[11];
  // yyyy-mm-dd
  std::strftime(t, 11, "%F", gmt);
  std::cout << t << std::endl;

}
