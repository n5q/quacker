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

/**
 * @brief Displays the main start page for the Quacker application and prompts user actions.
 *
 * This function continually displays the main start page menu until the user logs in or exits.
 * While `_user_id` remains `nullptr`, the menu provides options to log in, sign up, or exit the program.
 * Each option triggers the corresponding page or action.
 *
 * The menu options include:
 * - **1. Log in**: Opens the login page.
 * - **2. Sign up**: Opens the signup page.
 * - **3. Exit**: Terminates the program.
 *
 * @note The screen is cleared before each menu display to keep the interface clean.
 */
void Quacker::startPage() {
  std::string error = "";
  while (this->_user_id == nullptr) {
    std::system("clear");

    char select;
    std::cout << QUACKER_BANNER << error << "\n1. Log in\n2. Sign up\n3. Exit\n\nSelection: ";
    std::cin >> select;
    // Consume any trailing '\n' and discard it
    { std::string dummy; std::getline(std::cin, dummy); }
    switch (select) {
      case '1':
        error = "";
        loginPage();
        break;
      case '2':
        error = "";
        signupPage();
        
        break;
      case '3':
        std::system("clear");
        error = "";
        exit(0);
        break;

      default:
        error = "\nIvalid Input Entered [eg. 2]\n";
        break;
    }
  }
}

/**
 * @brief Displays the login page and prompts the user for credentials.
 * 
 * @note The credentials are verified using the `checkLogin` method, and if successful,
 *       the `_user_id` member is updated with the user ID.
 */
void Quacker::loginPage() {
  std::string description = "Enter login credentials or press Enter to return.";

  while (true) {
    // Clear the screen and show the login interface
    std::system("clear");
    std::cout << QUACKER_BANNER << "\n" << description << "\n\n--- Log In ---\n" << "\nUser ID: ";

    std::string user_id_str;
    int32_t user_id;
    std::string password;

    // Get the user ID input from the user
    std::getline(std::cin, user_id_str);
    if (user_id_str.empty()) {
      return;
    } else if (isID(user_id_str)) {
      try {
        user_id = std::stoi(user_id_str);
      } catch (const std::out_of_range&) {
        description = "Invalid User ID, ID must be a valid integer.";
        continue;
      }
    } else {
      description = "Invalid User ID, ID must be a valid integer.";
      continue;
    }

    // Ask for the password
    std::cout << "Password: ";
    std::getline(std::cin, password);

    // Check login credentials
    this->_user_id = pond.checkLogin(user_id, password);

    // If credentials are invalid, prompt the user to try again
    if (_user_id == nullptr) {
      description = "Invalid credentials, please enter a valid 'User ID' and 'Password', or press Enter to return.";
      continue;
    }
    break;
  }
  loged_in = true;
  mainPage();
}

/**
 * @brief Displays the sign-up page and prompts the user for details to create a new account.
 * 
 * @note The function utilizes `isValidEmail` for email validation, `isValidPhoneNumber` for phone number validation,
 *       and `pond.addUser` to add the new user to the database.
 */
void Quacker::signupPage() {
  std::string description = "Enter your details or press Enter to return...";
  while (true) {
    // Clear the screen and show the sign-up interface
    std::system("clear");
    std::cout << QUACKER_BANNER << "\n" << description << "\n\n--- Sign Up ---\n";

    std::string name, email, phone_str, password;
    long int phone_number;

    // Get and validate the name
    std::cout << "Enter Name: ";
    std::getline(std::cin, name);
    name = trim(name);
    if (name.empty()) return;

    // Get and validate the email
    std::cout << "Enter Email: ";
    std::getline(std::cin, email);
    if (email.empty()) return;
    if (!isValidEmail(email)) {
      description = "Invalid email format, please try again [eg. example@ualberta.ca]";
      continue;
    }

    // Get and validate the phone number
    std::cout << "Enter Phone Number: ";
    std::getline(std::cin, phone_str);
    if (phone_str.empty()) return;
    phone_number = isValidPhoneNumber(phone_str);
    if (phone_number == -1) {
      description = "Invalid phone number format, please try again [eg. 510-827-7791].";
      continue;
    }

    // Get and validate the password
    std::cout << "Enter Password: ";
    std::getline(std::cin, password);
    if (password.empty()) return;

    // Add user to the database
    int32_t* new_user_id = pond.addUser(name, email, phone_number, password);
    
    // If the user is successfully added, assign the new user ID to _user_id and notify the user
    if (new_user_id != nullptr) {
      this->_user_id = new_user_id;
      std::cout << "Account created! Press Enter to log in....\n";
      std::cin.get();
      loged_in = true;
      mainPage();
      break;
    } else {
        description = "Error during signup, please try again.\n";
    }
  }
}

/*
  * @brief Displays the user search page and prompts the user to enter a search term.
  *
  * The search term should be a portion of a user's name. The search results
  * are displayed to the user 5 at a time, showing matching users.
*/
void Quacker::userSearchPage() {
  std::string description = "Search for a user or press Enter to return.";
  while (true) {
    // show search interface
    std::system("clear");
    std::cout << QUACKER_BANNER << "\n" << description << "\n\n--- User Search ---\n";

    std::string search_term;
    std::cout << "Search for user name: ";
    std::getline(std::cin, search_term);
    search_term = trim(search_term);
    if (search_term.empty()) return;

    // query
    std::vector<Pond::User> results = pond.searchForUsers(search_term);

    // display results
    if (results.empty()) {
      std::cout << "No users found matching the search term.\n";
    } else {
      std::cout << "Found " << results.size() << " users matching the search term.\n";
      for (const Pond::User& result : results) {
        std::cout << "User ID: " << result.usr << ", Name: " << result.name << std::endl;
      }
    }

    // Prompt the user to search again or return
    std::cout << "\nPress Enter to return...";
    std::string input;
    std::getline(std::cin, input);
    if (input.empty()) {
      continue;
    }
    else {
      break;
    }
  }
}

/*
  * @brief Displays the tweet search page and prompts the user to enter a search term.
  *
  * The search term should be a keyword or a hashtag prefixed with '#'. The search results
  * are displayed to the user 5 at a time, showing matching users.
*/
void Quacker::quackSearchPage() {
  std::string description = "Search for a keyword or hashtag, or press Enter to return...";
  while (true) {
    // show search interface
    std::system("clear");
    std::cout << QUACKER_BANNER << "\n" << description << "\n\n--- Quack Search ---\n";

    std::string search_term;
    std::cout << "Search for a quack: ";
    std::getline(std::cin, search_term);
    search_term = trim(search_term);
    if (search_term.empty()) return;

    // query
    std::vector<Pond::Tweet> results = pond.searchForTweets(search_term);

    // display results
    if (results.empty()) {
      std::cout << "No quacks found matching the search term.\n";
    }
    else {
      std::cout << "Found " << results.size() << " quacks matching the search term.\n";
      for (const Pond::Tweet& result : results) {
        std::cout << "Quack ID: " << result.tid << ", Writer ID: " << result.writer_id
                  << ", Text: " << result.text << ", Date: " << result.date
                  << ", Time: " << result.time << std::endl;
        if (result.replyto_tid != 0) {
          std::cout << "Reply to Quack ID: " << result.replyto_tid << std::endl;
        }
      }
    }

    // Prompt the user to search again or return
    std::cout << "\nPress Enter to return...";
    std::string input;
    std::getline(std::cin, input);
    if (input.empty()) {
      continue;
    }
    else {
      break;
    }
  }
}


/**
 * @brief Validates a phone number string and returns its numeric value.
 *
 * Extracts digits from the input string and checks if it contains 10 or 11 digits,
 * excluding any newline or carriage return characters. Returns the numeric value
 * if valid, otherwise returns -1.
 *
 * @param input The phone number string to validate.
 * @return The numeric value of the phone number if valid, -1 if invalid.
 */
int64_t Quacker::isValidPhoneNumber(const std::string& input) {
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
    return std::stoll(digits);
  } catch (const std::out_of_range&) {
    std::cout << "out of range\n";
    std::cin.get();
    return -1;
  }
}

/**
 * @brief Validates an email address format.
 *
 * This function checks if the given email string adheres to a standard email
 * format and ensures it does not contain any newline or carriage return
 * characters, which are considered invalid.
 *
 * @param email The email address to validate.
 * @return true if the email address is in a valid format, false otherwise.
 *
 * @note The regex used is sourced from https://emailregex.com/
 */
bool Quacker::isValidEmail(const std::string& email) {
  if (email.find('\n') != std::string::npos || email.find('\r') != std::string::npos) {
    return false;
  }

  const std::regex email_pattern(R"((?:[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*|"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\]))");
  return std::regex_match(email, email_pattern);
}

/**
 * @brief Determines if a string is a valid integer ID.
 *
 * If the entire
 * string is successfully parsed as an integer (i.e., no non-numeric
 * characters remain), it is considered a valid ID.
 *
 * @param str The string to check.
 * @return true if the string is a valid integer, false otherwise.
 */
bool Quacker::isID(std::string str) {
  char* p;
  strtol(str.c_str(), &p, 10);
  return *p == 0;
}

/**
 * @brief Trims leading and trailing whitespace from a string.
 * 
 * @param str The string to be trimmed.
 * @return A new string with leading and trailing whitespace removed.
 * 
 * @note This function does not modify the original string but instead returns a new string with the whitespace removed.
 */
std::string Quacker::trim(const std::string& str) {
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

void Quacker::mainPage() {
  std::string error = "";
  while (loged_in) {
    std::system("clear");
    
    std::string username = pond.getUsername(*(this->_user_id));

    char select;
    std::cout << QUACKER_BANNER << "\nWelcome back, " << username << "!\n" << error <<
      "\n1. Search for users"
      "\n2. Search for quacks"
      "\n3. Exit"
      "\n\nSelection: ";
    std::cin >> select;
    // Consume any trailing '\n' and discard it
    { std::string dummy; std::getline(std::cin, dummy); }
    switch (select) {
      case '1':
        this->userSearchPage();
        // error = "";
        break;
      case '2':
        this->quackSearchPage();
        // error = "";
        break;
      case '3':
        std::system("clear");
        error = "";
        
        loged_in = false;
        delete this->_user_id;
        this->_user_id = nullptr;
        break;

      default:
        error = "\nIvalid Input Entered [eg. 2]\n";
        break;
    }
  }
  
  startPage();
}