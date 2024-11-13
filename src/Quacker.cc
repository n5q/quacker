#include "Quacker.hh"
#include <regex>

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
    int32_t user_id;
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
      this->_user_id = new int32_t(user_id);
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

    std::string name, email, phone_str, password;
    int phone_number;

    // Get and validate the name
    std::cout << "Enter Name: ";
    std::getline(std::cin >> std::ws, name);  // Using std::ws to ignore leading whitespace
    if (name.empty()) {
      std::cout << "Name cannot be empty. Press Enter to retry.";
      std::cin.get();
      continue;
    }

    // Get and validate the email
    std::cout << "Enter Email: ";
    std::cin >> email;
    if (!isValidEmail(email)) {
      std::cout << "Invalid email format. Press Enter to retry.";
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cin.get();
      continue;
    }

    // Get and validate the phone number
    std::cout << "Enter Phone Number: ";
    std::cin >> phone_str;
    phone_number = isValidPhoneNumber(phone_str);
    if (phone_number == -1) {
      std::cout << "Invalid phone number format. Press Enter to retry.";
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cin.get();
      continue;
    }

    // Get and validate the password
    std::cout << "Enter Password: ";
    std::cin >> password;
    if (password.empty()) {
      std::cout << "Password cannot be empty. Press Enter to retry.";
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      std::cin.get();
      continue;
    }

    // Add user to the database
    int32_t* new_user_id = pond.addUser(name, email, phone_number, password);

    if (new_user_id != nullptr) {
        // Assign the new user ID to this->_user_id
        this->_user_id = new_user_id;
        std::cout << "Signup successful! Press Enter to log in.";
        std::cin.get();
        return;
    } else {
        // Print an error message if signup was unsuccessful
        std::cout << "Error during signup. Please try again.";
        std::cin.get();
    }
  }
}

int Quacker::isValidPhoneNumber(const std::string& input) {
  if (input.find('\n') != std::string::npos || input.find('\r') != std::string::npos) {
    return -1;
  }

  std::string digits;

  for (char c : input) {
    if (std::isdigit(c)) {
      digits += c;
    }
  }

  if (digits.size() < 10 || digits.size() > 11) {
    return -1;
  }

  try {
    return std::stoi(digits);
  } catch (const std::out_of_range&) {
    return -1;
  }
}

bool Quacker::isValidEmail(const std::string& email) {
  if (email.find('\n') != std::string::npos || email.find('\r') != std::string::npos) {
    return false;
  }

  const std::regex email_pattern(R"((^[A-Za-z0-9._%+-]+)@([A-Za-z0-9.-]+)\.([A-Za-z]{2,6}$))");
  return std::regex_match(email, email_pattern);
}