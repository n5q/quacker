#include "Quacker.hpp"

Quacker::Quacker() {
}

Quacker::~Quacker() {
  std::system("clear");
}

void Quacker::run() {
  startPage();
}

void Quacker::startPage() {
  while (this->_user_id.empty()) {
    std::system("clear");

    char select;
    std::cout << QUACKER_BANNER << "\n1. Log in\n2. Sign up\n3. Exit\n\nSelection: ";
    std::cin >> select;
    switch (select) {
      case '1':
        loginPage();
        break;

      default:
        break;
    }
  }
}

void Quacker::loginPage() {
  while (this->_user_id.empty()) {
    std::system("clear");

    std::string user_id;
    std::string password;
    
    std::cout << QUACKER_BANNER << "\n--- Log In ---\nUser ID: ";
    std::cin >> user_id;

    std::cout << "Password: ";
    std::cin >> password;
    /*CHECK FOR VALIDITY*/
  }
}

void Quacker::signupPage() {
  while (this->_user_id.empty()) {
    std::system("clear");
    std::cout << QUACKER_BANNER << "\n--- Sign Up ---\n";

    std::string user_id;
    std::string email;
    std::string phone;
    std::string password;
  }
}