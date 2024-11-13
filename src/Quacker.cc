#include "Quacker.hh"

Quacker::Quacker(const std::string& db_filename) {
  if (pond.loadDatabase(db_filename)) {
    std::cerr << "Database Error: Could Not Open" << db_filename << std::endl;
    exit(-3);
  }
}

Quacker::~Quacker() {
  std::system("clear");
  delete _user_id;
}

void Quacker::run() {
  startPage();
}

void Quacker::startPage() {
  while (this->_user_id == nullptr) {
    std::system("clear");

    char select;
    std::cout << QUACKER_BANNER << "\n1. Log in\n2. Sign up\n3. Exit\n\nSelection: ";
    std::cin >> select;
    switch (select) {
      case '1':
        loginPage();
        break;
      case '2':
        signupPage();
        break;
      case '3':
        exit(0);
        break;

      default:
        break;
    }
  }
}

void Quacker::loginPage() {
  std::system("clear");
  std::cout << QUACKER_BANNER << "\n--- Log In ---\nUser ID: ";

  while (this->_user_id == nullptr) {
    uint32_t user_id;
    std::string password;

    do {
      if (std::cin >> user_id) {
        break;
      } else {
        std::cin.clear();
        std::system("clear");
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << QUACKER_BANNER << "\n--- Log In ---\nInvalid input. Please enter a valid number for your 'User ID'.\n\nUser ID: ";
      }
    } while (true);

    std::cout << "Password: ";
    if (std::cin >> password && pond.checkLogin(user_id, password)) {
      this->_user_id = new uint32_t(user_id);
    } else {
      std::cin.clear();
      std::system("clear");
      std::cout << QUACKER_BANNER << "\n--- Log In ---\nInvalid login credentials. Please enter a valid 'User ID' and 'Password'.\n\nUser ID: ";
    }
  }
}

void Quacker::signupPage() {
  while (this->_user_id == nullptr) {
    std::system("clear");
    std::cout << QUACKER_BANNER << "\n--- Sign Up ---\n";

    std::string user_id;
    std::string email;
    std::string phone;
    std::string password;
  }
}