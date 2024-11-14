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
    
    if (std::cin.peek() != '\n') select = '0';
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
        error = "\nIvalid Input Entered [use: 1, 2, 3]\n";
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
  std::string description = "Enter your details or press Enter to return.";
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
  int32_t FeedDisplayCount = 5;
  while (loged_in) {
    std::system("clear");
    
    std::string username = pond.getUsername(*(this->_user_id));

    char select;
    std::cout << QUACKER_BANNER << "\nWelcome back, " << username << "!\n\n-------------------------------------------- Your Feed --------------------------------------------\n\n";
    std::cout << processFeed(*(this->_user_id), FeedDisplayCount, error);
    std::cout << "\n" << error << "\n\n1. See More Of My Feed\n2. See Less Of My Feed\n3. Do Stuff\n4. Exit\n\nSelection: ";
    std::cin >> select;
    if (std::cin.peek() != '\n') select = '0';
    // Consume any trailing '\n' and discard it
    { std::string dummy; std::getline(std::cin, dummy); }
    switch (select) {
      case '1':
        FeedDisplayCount += 5;
        error = "";
        break;
      case '2':
        FeedDisplayCount -= 5;
        error = "";
        break;
      case '3':
        FeedDisplayCount = 5;
        error = "";
        break;
      case '4':
        std::system("clear");
        FeedDisplayCount = 5;
        error = "";
        
        loged_in = false;
        delete this->_user_id;
        this->_user_id = nullptr;
        break;

      default:
        error = "\nIvalid Input Entered [use: 1, 2, 3, 4]\n";
        break;
    }
  }
  
  startPage();
}

std::string Quacker::processFeed(const std::int32_t& user_id, int32_t& FeedDisplayCount, std::string& error) {
    std::vector<std::string> feed = pond.getFeed(user_id);

    int32_t maxTweets = feed.size();

    if (FeedDisplayCount >= maxTweets + 5) {
        // Case 1: FeedDisplayCount is 5 or more beyond the available tweets
        error = "\nYou Have No More Tweets Left To Display\n";
        FeedDisplayCount = std::max(0, static_cast<int>(FeedDisplayCount) - 5);
        std::ostringstream oss;
        for (int32_t i = 0; i < maxTweets; ++i) {
            oss << feed[i] << "\n";
            for(int i = 0; i < 100; ++i) oss << '-'; 
            oss << '\n';

        }
        return oss.str();
    } else if (FeedDisplayCount >= maxTweets && FeedDisplayCount <= maxTweets + 4) {
        // Case 2: FeedDisplayCount is between maxTweets and maxTweets + 4
        // Do not modify FeedDisplayCount, but limit the display to maxTweets
    } else if (FeedDisplayCount < maxTweets && FeedDisplayCount > 0) {
        // Case 3: FeedDisplayCount is between 0 and maxTweets
        // FeedDisplayCount remains as is
    } else if (FeedDisplayCount <= 0) {
        // Case 4: FeedDisplayCount is less than zero
        
        if(FeedDisplayCount != 0) error = "\nYou Are Already Not Displaying Any Tweets\n";
        FeedDisplayCount = 0;
        return "";
    }

    int32_t displayCount = std::min(FeedDisplayCount, maxTweets);

    std::ostringstream oss;
    for (int32_t i = 0; i < displayCount; ++i) {
        oss << feed[i] << "\n";
        for(int i = 0; i < 100; ++i) oss << '-'; 
        oss << '\n';
    }

    return oss.str();
}